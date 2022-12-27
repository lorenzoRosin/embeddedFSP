/**
 * @file       eFSP_MSGRX.h
 *
 * @brief      Message receiver utils
 *
 * @author     Lorenzo Rosin
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *      INCLUDES
 **********************************************************************************************************************/
#include "eFSP_MSGRX.h"
#include "eFSP_MSGRXPRV.h"



/***********************************************************************************************************************
 *  PRIVATE STATIC FUNCTION DECLARATION
 **********************************************************************************************************************/
static bool_t eFSP_MSGRX_IsStatusStillCoherent(const t_eFSP_MSGRX_Ctx* p_ptCtx);
static e_eFSP_MSGRX_RES eFSP_MSGRX_ConvertRetFromMSGD(e_eFSP_MSGD_RES p_eRetEvent);



/***********************************************************************************************************************
 *   GLOBAL FUNCTIONS
 **********************************************************************************************************************/
e_eFSP_MSGRX_RES eFSP_MSGRX_InitCtx(t_eFSP_MSGRX_Ctx* const p_ptCtx, const t_eFSP_MSGRX_InitData* p_initData)
{
	/* Local variable */
	e_eFSP_MSGRX_RES l_eRes;
	e_eFSP_MSGD_RES l_eResMsgD;

	/* Check pointer validity */
	if( ( NULL == p_ptCtx ) || ( NULL == p_initData ) )
	{
		l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
	}
	else
	{
        /* Check pointer validity */
        if( ( NULL == p_initData->puIMemArea ) || ( NULL == p_initData->puIRxBuffArea ) ||
            ( NULL == p_initData->fICrc ) || ( NULL == p_initData->ptICbCrcCtx ) || ( NULL == p_initData->fIRx ) ||
            ( NULL == p_initData->ptICbRxCtx ) || ( NULL == p_initData->tIRxTim.ptTimCtx ) ||
            ( NULL == p_initData->tIRxTim.fTimStart ) || ( NULL == p_initData->tIRxTim.fTimGetRemain ) )
        {
            l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
        }
        else
        {
            /* Check data validity of data area */
            if( p_initData->uIRxBuffAreaL < 1u )
            {
                l_eRes = e_eFSP_MSGRX_RES_BADPARAM;
            }
            else
            {
                /* Check data validity of time */
                if( ( p_initData->uITimeoutMs < 1u ) || ( p_initData->uITimePerRecMs < 1u ) ||
                    ( p_initData->uITimePerRecMs > p_initData->uITimeoutMs ) )
                {
                    l_eRes = e_eFSP_MSGRX_RES_BADPARAM;
                }
                else
                {
                    /* Initialize internal status variable */
                    p_ptCtx->p_rxBuff = p_initData->puIRxBuffArea;
                    p_ptCtx->rxBuffSize = p_initData->uIRxBuffAreaL;
                    p_ptCtx->rxBuffCntr = 0u;
                    p_ptCtx->rxBuffFill = 0u;
                    p_ptCtx->f_Rx = p_initData->fIRx;
                    p_ptCtx->p_RxCtx = p_initData->ptICbRxCtx;
                    p_ptCtx->rxTim = p_initData->tIRxTim;
                    p_ptCtx->timeoutMs = p_initData->uITimeoutMs;
                    p_ptCtx->timePerRecMs = p_initData->uITimePerRecMs;
                    p_ptCtx->needWaitFrameStart = p_initData->bINeedWaitFrameStart;

                    /* initialize internal bytestuffer */
                    l_eResMsgD =  eFSP_MSGD_InitCtx(&p_ptCtx->msgd_Ctx, p_initData->puIMemArea, p_initData->uIMemAreaL,
                                               p_initData->fICrc, p_initData->ptICbCrcCtx);
                    l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);
                }
            }
        }
	}

	return l_eRes;
}

e_eFSP_MSGRX_RES eFSP_MSGRX_IsInit(t_eFSP_MSGRX_Ctx* const p_ptCtx, bool_t* p_pbIsInit)
{
	/* Local variable */
	e_eFSP_MSGRX_RES l_eRes;
	e_eFSP_MSGD_RES l_eResMsgD;

	/* Check pointer validity */
	if( ( NULL == p_ptCtx ) || ( NULL == p_pbIsInit ) )
	{
		l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
	}
	else
	{
        l_eResMsgD = eFSP_MSGD_IsInit(&p_ptCtx->msgd_Ctx, p_pbIsInit);
        l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);
	}

	return l_eRes;
}

