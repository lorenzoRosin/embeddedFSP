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
#include "eFSPMsgTransmitter_Priv.h"



/***********************************************************************************************************************
 *  PRIVATE STATIC FUNCTION DECLARATION
 **********************************************************************************************************************/
static bool_t eFSP_MSGTX_isStatusStillCoherent(const s_eFSP_MSGTX_Ctx* p_ctx);
static e_eFSP_MSGTX_Res eFSP_MSGTX_convertReturnFromMSGE(e_eFSP_MSGE_Res returnedEvent);



/***********************************************************************************************************************
 *   GLOBAL FUNCTIONS
 **********************************************************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_disable = "MISRAC2012-Rule-10.3"
    /* Suppressed because this warning is not working */
#endif

e_eFSP_MSGTX_Res eFSP_MSGTX_InitCtx(s_eFSP_MSGTX_Ctx* const p_ctx, const s_eFSP_MSGTX_InitData* p_initData)
{
	/* Local variable */
	e_eFSP_MSGTX_Res result;
	e_eFSP_MSGE_Res resultMsgE;

	/* Check pointer validity */
	if( ( NULL == p_ctx ) || ( NULL == p_initData ) )
	{
		result = MSGTX_RES_BADPOINTER;
	}
	else
	{
        /* Check pointer validity */
        if( ( NULL == p_initData->i_memArea ) || ( NULL == p_initData->i_sendBuffArea ) || ( NULL == p_initData->i_cbCrcP ) ||
            ( NULL == p_initData->i_cbCrcCtx ) || ( NULL == p_initData->i_cbTxP ) || ( NULL == p_initData->i_cbTxCtx ) ||
            ( NULL == p_initData->i_txTimer.timerCtx ) || ( NULL == p_initData->i_txTimer.tim_start ) ||
            ( NULL == p_initData->i_txTimer.tim_getRemaining ) )
        {
            result = MSGTX_RES_BADPOINTER;
        }
        else
        {
            /* Check data validity, we need some len to store the data */
            if( ( p_initData->i_sendBuffAreaSize < 1u ) || ( p_initData->i_frameTimeoutMs < 1u ) ||
                ( p_initData->i_timePerSendMs < 1u ) || ( p_initData->i_timePerSendMs > p_initData->i_frameTimeoutMs ) )
            {
                result = MSGTX_RES_BADPARAM;
            }
            else
            {
                /* Initialize internal status variable */
                p_ctx->sendBuff = p_initData->i_sendBuffArea;
                p_ctx->sendBuffSize = p_initData->i_sendBuffAreaSize;
                p_ctx->sendBuffCntr = 0u;
                p_ctx->sendBuffFill = 0u;
                p_ctx->cbTxP = p_initData->i_cbTxP;
                p_ctx->cbTxCtx = p_initData->i_cbTxCtx;
                p_ctx->txTimer =  p_initData->i_txTimer;
                p_ctx->frameTimeoutMs = p_initData->i_frameTimeoutMs;
                p_ctx->timePerSendMs = p_initData->i_timePerSendMs;

                /* initialize internal message encoder */
                resultMsgE =  eFSP_MSGE_InitCtx(&p_ctx->msgEncoderCtnx, p_initData->i_memArea, p_initData->i_memAreaSize,
                                           p_initData->i_cbCrcP, p_initData->i_cbCrcCtx);
                result = eFSP_MSGTX_convertReturnFromMSGE(resultMsgE);
            }
        }
	}

	return result;
}

#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_restore = "MISRAC2012-Rule-10.3"
#endif

e_eFSP_MSGTX_Res eFSP_MSGTX_IsInit(s_eFSP_MSGTX_Ctx* const p_ctx, bool_t* p_isInit)
{
	/* Local variable */
	e_eFSP_MSGTX_Res result;
	e_eFSP_MSGE_Res resultMsgE;

	/* Check pointer validity */
	if( ( NULL == p_ctx ) || ( NULL == p_isInit ) )
	{
		result = MSGTX_RES_BADPOINTER;
	}
	else
	{
        resultMsgE = eFSP_MSGE_IsInit(&p_ctx->msgEncoderCtnx, p_isInit);
        result = eFSP_MSGTX_convertReturnFromMSGE(resultMsgE);
	}

	return result;
}

