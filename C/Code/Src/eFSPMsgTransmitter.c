/**
 * @file       eFSPMsgTransmitter.h
 *
 * @brief      Message transmitter utils
 *
 * @author     Lorenzo Rosin
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *      INCLUDES
 **********************************************************************************************************************/
#include "eFSPMsgTransmitter.h"



/***********************************************************************************************************************
 *  PRIVATE STATIC FUNCTION DECLARATION
 **********************************************************************************************************************/
static bool_t isMsgTransStatusStillCoherent(const s_eFSP_MsgTxCtx* ctx);
static e_eFSP_MsgTx_Res convertReturnFromMSGEToMSGTX(e_eFSP_MsgE_Res returnedEvent);



/***********************************************************************************************************************
 *   GLOBAL FUNCTIONS
 **********************************************************************************************************************/
e_eFSP_MsgTx_Res msgTransmInitCtx(s_eFSP_MsgTxCtx* const ctx, const s_eFSP_MsgTxInitData* initData)
{
	/* Local variable */
	e_eFSP_MsgTx_Res result;
	e_eFSP_MsgE_Res resultMsgE;

	/* Check pointer validity */
	if( ( NULL == ctx ) || ( NULL == initData ) )
	{
		result = MSGTTX_RES_BADPOINTER;
	}
	else
	{
        /* Check pointer validity */
        if( ( NULL == initData->i_memArea ) || ( NULL == initData->i_sendBuffArea ) || ( NULL == initData->i_cbCrcP ) ||
            ( NULL == initData->i_cbCrcCtx ) || ( NULL == initData->i_cbTxP ) || ( NULL == initData->i_cbTxCtx ) )
        {
            result = MSGTTX_RES_BADPOINTER;
        }
        else
        {
            /* Check data validity, we need some len to store the data */
            if( ( initData->i_sendBuffAreaSize < 1u ) || ( initData->i_frameTimeoutMs < 1u ) ||
                ( initData->i_timePerSendMs < 1u ) || ( initData->i_timePerSendMs > initData->i_frameTimeoutMs ) )
            {
                result = MSGTTX_RES_BADPARAM;
            }
            else
            {
                /* Initialize internal status variable */
                ctx->sendBuff = initData->i_sendBuffArea;
                ctx->sendBuffSize = initData->i_sendBuffAreaSize;
                ctx->sendBuffCntr = 0u;
                ctx->sendBuffFill = 0u;
                ctx->cbTxP = initData->i_cbTxP;
                ctx->cbTxCtx = initData->i_cbTxCtx;
                ctx->timeCounterMs = 0u;
                ctx->frameTimeoutMs = initData->i_frameTimeoutMs;
                ctx->timePerSendMs = initData->i_timePerSendMs;

                /* initialize internal bytestuffer */
                resultMsgE =  msgEncoderInitCtx(&ctx->msgEncoderCtnx, initData->i_memArea, initData->i_memAreaSize,
                                                initData->i_cbCrcP, initData->i_cbCrcCtx);
                result = convertReturnFromMSGEToMSGTX(resultMsgE);
            }
        }
	}

	return result;
}

e_eFSP_MsgTx_Res msgTransmStartNewMessage(s_eFSP_MsgTxCtx* const ctx, const uint32_t messageLen)
{
	/* Local variable */
	e_eFSP_MsgTx_Res result;
	e_eFSP_MsgE_Res resultMsgE;

	/* Check pointer validity */
	if( NULL == ctx )
	{
		result = MSGTTX_RES_BADPOINTER;
	}
	else
	{
        /* Check param validity, need at least 1 byte of paylaod */
        if( messageLen <= 0u )
        {
            result = MSGTTX_RES_BADPARAM;
        }
		else
		{
            /* Check internal status validity */
            if( false == isMsgTransStatusStillCoherent(ctx) )
            {
                result = MSGTTX_RES_CORRUPTCTX;
            }
            else
            {
                /* Reset internal variable */
                ctx->sendBuffCntr = 0u;
                ctx->sendBuffFill = 0u;
                ctx->timeCounterMs = 0u;

                /* Init message encoder */
                resultMsgE = msgEncoderStartNewMessage(&ctx->msgEncoderCtnx, messageLen);
                result = convertReturnFromMSGEToMSGTX(resultMsgE);
            }
		}
	}

	return result;
}

e_eFSP_MsgTx_Res msgTransmGetPayloadLocation(s_eFSP_MsgTxCtx* const ctx, uint8_t** dataP, uint32_t* const maxDataSize)
{
	/* Local variable */
	e_eFSP_MsgTx_Res result;
	e_eFSP_MsgE_Res resultMsgE;

	/* Check pointer validity */
	if( ( NULL == ctx ) || ( NULL == dataP ) || ( NULL == maxDataSize ) )
	{
		result = MSGTTX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == isMsgTransStatusStillCoherent(ctx) )
		{
			result = MSGTTX_RES_CORRUPTCTX;
		}
		else
		{
			/* Get memory reference of CRC+LEN+DATA, so we can calculate reference of only data payload */
			resultMsgE = msgEncoderGetPayloadLocation(&ctx->msgEncoderCtnx, dataP, maxDataSize);
			result = convertReturnFromMSGEToMSGTX(resultMsgE);
		}
	}

	return result;
}