e_eFSP_MSGRX_RES eFSP_MSGRX_NewMsg(t_eFSP_MSGRX_Ctx* const p_ptCtx)
{
	/* Local variable */
	e_eFSP_MSGRX_RES l_eRes;
	e_eFSP_MSGD_RES l_eResMsgD;

	/* Check pointer validity */
	if( NULL == p_ptCtx )
	{
		l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
	}
	else
	{
        /* Check internal status validity */
        if( false == eFSP_MSGRX_IsStatusStillCoherent(p_ptCtx) )
        {
            l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
        }
        else
        {
            /* Init message encoder */
            l_eResMsgD = eFSP_MSGD_NewMsg(&p_ptCtx->msgd_Ctx);
            l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);

            if( e_eFSP_MSGRX_RES_OK == l_eRes )
            {
                /* Start timer even if we need to wait SOF, this case is handled in the MSGRX_GetDecodedData */
                if( true != p_ptCtx->rxTim.fTimStart( p_ptCtx->rxTim.ptTimCtx, p_ptCtx->timeoutMs ) )
                {
                    l_eRes = e_eFSP_MSGRX_RES_TIMCLBKERROR;
                }
            }
        }
	}

	return l_eRes;
}

e_eFSP_MSGRX_RES eFSP_MSGRX_NewMsgNClean(t_eFSP_MSGRX_Ctx* const p_ptCtx)
{
	/* Local variable for l_eRes */
	e_eFSP_MSGRX_RES l_eRes;
	e_eFSP_MSGD_RES l_eResMsgD;

	/* Check pointer validity */
	if( NULL == p_ptCtx )
	{
		l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
	}
	else
	{
        /* Check internal status validity */
        if( false == eFSP_MSGRX_IsStatusStillCoherent(p_ptCtx) )
        {
            l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
        }
        else
        {
            /* Reset internal variable */
            p_ptCtx->rxBuffCntr = 0u;
            p_ptCtx->rxBuffFill = 0u;

            /* Init message encoder */
            l_eResMsgD = eFSP_MSGD_NewMsg(&p_ptCtx->msgd_Ctx);
            l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);

            if( e_eFSP_MSGRX_RES_OK == l_eRes )
            {
                /* Start timer even if we need to wait SOF, this case is handled in the MSGRX_GetDecodedData */
                if( true != p_ptCtx->rxTim.fTimStart( p_ptCtx->rxTim.ptTimCtx, p_ptCtx->timeoutMs ) )
                {
                    l_eRes = e_eFSP_MSGRX_RES_TIMCLBKERROR;
                }
            }
        }
	}

	return l_eRes;
}

e_eFSP_MSGRX_RES eFSP_MSGRX_GetDecodedData(t_eFSP_MSGRX_Ctx* const p_ptCtx, uint8_t** pp_data, uint32_t* const p_GetLen)
{
	/* Local variable */
	e_eFSP_MSGRX_RES l_eRes;
	e_eFSP_MSGD_RES l_eResMsgD;

	/* Check pointer validity */
	if( ( NULL == p_ptCtx ) || ( NULL == pp_data ) || ( NULL == p_GetLen ) )
	{
		l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == eFSP_MSGRX_IsStatusStillCoherent(p_ptCtx) )
		{
			l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
		}
		else
		{
			/* Get memory reference of CRC+LEN+DATA, so we can calculate reference of only data payload */
			l_eResMsgD = eFSP_MSGD_GetDecodedData(&p_ptCtx->msgd_Ctx, pp_data, p_GetLen);
			l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);
		}
	}

	return l_eRes;
}