e_eFSP_MSGTX_Res eFSP_MSGTX_GetWherePutData(s_eFSP_MSGTX_Ctx* const p_ctx, uint8_t** pp_data, uint32_t* const p_maxDataL)
{
	/* Local variable */
	e_eFSP_MSGTX_Res result;
	e_eFSP_MSGE_Res resultMsgE;

	/* Check pointer validity */
	if( ( NULL == p_ctx ) || ( NULL == pp_data ) || ( NULL == p_maxDataL ) )
	{
		result = MSGTX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == eFSP_MSGTX_isStatusStillCoherent(p_ctx) )
		{
			result = MSGTX_RES_CORRUPTCTX;
		}
		else
		{
			/* Get only the payload data reference */
			resultMsgE = eFSP_MSGE_GetWherePutData(&p_ctx->msgEncoderCtnx, pp_data, p_maxDataL);
			result = eFSP_MSGTX_convertReturnFromMSGE(resultMsgE);
		}
	}

	return result;
}

e_eFSP_MSGTX_Res eFSP_MSGTX_NewMessage(s_eFSP_MSGTX_Ctx* const p_ctx, const uint32_t messageLen)
{
	/* Local variable */
	e_eFSP_MSGTX_Res result;
	e_eFSP_MSGE_Res resultMsgE;

	/* Check pointer validity */
	if( NULL == p_ctx )
	{
		result = MSGTX_RES_BADPOINTER;
	}
	else
	{
        /* Check internal status validity */
        if( false == eFSP_MSGTX_isStatusStillCoherent(p_ctx) )
        {
            result = MSGTX_RES_CORRUPTCTX;
        }
		else
		{
            /* Check param validity, need at least 1 byte of paylaod */
            if( messageLen <= 0u )
            {
                result = MSGTX_RES_BADPARAM;
            }
            else
            {
                /* Reset internal variable */
                p_ctx->sendBuffCntr = 0u;
                p_ctx->sendBuffFill = 0u;

                /* Init message encoder */
                resultMsgE = eFSP_MSGE_NewMessage(&p_ctx->msgEncoderCtnx, messageLen);
                result = eFSP_MSGTX_convertReturnFromMSGE(resultMsgE);

                /* Start timer */
                if( MSGTX_RES_OK == result )
                {
                    if( true != p_ctx->txTimer.tim_start( p_ctx->txTimer.timerCtx, p_ctx->frameTimeoutMs ) )
                    {
                        result = MSGTX_RES_TIMCLBKERROR;
                    }
                }
            }
		}
	}

	return result;
}

e_eFSP_MSGTX_Res eFSP_MSGTX_RestartMessage(s_eFSP_MSGTX_Ctx* const p_ctx)
{
	/* Local variable */
	e_eFSP_MSGTX_Res result;
	e_eFSP_MSGE_Res resultMsgE;

	/* Check pointer validity */
	if( NULL == p_ctx )
	{
		result = MSGTX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == eFSP_MSGTX_isStatusStillCoherent(p_ctx) )
		{
			result = MSGTX_RES_CORRUPTCTX;
		}
		else
		{
            /* Reset internal variable */
            p_ctx->sendBuffCntr = 0u;
            p_ctx->sendBuffFill = 0u;

			/* Restart only the byte stuffer */
			resultMsgE = eFSP_MSGE_RestartMessage(&p_ctx->msgEncoderCtnx);
			result = eFSP_MSGTX_convertReturnFromMSGE(resultMsgE);

            /* Start timer */
            if( MSGTX_RES_OK == result )
            {
                if( true != p_ctx->txTimer.tim_start( p_ctx->txTimer.timerCtx, p_ctx->frameTimeoutMs ) )
                {
                    result = MSGTX_RES_TIMCLBKERROR;
                }
            }
		}
	}

	return result;
}

#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_disable = "MISRAC2012-Rule-10.3"
    /* Suppressed because this warning is not working */
#endif