e_eFSP_MsgTx_Res msgTransmRestartCurrentMessage(s_eFSP_MsgTxCtx* const ctx)
{
	/* Local variable */
	e_eFSP_MsgTx_Res result;
	e_eFSP_MsgE_Res resultMsgE;

	/* Check pointer validity */
	if( NULL == ctx )
	{
		result = MSGTTX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == isMsgTransStatusStillCoherent(ctx) )
		{
			result = MSGTTX_RES_CORRUPTCTX;
		}
		else
		{
            /* Reset internal variable */
            ctx->sendBuffCntr = 0u;
            ctx->sendBuffFill = 0u;
            ctx->timeCounterMs = 0u;

			/* Restart only the byte stuffer */
			resultMsgE = msgEncoderRestartCurrentMessage(&ctx->msgEncoderCtnx);
			result = convertReturnFromMSGEToMSGTX(resultMsgE);
		}
	}

	return result;
}

e_eFSP_MsgTx_Res msgTransmSendChunk(s_eFSP_MsgTxCtx* const ctx)
{
	/* Local variable */
	e_eFSP_MsgTx_Res result;
	e_eFSP_MsgE_Res resultMsgE;

    /* Local variable of data to send */
    uint32_t remainingTimeMs;
    uint32_t sendFramTimeTotal;
    uint32_t sendFramTimeSingle;
    uint8_t *cDataToSendP;
    uint32_t cDataToSendLen;
    uint32_t cDataSendedSingle;


	/* Check pointer validity */
	if( NULL == ctx )
	{
		result = MSGTTX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == isMsgTransStatusStillCoherent(ctx) )
		{
			result = MSGTTX_RES_CORRUPTCTX;
		}
		else
		{
            /* Init time frame counter */
            sendFramTimeTotal = 0u;
            result = MSGTTX_RES_OK;

            /* wait end */
            while( ( ctx->timeCounterMs < ctx->frameTimeoutMs ) && ( sendFramTimeTotal < ctx->timePerSendMs ) &&
                   ( result == MSGTTX_RES_OK ) )
            {
                /* Refresh remaining time */
                remainingTimeMs = ctx->frameTimeoutMs - ctx->timeCounterMs;

                if( ( ctx->timePerSendMs - sendFramTimeTotal ) < remainingTimeMs)
                {
                    remainingTimeMs = ctx->timePerSendMs - sendFramTimeTotal;
                }

                /* Is data present in send buffer? */
                cDataToSendLen = ctx->sendBuffFill - ctx->sendBuffCntr;

                if( cDataToSendLen > 0u )
                {
                    /* Can send data */
                    if( true == (*ctx->cbTxP)(ctx->cbTxCtx, cDataToSendP, cDataToSendLen, &cDataSendedSingle, remainingTimeMs, &sendFramTimeSingle) )
                    {
                        /* Data sended! update timeout and counter */
                    }
                    else
                    {
                        /* Error sending data */
                        result = MSGTTX_RES_TXCLBKERROR;
                    }
                }
                else
                {
                    /* Is data present in message encoder buffer? */

                    /* Data present -> extraxt */
                    /* Data non present -> we are done */
                }

            }

            /* Check for timeout */
            if( ( ctx->timeCounterMs < ctx->frameTimeoutMs ) && ( sendFramTimeTotal < ctx->timePerSendMs ) )
            {
                if( ( MSGTTX_RES_OK == result ) || ( MSGTTX_RES_MESSAGESENDED == result ) )
                {
                    /* Frame can go in timeout when we are not finished (RES_OK) or when all the message is sended
                     * ( RES_MESSAGESENDED ) */
                    result = MSGTTX_RES_MESSAGETIMEOUT;
                }
            }
		}
	}

	return result;
}

/***********************************************************************************************************************
 *  PRIVATE FUNCTION
 **********************************************************************************************************************/
bool_t isMsgTransStatusStillCoherent(const s_eFSP_MsgTxCtx* ctx)
{
    bool_t result;

	/* Check pointer validity */
	if( ( NULL == ctx->sendBuff ) || ( NULL == ctx->cbTxP ) || ( NULL == ctx->cbTxCtx ) )
	{
		result = false;
	}
	else
	{
        /* Check send buffer validity */
        if( ( ctx->sendBuffSize < 1u ) || ( ctx->sendBuffFill > ctx->sendBuffSize )  ||
            ( ctx->sendBuffCntr > ctx->sendBuffFill ) )
        {
            return false;
        }
        else
        {
            /* Check timings validity */
            if( ( ctx->frameTimeoutMs < 1u ) || ( ctx->timePerSendMs < 1u ) ||
                ( ctx->timePerSendMs > ctx->frameTimeoutMs ) )
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
	}

    return result;
}

e_eFSP_MsgTx_Res convertReturnFromMSGEToMSGTX(e_eFSP_MsgE_Res returnedEvent)
{
	e_eFSP_MsgTx_Res result;

	switch( returnedEvent )
	{
		case MSGE_RES_OK:
		{
			result = MSGTTX_RES_OK;
            break;
		}

		case MSGE_RES_BADPARAM:
		{
			result = MSGTTX_RES_BADPARAM;
            break;
		}

		case MSGE_RES_BADPOINTER:
		{
			result = MSGTTX_RES_BADPOINTER;
            break;
		}

		case MSGE_RES_CORRUPTCTX:
		{
			result = MSGTTX_RES_CORRUPTCTX;
            break;
		}

		case MSGE_RES_MESSAGEENDED:
		{
			result = MSGTTX_RES_MESSAGESENDED;
            break;
		}

		case MSGE_RES_NOINITLIB:
		{
			result = MSGTTX_RES_NOINITLIB;
            break;
		}

		case MSGE_RES_NOINITMESSAGE :
		{
			result = MSGTTX_RES_NOINITMESSAGE;
            break;
		}

		case MSGE_RES_CRCCLBKERROR :
		{
			result = MSGTTX_RES_CRCCLBKERROR;
            break;
		}

		default:
		{
            /* Impossible end here */
			result = MSGTTX_RES_CORRUPTCTX;
            break;
		}
	}

	return result;
}