e_eFSP_MSGRX_RES eFSP_MSGRX_ReceiveChunk(t_eFSP_MSGRX_Ctx* const p_ptCtx)
{
	/* Local variable of the operation l_eRes */
	e_eFSP_MSGRX_RES l_eRes;
	e_eFSP_MSGD_RES l_eResMsgD;

    /* Local variable to keep track of the current state machine state */
    e_eFSP_MSGRXPRV_SM l_eSM;

    /* Local variable used for the current time calculation */
    uint32_t l_uSRemRxT;
    uint32_t l_uCRemRxT;
    uint32_t l_uElapsFromStart;
    uint32_t l_uSSessionRemanT;
    uint32_t l_uCSessionRemanT;

    bool_t l_bIsMsgDec;
    bool_t l_bIsInit;

    /* Local variable usend for the current data calculation */
    uint8_t *l_puCDToRxP;
    uint32_t l_uCDToRxL;
    uint32_t l_uCDRxed;

    /* Other local variable */
    uint32_t l_uRxMostEff;
    bool_t l_bIsWaitingSof;

	/* Check pointer validity */
	if( NULL == p_ptCtx )
	{
		l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
	}
	else
	{
		/* Check internal status validity */
		if( false == eFSP_MSGRX_IsStatusStillCoherent(p_ptCtx) )
		{
			l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
		}
		else
		{
            /* Init time frame counter */
            l_eRes = e_eFSP_MSGRX_RES_OK;
            l_eSM = e_eFSP_MSGRXPRV_SM_CHECKINIT;

            /* Init data */
            l_uSRemRxT = 0u;
            l_uCSessionRemanT = 0u;
            l_uSSessionRemanT = 0u;
            l_uRxMostEff = 0u;
            l_uCDRxed = 0u;
            l_bIsMsgDec = false;
            l_bIsWaitingSof = false;

            /* wait end elaboration or end for timeout */
            while( l_eSM != e_eFSP_MSGRXPRV_SM_ELABDONE )
            {
                switch( l_eSM )
                {
                    case e_eFSP_MSGRXPRV_SM_CHECKINIT:
                    {
                        /* Check if lib is initialized */
                        l_bIsInit = false;
                        l_eResMsgD = eFSP_MSGD_IsInit(&p_ptCtx->msgd_Ctx, &l_bIsInit);
                        l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);

                        /* Check if any error is returned */
                        if( e_eFSP_MSGRX_RES_OK == l_eRes )
                        {
                            if( true == l_bIsInit )
                            {
                                /* All ok */
                                l_eSM = e_eFSP_MSGRXPRV_SM_CHECKINITTIMEOUT;
                            }
                            else
                            {
                                /* Need to init the lib before */
                                l_eRes = e_eFSP_MSGRX_RES_NOINITLIB;
                                l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                            }
                        }
                        else
                        {
                            /* Some error */
                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                        }
                        break;
                    }

                    case e_eFSP_MSGRXPRV_SM_CHECKINITTIMEOUT:
                    {
                        /* Check if frame timeout is eplased */
                        if( true == p_ptCtx->rxTim.fTimGetRemain(p_ptCtx->rxTim.ptTimCtx, &l_uSRemRxT) )
                        {
                            /* Check also if we are still waiting start of frame to be received */
                            l_eResMsgD =  eFSP_MSGD_IsWaitingSof(&p_ptCtx->msgd_Ctx, &l_bIsWaitingSof);
                            l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);

                            if( e_eFSP_MSGRX_RES_OK == l_eRes )
                            {
                                if( true == p_ptCtx->needWaitFrameStart )
                                {
                                    /* Need to wait SOF */
                                    if( true == l_bIsWaitingSof )
                                    {
                                        /* We are waiting start of frame, reset timer */
                                        if( true == p_ptCtx->rxTim.fTimStart( p_ptCtx->rxTim.ptTimCtx, p_ptCtx->timeoutMs ) )
                                        {
                                            /* Ok restarted the timer */
                                            l_uSRemRxT = p_ptCtx->timeoutMs;
                                            l_uCSessionRemanT = p_ptCtx->timePerRecMs;
                                            l_uSSessionRemanT = p_ptCtx->timePerRecMs;

                                            /* check if we have some data to receive in RX buffer */
                                            l_eSM = e_eFSP_MSGRXPRV_SM_CHECKHOWMANYDATA;
                                        }
                                        else
                                        {
                                            /* Some error */
                                            l_eRes = e_eFSP_MSGRX_RES_TIMCLBKERROR;
                                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                        }
                                    }
                                    else
                                    {
                                        /* Frame already started */
                                        if( l_uSRemRxT <= 0u )
                                        {
                                            /* Time elapsed */
                                            l_eRes = e_eFSP_MSGRX_RES_MESSAGETIMEOUT;
                                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                        }
                                        else
                                        {
                                            /* Frame timeout is not elapsed, calculate frame session timeout */
                                            if( l_uSRemRxT >= p_ptCtx->timePerRecMs )
                                            {
                                                /* Time elapsed */
                                                l_uCSessionRemanT = p_ptCtx->timePerRecMs;
                                                l_uSSessionRemanT = p_ptCtx->timePerRecMs;
                                            }
                                            else
                                            {
                                                /* Session timeout not elapsed, can receive data */
                                                l_uCSessionRemanT = l_uSRemRxT;
                                                l_uSSessionRemanT = l_uSRemRxT;
                                            }

                                            /* check if we have some data to receive in RX buffer */
                                            l_eSM = e_eFSP_MSGRXPRV_SM_CHECKHOWMANYDATA;
                                        }
                                    }
                                }
                                else
                                {
                                    /* No need to wait SOF, timer start con receiveStart function call */
                                    if( l_uSRemRxT <= 0u )
                                    {
                                        /* Time elapsed */
                                        l_eRes = e_eFSP_MSGRX_RES_MESSAGETIMEOUT;
                                        l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                    }
                                    else
                                    {
                                        /* Frame timeout is not elapsed, calculate frame session timeout */
                                        if( l_uSRemRxT >= p_ptCtx->timePerRecMs )
                                        {
                                            /* Time elapsed */
                                            l_uCSessionRemanT = p_ptCtx->timePerRecMs;
                                            l_uSSessionRemanT = p_ptCtx->timePerRecMs;
                                        }
                                        else
                                        {
                                            /* Session timeout not elapsed, can receive data */
                                            l_uCSessionRemanT = l_uSRemRxT;
                                            l_uSSessionRemanT = l_uSRemRxT;
                                        }

                                        /* check if we have some data to receive in RX buffer */
                                        l_eSM = e_eFSP_MSGRXPRV_SM_CHECKHOWMANYDATA;
                                    }
                                }
                            }
                            else
                            {
                                /* Some error */
                                l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                            }
                        }
                        else
                        {
                            /* Some error on timer */
                            l_eRes = e_eFSP_MSGRX_RES_TIMCLBKERROR;
                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                        }
                        break;
                    }

                    case e_eFSP_MSGRXPRV_SM_CHECKHOWMANYDATA:
                    {
                        /* How many byte do we need to receive? */
                        l_eResMsgD = eFSP_MSGD_GetMostEffDatLen(&p_ptCtx->msgd_Ctx, &l_uRxMostEff);
                        l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);
                        if( e_eFSP_MSGRX_RES_OK == l_eRes )
                        {
                            if( l_uRxMostEff > 0u )
                            {
                                /* Ok we need some data to be retrived */
                                l_eSM = e_eFSP_MSGRXPRV_SM_CHECKIFBUFFERRX;

                                /* Check compatibility with rx buffer dimension */
                                if( l_uRxMostEff > p_ptCtx->rxBuffSize )
                                {
                                    /* In this way we dont' have any overflow */
                                    l_uRxMostEff = p_ptCtx->rxBuffSize;
                                }
                            }
                            else
                            {
                                /* Could be because the message is received or an error occoured */
                                l_eResMsgD = eFSP_MSGD_IsAFullMsgDecoded(&p_ptCtx->msgd_Ctx, &l_bIsMsgDec);
                                l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);

                                if( e_eFSP_MSGRX_RES_OK == l_eRes )
                                {
                                    if( true == l_bIsMsgDec )
                                    {
                                        /* Ok the message is correct */
                                        l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                        l_eRes = e_eFSP_MSGRX_RES_MESSAGERECEIVED;
                                    }
                                    else
                                    {
                                        /* Umm the message is not correct */
                                        l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                        l_eRes = e_eFSP_MSGRX_RES_BADFRAME;
                                    }
                                }
                                else
                                {
                                    /* Some error */
                                    l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                }
                            }
                        }
                        else
                        {
                            /* Some error */
                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                        }
                        break;
                    }

                    case e_eFSP_MSGRXPRV_SM_CHECKIFBUFFERRX:
                    {
                        /* Is needed data already present in the receive buffer? */
                        l_uCDToRxL = p_ptCtx->rxBuffFill - p_ptCtx->rxBuffCntr;

                        if( l_uCDToRxL > 0u )
                        {
                            /* Can parse already received data in msg buffer */
                            l_eSM = e_eFSP_MSGRXPRV_SM_INSERTCHUNK;
                        }
                        else
                        {
                            /* No data in msg buffer, retrive some other chunk of data */
                            p_ptCtx->rxBuffCntr = 0u;
                            p_ptCtx->rxBuffFill = 0u;
                            l_eSM = e_eFSP_MSGRXPRV_SM_RECEIVEBUFF;
                        }
                        break;
                    }

                    case e_eFSP_MSGRXPRV_SM_RECEIVEBUFF:
                    {
                        /* Ok, rx buffer is empty and need to be filled with data that we can parse */
                        if( true == (*p_ptCtx->f_Rx)(p_ptCtx->p_RxCtx, p_ptCtx->p_rxBuff, &p_ptCtx->rxBuffFill, l_uRxMostEff,
                                                  l_uCSessionRemanT) )
                        {
                            /* Check for some strangeness */
                            if( p_ptCtx->rxBuffFill > l_uRxMostEff )
                            {
                                l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
                                l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                            }
                            else
                            {
                                /* Update received counter */
                                p_ptCtx->rxBuffCntr = 0u;

                                /* Go next state */
                                l_eSM = e_eFSP_MSGRXPRV_SM_INSERTCHUNK;
                            }
                        }
                        else
                        {
                            /* Error sending data */
                            l_eRes = e_eFSP_MSGRX_RES_RXCLBKERROR;
                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                        }

                        break;
                    }

                    case e_eFSP_MSGRXPRV_SM_INSERTCHUNK:
                    {
                        /* Ok, we have some data in RX buffer */
                        l_puCDToRxP = &p_ptCtx->p_rxBuff[p_ptCtx->rxBuffCntr];
                        l_uCDToRxL = p_ptCtx->rxBuffFill - p_ptCtx->rxBuffCntr;

                        if( l_uCDToRxL > 0u )
                        {
                            /* We can try to decode data event if we already finished cuz the function
                            * MSGD_InsEncChunk is well maden */
                            l_eResMsgD = eFSP_MSGD_InsEncChunk(&p_ptCtx->msgd_Ctx, l_puCDToRxP, l_uCDToRxL, &l_uCDRxed );
                            l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);

                            if( e_eFSP_MSGRX_RES_OK == l_eRes )
                            {
                                /* Retrived some data, by design if MSGD_InsEncChunk return e_eFSP_MSGE_RES_OK this
                                 * means that the value of loaded data inside send buffer is equals to it's size */
                                p_ptCtx->rxBuffCntr = 0u;
                                p_ptCtx->rxBuffFill = 0u;

                                /* Check for timeout */
                                l_eSM = e_eFSP_MSGRXPRV_SM_CHECKTIMEOUTAFTERRX;
                            }
                            else if( e_eFSP_MSGRX_RES_MESSAGERECEIVED == l_eRes )
                            {
                                /* Ok we retrived all the possible data */
                                /* Update RX buffer */
                                p_ptCtx->rxBuffCntr += l_uCDRxed;

                                /* Check for timeout */
                                l_eSM = e_eFSP_MSGRXPRV_SM_CHECKTIMEOUTAFTERRX;
                            }
                            else if( e_eFSP_MSGRX_RES_BADFRAME == l_eRes )
                            {
                                /* Update RX buffer */
                                p_ptCtx->rxBuffCntr += l_uCDRxed;

                                /* Bad frame, but check timeout also */
                                l_eSM = e_eFSP_MSGRXPRV_SM_CHECKTIMEOUTAFTERRX;
                            }
                            else if( e_eFSP_MSGRX_RES_FRAMERESTART == l_eRes )
                            {
                                /* Update RX buffer */
                                p_ptCtx->rxBuffCntr += l_uCDRxed;

                                 /* ok frame restarted, but check timeout also */
                                l_eSM = e_eFSP_MSGRXPRV_SM_CHECKTIMEOUTAFTERRX;
                            }
                            else
                            {
                                /* Update RX buffer */
                                p_ptCtx->rxBuffCntr += l_uCDRxed;

                                /* Some error, can return */
                                l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                            }
                        }
                        else
                        {
                            /* Didn't receive data in the previous e_eFSP_MSGRXPRV_SM_RECEIVEBUFF, check timeout */
                            l_eRes = e_eFSP_MSGRX_RES_OK;
                            l_eSM = e_eFSP_MSGRXPRV_SM_CHECKTIMEOUTAFTERRX;
                        }

                        break;
                    }

                    case e_eFSP_MSGRXPRV_SM_CHECKTIMEOUTAFTERRX:
                    {
                        /* Check if frame timeout is eplased */
                        if( true == p_ptCtx->rxTim.fTimGetRemain(p_ptCtx->rxTim.ptTimCtx, &l_uCRemRxT) )
                        {
                            /* Check time validity */
                            if( l_uCRemRxT > l_uSRemRxT )
                            {
                                /* It's not possible to have more time to send the frame now than during
                                * the begining */
                                l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
                                l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                            }
                            else
                            {
                                /* Calc elapsed time */
                                l_uElapsFromStart = l_uSRemRxT - l_uCRemRxT;

                                /* Are we waiting SOF? */
                                l_eResMsgD =  eFSP_MSGD_IsWaitingSof(&p_ptCtx->msgd_Ctx, &l_bIsWaitingSof);
                                if( e_eFSP_MSGD_RES_OK == l_eResMsgD )
                                {
                                    /* Do we need to wait start of frame? */
                                    if( ( true == p_ptCtx->needWaitFrameStart ) && ( e_eFSP_MSGRX_RES_FRAMERESTART == l_eRes ) )
                                    {
                                        /* Timeout dosent occour in this situation */
                                        l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;

                                        /* Frame restarted, restart the timer */
                                        if( true == p_ptCtx->rxTim.fTimStart( p_ptCtx->rxTim.ptTimCtx, p_ptCtx->timeoutMs ) )
                                        {
                                            /* Ok restarted the timer */
                                            l_uSRemRxT = p_ptCtx->timeoutMs;

                                            /* Dosent care about session imeout because we are returning in any case */
                                        }
                                        else
                                        {
                                            /* Some error */
                                            l_eRes = e_eFSP_MSGRX_RES_TIMCLBKERROR;
                                        }
                                    }
                                    else if( ( true == p_ptCtx->needWaitFrameStart ) && ( true == l_bIsWaitingSof ) &&
                                             ( e_eFSP_MSGRX_RES_OK == l_eRes ) )
                                    {
                                        /* In this case total time dosen't need to be decreased, only the session */
                                        /* Frame restarted, restart the timer */
                                        if( true == p_ptCtx->rxTim.fTimStart( p_ptCtx->rxTim.ptTimCtx, p_ptCtx->timeoutMs ) )
                                        {
                                            /* Ok restarted the timer */
                                            l_uSRemRxT = p_ptCtx->timeoutMs;
                                            l_uCRemRxT = p_ptCtx->timeoutMs;

                                            /* Update session timeout */
                                            if( l_uElapsFromStart >= l_uSSessionRemanT )
                                            {
                                                /* No more time */
                                                l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                            }
                                            else
                                            {
                                                /* Update remaining session time */
                                                l_uSSessionRemanT = l_uSSessionRemanT - l_uElapsFromStart;
                                                l_uCSessionRemanT = l_uSSessionRemanT;

                                                /* Can retrive more data */
                                                l_eSM = e_eFSP_MSGRXPRV_SM_CHECKHOWMANYDATA;
                                            }
                                        }
                                        else
                                        {
                                            /* Some error */
                                            l_eRes = e_eFSP_MSGRX_RES_TIMCLBKERROR;
											l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                        }
                                    }
                                    else
                                    {
                                        /* Check for timeout */
                                        if( l_uCRemRxT <= 0u )
                                        {
                                            /* Time elapsed */
                                            l_eRes = e_eFSP_MSGRX_RES_MESSAGETIMEOUT;
                                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                        }
                                        else
                                        {
                                            /* Frame timeout is not elapsed, check current session if expired */
                                            if( l_uElapsFromStart >= l_uSSessionRemanT )
                                            {
                                                /* Session time is elapsed, can return the previous l_eRes */
                                                l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                            }
                                            else
                                            {
                                                /* Session timeout not elapsed, if needed can do more elaboration */
                                                l_uCSessionRemanT = l_uSSessionRemanT - l_uElapsFromStart;

                                                /* Check what to do looking at the previous state l_eRes */
                                                if( e_eFSP_MSGRX_RES_OK == l_eRes )
                                                {
                                                    /* Can retrive more data */
                                                    l_eSM = e_eFSP_MSGRXPRV_SM_CHECKHOWMANYDATA;
                                                }
                                                else if( e_eFSP_MSGRX_RES_MESSAGERECEIVED == l_eRes )
                                                {
                                                    /* Ok we retrived all the possible data */
                                                    l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                                }
                                                else if( e_eFSP_MSGRX_RES_BADFRAME == l_eRes )
                                                {
                                                    /* Bad frame */
                                                    l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                                }
                                                else if( e_eFSP_MSGRX_RES_FRAMERESTART == l_eRes )
                                                {
                                                    /* ok frame restarted */
                                                    l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                                }
                                                else
                                                {
                                                    /* Some error, But was already handled by previous state */
                                                    l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
                                                    l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    /* Some error */
                                    l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                                    l_eRes = eFSP_MSGRX_ConvertRetFromMSGD(l_eResMsgD);
                                }
                            }
                        }
                        else
                        {
                            /* Some error on timer */
                            l_eRes = e_eFSP_MSGRX_RES_TIMCLBKERROR;
                            l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                        }
                        break;
                    }

                    default:
                    {
                        /* Impossible end here */
                        l_eSM = e_eFSP_MSGRXPRV_SM_ELABDONE;
                        l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
                        break;
                    }
                }
            }
		}
	}

	return l_eRes;
}