e_eFSP_MSGTX_Res eFSP_MSGTX_SendChunk(s_eFSP_MSGTX_Ctx* const p_ctx)
{
	/* Local variable of the operation result */
	e_eFSP_MSGTX_Res result;
	e_eFSP_MSGE_Res resultMsgE;
    bool_t isInit;

    /* Local variable to keep track of the current state machine state */
    e_eFSP_MSGTX_Priv_state stateM;

    /* Local variable to keep track of the start, current etc remaining time */
    uint32_t sRemainTxTime;
    uint32_t cRemainTxTime;
    uint32_t sessionRemaining;
    uint32_t elapsedFromStart;

    /* Local variable usend for the current data calculation */
    const uint8_t *cDToTxP;
    uint32_t cDToTxLen;
    uint32_t cDTxed;

	/* Check pointer validity */
	if( NULL == p_ctx )
	{
		result = MSGTX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == eFSP_MSGTX_isStatusStillCoherent(p_ctx) )
		{
			result = MSGTX_RES_CORRUPTCTX;
		}
		else
		{
            /* Init state machine value */
            sessionRemaining = 0u;
            sRemainTxTime = 0u;
            result = MSGTX_RES_OK;
            stateM = MSGTX_PRV_CHECKINIT;

            /* wait end elaboration or end for timeout */
            while( stateM != MSGTX_PRV_ELABDONE )
            {
                switch( stateM )
                {
                    case MSGTX_PRV_CHECKINIT:
                    {
                        /* Check if lib is initialized */
                        isInit = false;
                        resultMsgE = eFSP_MSGE_IsInit(&p_ctx->msgEncoderCtnx, &isInit);
                        result = eFSP_MSGTX_convertReturnFromMSGE(resultMsgE);

                        /* Check if frame timeout is eplased */
                        if( MSGTX_RES_OK == result )
                        {
                            if( true == isInit )
                            {
                                /* All ok */
                                stateM = MSGTX_PRV_CHECKINITTIMEOUT;
                            }
                            else
                            {
                                /* Need to init the lib before */
                                result = MSGTX_RES_NOINITLIB;
                                stateM = MSGTX_PRV_ELABDONE;
                            }
                        }
                        else
                        {
                            /* Some error */
                            stateM = MSGTX_PRV_ELABDONE;
                        }
                        break;
                    }

                    case MSGTX_PRV_CHECKINITTIMEOUT:
                    {
                        /* Check if frame timeout is eplased */
                        if( true == p_ctx->txTimer.tim_getRemaining(p_ctx->txTimer.timerCtx, &sRemainTxTime) )
                        {
                            if( sRemainTxTime <= 0u )
                            {
                                /* Time elapsed */
                                result = MSGTX_RES_MESSAGETIMEOUT;
                                stateM = MSGTX_PRV_ELABDONE;
                            }
                            else
                            {
                                /* Frame timeout is not elapsed, calculate frame session timeout */
                                if( sRemainTxTime >= p_ctx->timePerSendMs )
                                {
                                    /* Time elapsed */
                                    sessionRemaining = p_ctx->timePerSendMs;
                                }
                                else
                                {
                                    /* Session timeout not elapsed, can send data */
                                    sessionRemaining = sRemainTxTime;
                                }

                                /* check if we have some data to send in TX buffer */
                                stateM = MSGTX_PRV_CHECKIFBUFFERTX;
                            }
                        }
                        else
                        {
                            /* Some error on timer */
                            result = MSGTX_RES_TIMCLBKERROR;
                            stateM = MSGTX_PRV_ELABDONE;
                        }
                        break;
                    }

                    case MSGTX_PRV_CHECKIFBUFFERTX:
                    {
                        /* Is data present in send buffer? */
                        cDToTxLen = p_ctx->sendBuffFill - p_ctx->sendBuffCntr;

                        if( cDToTxLen > 0u )
                        {
                            /* Can send data in msg buffer */
                            stateM = MSGTX_PRV_SENDBUFF;
                        }
                        else
                        {
                            /* No data in msg buffer, retrive some other chunk of data, only if the message send is
                             * not completed of course */
                            p_ctx->sendBuffFill = 0u;
                            p_ctx->sendBuffCntr = 0u;
                            stateM = MSGTX_PRV_RETRIVECHUNK;
                        }
                        break;
                    }

                    case MSGTX_PRV_RETRIVECHUNK:
                    {
                        /* Ok, the send buffer is empty, need to load remainings data */
                        p_ctx->sendBuffCntr = 0u;
                        p_ctx->sendBuffFill = 0u;

                        /* Is data present in message encoder buffer? */
                        resultMsgE = eFSP_MSGE_GetEncChunk(&p_ctx->msgEncoderCtnx, p_ctx->sendBuff, p_ctx->sendBuffSize,
                                                        &p_ctx->sendBuffFill);
                        result = eFSP_MSGTX_convertReturnFromMSGE(resultMsgE);

                        if( MSGTX_RES_OK == result )
                        {
                            /* Retrived some data to send, by design if MSGE_RetriveEChunk return MSGE_RES_OK this
                             * means that the value of loaded data inside send buffer is equals to it's size */

                            /* Can go to send loaded data now */
                            stateM = MSGTX_PRV_SENDBUFF;
                        }
                        else if( MSGTX_RES_MESSAGESENDED == result )
                        {
                            /* Ok we retrived all the possible data */
                            if( 0u == p_ctx->sendBuffFill )
                            {
                                /* No more data to send or retrive */
                                stateM = MSGTX_PRV_ELABDONE;
                            }
                            else
                            {
                                /* Ok retrived all data but need to send the remaining data present in sendBuff */
                                stateM = MSGTX_PRV_SENDBUFF;
                            }
                        }
                        else
                        {
                            /* Some error */
                            stateM = MSGTX_PRV_ELABDONE;
                        }

                        break;
                    }

                    case MSGTX_PRV_SENDBUFF:
                    {
                        /* Get data to send */
                        cDToTxP = &p_ctx->sendBuff[p_ctx->sendBuffCntr];
                        cDToTxLen = p_ctx->sendBuffFill - p_ctx->sendBuffCntr;

                        /* Can send data from send buffer */
                        if( true == (*p_ctx->cbTxP)(p_ctx->cbTxCtx, cDToTxP, cDToTxLen, &cDTxed, sessionRemaining) )
                        {
                            /* Check for some strangeness */
                            if( cDTxed > cDToTxLen )
                            {
                                result = MSGTX_RES_CORRUPTCTX;
                                stateM = MSGTX_PRV_ELABDONE;
                            }
                            else
                            {
                                /* Update sended counter */
                                p_ctx->sendBuffCntr += cDTxed;

                                /* Check if time is elapsed */
                                stateM = MSGTX_PRV_CHECKTIMEOUTAFTERTX;
                            }
                        }
                        else
                        {
                            /* Error sending data */
                            result = MSGTX_RES_TXCLBKERROR;
                            stateM = MSGTX_PRV_ELABDONE;
                        }
                        break;
                    }

                    case MSGTX_PRV_CHECKTIMEOUTAFTERTX:
                    {
                        /* Check if frame timeout is eplased */
                        if( true == p_ctx->txTimer.tim_getRemaining(p_ctx->txTimer.timerCtx, &cRemainTxTime) )
                        {
                            /* Check time validity */
                            if( cRemainTxTime > sRemainTxTime )
                            {
                                /* It's not possible to have more time to send the frame now than during
                                 * the begining */
                                result = MSGTX_RES_CORRUPTCTX;
                                stateM = MSGTX_PRV_ELABDONE;
                            }
                            else
                            {
                                if( cRemainTxTime <= 0u )
                                {
                                    /* Time elapsed */
                                    result = MSGTX_RES_MESSAGETIMEOUT;
                                    stateM = MSGTX_PRV_ELABDONE;
                                }
                                else
                                {
                                    /* During start do we had the whole time slice? */
                                    if( sRemainTxTime >= p_ctx->timePerSendMs )
                                    {
                                        /* Started with a whole time slice */
                                        elapsedFromStart = sRemainTxTime - cRemainTxTime;

                                        /* Frame timeout is not elapsed, check current session if expired */
                                        if( elapsedFromStart >= p_ctx->timePerSendMs )
                                        {
                                            /* Time elapsed */
                                            result = MSGTX_RES_OK;
                                            stateM = MSGTX_PRV_ELABDONE;
                                        }
                                        else
                                        {
                                            /* Session timeout not elapsed, can send data */
                                            sessionRemaining = p_ctx->timePerSendMs - elapsedFromStart;

                                            /* Check if something to send is present */
                                            stateM = MSGTX_PRV_CHECKIFBUFFERTX;
                                        }
                                    }
                                    else
                                    {
                                        /* Started only with remaining time */
                                        sessionRemaining = cRemainTxTime;

                                        /* Check if something to send is present */
                                        stateM = MSGTX_PRV_CHECKIFBUFFERTX;
                                    }
                                }
                            }
                        }
                        else
                        {
                            /* Some error on timer */
                            result = MSGTX_RES_TIMCLBKERROR;
                            stateM = MSGTX_PRV_ELABDONE;
                        }
                        break;
                    }

                    default:
                    {
                        /* Impossible end here */
                        stateM = MSGTX_PRV_ELABDONE;
                        result = MSGTX_RES_CORRUPTCTX;
                        break;
                    }
                }
            }
        }
	}

	return result;
}