/***********************************************************************************************************************
 *  PRIVATE FUNCTION
 **********************************************************************************************************************/
static bool_t eFSP_MSGRX_IsStatusStillCoherent(const t_eFSP_MSGRX_Ctx* p_ptCtx)
{
    bool_t l_bRes;

	/* Check pointer validity */
	if( ( NULL == p_ptCtx->p_rxBuff ) || ( NULL == p_ptCtx->f_Rx ) || ( NULL == p_ptCtx->p_RxCtx ) ||
        ( NULL == p_ptCtx->rxTim.ptTimCtx ) || ( NULL == p_ptCtx->rxTim.fTimStart ) ||
        ( NULL == p_ptCtx->rxTim.fTimGetRemain ) )
	{
		l_bRes = false;
	}
	else
	{
        /* Check send buffer validity */
        if( ( p_ptCtx->rxBuffSize < 1u ) || ( p_ptCtx->rxBuffFill > p_ptCtx->rxBuffSize )  ||
            ( p_ptCtx->rxBuffCntr > p_ptCtx->rxBuffFill ) )
        {
            l_bRes =  false;
        }
        else
        {
            /* Check timings validity */
            if( ( p_ptCtx->timeoutMs < 1u ) || ( p_ptCtx->timePerRecMs < 1u ) ||
                ( p_ptCtx->timePerRecMs > p_ptCtx->timeoutMs ) )
            {
                l_bRes = false;
            }
            else
            {
                l_bRes = true;
            }
        }
	}

    return l_bRes;
}

static e_eFSP_MSGRX_RES eFSP_MSGRX_ConvertRetFromMSGD(e_eFSP_MSGD_RES p_eRetEvent)
{
	e_eFSP_MSGRX_RES l_eRes;

	switch( p_eRetEvent )
	{
		case e_eFSP_MSGD_RES_OK:
		{
			l_eRes = e_eFSP_MSGRX_RES_OK;
            break;
		}

		case e_eFSP_MSGD_RES_BADPARAM:
		{
			l_eRes = e_eFSP_MSGRX_RES_BADPARAM;
            break;
		}

		case e_eFSP_MSGD_RES_BADPOINTER:
		{
			l_eRes = e_eFSP_MSGRX_RES_BADPOINTER;
            break;
		}

		case e_eFSP_MSGD_RES_CORRUPTCTX:
		{
			l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
            break;
		}

        case e_eFSP_MSGD_RES_OUTOFMEM:
        {
			l_eRes = e_eFSP_MSGRX_RES_OUTOFMEM;
            break;
        }

        case e_eFSP_MSGD_RES_BADFRAME:
        {
			l_eRes = e_eFSP_MSGRX_RES_BADFRAME;
            break;
        }

		case e_eFSP_MSGD_RES_MESSAGEENDED:
		{
			l_eRes = e_eFSP_MSGRX_RES_MESSAGERECEIVED;
            break;
		}

		case e_eFSP_MSGD_RES_FRAMERESTART:
		{
			l_eRes = e_eFSP_MSGRX_RES_FRAMERESTART;
            break;
		}

		case e_eFSP_MSGD_RES_NOINITLIB:
		{
			l_eRes = e_eFSP_MSGRX_RES_NOINITLIB;
            break;
		}

		case e_eFSP_MSGD_RES_CRCCLBKERROR :
		{
			l_eRes = e_eFSP_MSGRX_RES_CRCCLBKERROR;
            break;
		}

		default:
		{
            /* Impossible end here */
			l_eRes = e_eFSP_MSGRX_RES_CORRUPTCTX;
            break;
		}
	}

	return l_eRes;
}