#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_restore = "MISRAC2012-Rule-10.3"
#endif



/***********************************************************************************************************************
 *  PRIVATE FUNCTION
 **********************************************************************************************************************/
static bool_t eFSP_MSGTX_isStatusStillCoherent(const s_eFSP_MSGTX_Ctx* p_ctx)
{
    bool_t result;

	/* Check pointer validity */
	if( ( NULL == p_ctx->sendBuff ) || ( NULL == p_ctx->cbTxP ) || ( NULL == p_ctx->cbTxCtx ) ||
        ( NULL == p_ctx->txTimer.timerCtx ) || ( NULL == p_ctx->txTimer.tim_start ) ||
        ( NULL == p_ctx->txTimer.tim_getRemaining ) )
	{
		result = false;
	}
	else
	{
        /* Check send buffer validity */
        if( p_ctx->sendBuffSize < 1u )
        {
            result =  false;
        }
        else
        {
            /* Check send buffer validity */
            if( ( p_ctx->sendBuffFill > p_ctx->sendBuffSize )  || ( p_ctx->sendBuffCntr > p_ctx->sendBuffFill ) )
            {
                result =  false;
            }
            else
            {
                /* Check timings validity */
                if( ( p_ctx->frameTimeoutMs < 1u ) || ( p_ctx->timePerSendMs < 1u ) )
                {
                    result = false;
                }
                else
                {
                    /* Check timings validity */
                    if( p_ctx->timePerSendMs > p_ctx->frameTimeoutMs )
                    {
                        result = false;
                    }
                    else
                    {
                        result = true;
                    }
                }
            }
        }
	}

    return result;
}

static e_eFSP_MSGTX_Res eFSP_MSGTX_convertReturnFromMSGE(e_eFSP_MSGE_Res returnedEvent)
{
	e_eFSP_MSGTX_Res result;

	switch( returnedEvent )
	{
		case MSGE_RES_OK:
		{
			result = MSGTX_RES_OK;
            break;
		}

		case MSGE_RES_BADPARAM:
		{
			result = MSGTX_RES_BADPARAM;
            break;
		}

		case MSGE_RES_BADPOINTER:
		{
			result = MSGTX_RES_BADPOINTER;
            break;
		}

		case MSGE_RES_CORRUPTCTX:
		{
			result = MSGTX_RES_CORRUPTCTX;
            break;
		}

		case MSGE_RES_MESSAGEENDED:
		{
			result = MSGTX_RES_MESSAGESENDED;
            break;
		}

		case MSGE_RES_NOINITLIB:
		{
			result = MSGTX_RES_NOINITLIB;
            break;
		}

		case MSGE_RES_NOINITMESSAGE :
		{
			result = MSGTX_RES_NOINITMESSAGE;
            break;
		}

		case MSGE_RES_CRCCLBKERROR :
		{
			result = MSGTX_RES_CRCCLBKERROR;
            break;
		}

		default:
		{
            /* Impossible end here */
			result = MSGTX_RES_CORRUPTCTX;
            break;
		}
	}

	return result;
}