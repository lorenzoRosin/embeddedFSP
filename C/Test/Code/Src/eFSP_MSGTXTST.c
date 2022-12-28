/**
 * @file       eFSP_MSGTXTST.c
 *
 * @brief      Message trasmitter test
 *
 * @author     Lorenzo Rosin
 *
 **********************************************************************************************************************/



/***********************************************************************************************************************
 *      INCLUDES
 **********************************************************************************************************************/
#include "eFSP_MSGTXTST.h"
#include "eFSP_MSGTX.h"
#include "eCU_CRC.h"

#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_disable = "MISRAC2004-20.9", "MISRAC2012-Rule-21.6"
    /* Suppressed for code clarity in test execution*/
#endif

#include <stdio.h>

#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_restore = "MISRAC2004-20.9", "MISRAC2012-Rule-21.6"
#endif

#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_disable = "MISRAC2012-Rule-10.3", "CERT-STR32-C", "MISRAC2012-Rule-11.5", "CERT-EXP36-C_b", \
                            "MISRAC2012-Rule-8.9_a", "MISRAC2012-Rule-8.13", "MISRAC2012-Rule-2.2_b", "MISRAC2004-16.7"
    /* Suppressed for code clarity in test execution*/
#endif



/***********************************************************************************************************************
 *   PRIVATE TEST FUNCTION DECLARATION
 **********************************************************************************************************************/
struct t_eFSP_MSGE_CrcCtxUser
{
    e_eCU_CRC_RES eLastEr;
};

struct t_eFSP_MSGTX_TxCtxUser
{
    bool bTxIsError;
};

struct t_eFSP_MSGTX_TimCtxUser
{
    bool bTxIsError;
};

static bool_t eFSP_MSGTXTST_c32SAdapt(t_eFSP_MSGE_CrcCtx* p_ptCtx, const uint32_t p_uS, const uint8_t* p_puD, const uint32_t p_uDLen, uint32_t* const p_puC32Val);
static bool_t eFSP_MSGTXTST_c32SAdaptEr(t_eFSP_MSGE_CrcCtx* p_ptCtx, const uint32_t p_uS, const uint8_t* p_puD, const uint32_t p_uDLen, uint32_t* const p_puC32Val);
static bool_t eFSP_MSGTXTST_sendMsg( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                       const uint32_t p_uTimeToTx );
static bool_t eFSP_MSGTXTST_sendMsgCorr( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                       const uint32_t p_uTimeToTx );
static bool_t eFSP_MSGTXTST_sendMsgOnce( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                       const uint32_t p_uTimeToTx );
static bool_t eFSP_MSGTXTST_sendMsgErr( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                          const uint32_t p_uTimeToTx );
static bool_t eFSP_MSGTXTST_timStart ( t_eFSP_MSGTX_TimCtx* p_ptCtx, const uint32_t p_uTimeoutVal );
static bool_t eFSP_MSGTXTST_timGetRemaining ( t_eFSP_MSGTX_TimCtx* p_ptCtx, uint32_t* const p_puRemainings );
static bool_t eFSP_MSGTXTST_timStartErr ( t_eFSP_MSGTX_TimCtx* p_ptCtx, const uint32_t p_uTimeoutVal );
static bool_t eFSP_MSGTXTST_timGetRemainingErr ( t_eFSP_MSGTX_TimCtx* p_ptCtx, uint32_t* const p_puRemainings );
static bool_t eFSP_MSGTXTST_timGetRemainingCorr ( t_eFSP_MSGTX_TimCtx* p_ptCtx, uint32_t* const p_puRemainings );



/***********************************************************************************************************************
 *   PRIVATE FUNCTION DECLARATION
 **********************************************************************************************************************/
static void eFSP_MSGTXTST_BadPointer(void);
static void eFSP_MSGTXTST_BadInit(void);
static void eFSP_MSGTXTST_BadIniMsg(void);
static void eFSP_MSGTXTST_BadParamEntr(void);
static void eFSP_MSGTXTST_CorruptContext(void);
static void eFSP_MSGTXTST_BadClBckCrc(void);
static void eFSP_MSGTXTST_BadClBckSend(void);
static void eFSP_MSGTXTST_BadClBckTim(void);
static void eFSP_MSGTXTST_CornerCase(void);
static void eFSP_MSGTXTST_CornerCase2(void);
static void eFSP_MSGTXTST_CornerCase3(void);



/***********************************************************************************************************************
 *   GLOBAL FUNCTIONS
 **********************************************************************************************************************/
void eFSP_MSGTXTST_ExeTest(void)
{
	(void)printf("\n\nMESSAGE TRANSMITTER TEST START \n\n");

    eFSP_MSGTXTST_BadPointer();
    eFSP_MSGTXTST_BadInit();
    eFSP_MSGTXTST_BadIniMsg();
    eFSP_MSGTXTST_BadParamEntr();
    eFSP_MSGTXTST_CorruptContext();
    eFSP_MSGTXTST_BadClBckCrc();
    eFSP_MSGTXTST_BadClBckSend();
    eFSP_MSGTXTST_BadClBckTim();
    eFSP_MSGTXTST_CornerCase();
    eFSP_MSGTXTST_CornerCase2();
    eFSP_MSGTXTST_CornerCase3();

    (void)printf("\n\nMESSAGE TRANSMITTER TEST END \n\n");
}



/***********************************************************************************************************************
 *   PRIVATE TEST FUNCTION DECLARATION
 **********************************************************************************************************************/
bool_t eFSP_MSGTXTST_c32SAdapt(t_eFSP_MSGE_CrcCtx* p_ptCtx, const uint32_t p_uS, const uint8_t* p_puD, const uint32_t p_uDLen, uint32_t* const p_puC32Val)
{
    bool_t result;
    t_eFSP_MSGE_CrcCtx* ctxCur;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puC32Val ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGE_CrcCtx*)p_ptCtx;

        ctxCur->eLastEr = eCU_CRC_32Seed(p_uS, (const uint8_t*)p_puD, p_uDLen, p_puC32Val);
        if( e_eCU_CRC_RES_OK == ctxCur->eLastEr )
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }

    return result;
}

bool_t eFSP_MSGTXTST_c32SAdaptEr(t_eFSP_MSGE_CrcCtx* p_ptCtx, const uint32_t p_uS, const uint8_t* p_puD, const uint32_t p_uDLen, uint32_t* const p_puC32Val)
{
    bool_t result;
    t_eFSP_MSGE_CrcCtx* ctxCur;

    (void)p_uS;
    (void)p_puD;
    (void)p_uDLen;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puC32Val ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGE_CrcCtx*)p_ptCtx;

        ctxCur->eLastEr = e_eCU_CRC_RES_BADPOINTER;
        result = false;
        *p_puC32Val = 0u;
    }

    return result;
}


static uint8_t m_auTxBuff[100u];
static uint32_t m_uTxBuffCounter;
static uint32_t m_uSendWhen;

bool_t eFSP_MSGTXTST_sendMsg( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                const uint32_t p_uTimeToTx )
{
    bool_t result;
    t_eFSP_MSGTX_TxCtx* ctxCur;

    (void)p_uTimeToTx;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puDataToTx ) || ( NULL == p_uDataTxed ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TxCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;


        if( ( m_uTxBuffCounter + p_uDataToTxL ) < sizeof(m_auTxBuff) )
        {
            (void)memcpy(&m_auTxBuff[m_uTxBuffCounter], p_puDataToTx, p_uDataToTxL);
            m_uTxBuffCounter += p_uDataToTxL;
            *p_uDataTxed = p_uDataToTxL;

            result = true;
        }
        else
        {
            result = false;
        }
    }

    return result;
}


bool_t eFSP_MSGTXTST_sendMsgCorr( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                const uint32_t p_uTimeToTx )
{
    bool_t result;
    t_eFSP_MSGTX_TxCtx* ctxCur;

    (void)p_uTimeToTx;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puDataToTx ) || ( NULL == p_uDataTxed ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TxCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;


        if( ( m_uTxBuffCounter + p_uDataToTxL ) < sizeof(m_auTxBuff) )
        {
            (void)memcpy(&m_auTxBuff[m_uTxBuffCounter], p_puDataToTx, p_uDataToTxL);
            m_uTxBuffCounter += p_uDataToTxL;
            *p_uDataTxed = p_uDataToTxL + 1u;

            result = true;
        }
        else
        {
            result = false;
        }
    }

    return result;
}

bool_t eFSP_MSGTXTST_sendMsgOnce( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                const uint32_t p_uTimeToTx )
{
    bool_t result;
    t_eFSP_MSGTX_TxCtx* ctxCur;

    (void)p_uTimeToTx;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puDataToTx ) || ( NULL == p_uDataTxed ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TxCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;


        if( ( m_uTxBuffCounter + p_uDataToTxL ) < sizeof(m_auTxBuff) )
        {
            if( 0u == m_uSendWhen )
            {
                (void)memcpy(&m_auTxBuff[m_uTxBuffCounter], p_puDataToTx, p_uDataToTxL);
                m_uTxBuffCounter += p_uDataToTxL;
                *p_uDataTxed = p_uDataToTxL;
                m_uSendWhen = 1u;
            }
            else
            {
                *p_uDataTxed = 0u;
                m_uSendWhen = 0u;
            }

            result = true;
        }
        else
        {
            result = false;
        }
    }

    return result;
}

bool_t eFSP_MSGTXTST_sendMsgErr( t_eFSP_MSGTX_TxCtx* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL, uint32_t* const p_uDataTxed,
                const uint32_t p_uTimeToTx )
{
    bool_t result;
    t_eFSP_MSGTX_TxCtx* ctxCur;

    (void)p_uDataToTxL;
    (void)p_uTimeToTx;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puDataToTx ) || ( NULL == p_uDataTxed ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TxCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;
        result = false;
    }

    return result;
}

static uint32_t m_tim_remainingTime;

bool_t eFSP_MSGTXTST_timStart ( t_eFSP_MSGTX_TimCtx* p_ptCtx, const uint32_t p_uTimeoutVal )
{
    bool_t result;
    t_eFSP_MSGTX_TimCtx* ctxCur;

    if( NULL == p_ptCtx )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TimCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;

        m_tim_remainingTime = p_uTimeoutVal;
        result = true;
    }

    return result;
}

bool_t eFSP_MSGTXTST_timGetRemaining ( t_eFSP_MSGTX_TimCtx* p_ptCtx, uint32_t* const p_puRemainings )
{
    bool_t result;
    t_eFSP_MSGTX_TimCtx* ctxCur;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puRemainings ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TimCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;

        if( m_tim_remainingTime > 0u )
        {
            m_tim_remainingTime--;
        }

        *p_puRemainings = m_tim_remainingTime;
        result = true;
    }

    return result;
}


bool_t eFSP_MSGTXTST_timStartErr ( t_eFSP_MSGTX_TimCtx* p_ptCtx, const uint32_t p_uTimeoutVal )
{
    bool_t result;
    t_eFSP_MSGTX_TimCtx* ctxCur;

    if( NULL == p_ptCtx )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TimCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;

        m_tim_remainingTime = p_uTimeoutVal;
        result = false;
    }

    return result;
}

bool_t eFSP_MSGTXTST_timGetRemainingErr ( t_eFSP_MSGTX_TimCtx* p_ptCtx, uint32_t* const p_puRemainings )
{
    bool_t result;
    t_eFSP_MSGTX_TimCtx* ctxCur;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puRemainings ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TimCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;

        if( m_tim_remainingTime > 0u )
        {
            m_tim_remainingTime--;
        }

        *p_puRemainings = m_tim_remainingTime;
        result = false;
    }

    return result;
}

bool_t eFSP_MSGTXTST_timGetRemainingCorr ( t_eFSP_MSGTX_TimCtx* p_ptCtx, uint32_t* const p_puRemainings )
{
    bool_t result;
    t_eFSP_MSGTX_TimCtx* ctxCur;

    if( ( NULL == p_ptCtx ) || ( NULL == p_puRemainings ) )
    {
        result = false;
    }
    else
    {
        ctxCur = (t_eFSP_MSGTX_TimCtx*)p_ptCtx;
        ctxCur->bTxIsError = true;
        if( m_tim_remainingTime < MAX_UINT32VAL )
        {
            m_tim_remainingTime++;
        }
        *p_puRemainings = m_tim_remainingTime + m_tim_remainingTime;

        if( m_tim_remainingTime < MAX_UINT32VAL )
        {
            m_tim_remainingTime++;
        }
        result = true;
    }

    return result;
}

/***********************************************************************************************************************
 *   PRIVATE FUNCTION
 **********************************************************************************************************************/
void eFSP_MSGTXTST_BadPointer(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;
    bool_t isInit;

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(NULL, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 1  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, NULL) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 2  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 2  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = NULL;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 3  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = NULL;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 4  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = NULL;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 5  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 5  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = NULL;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 6  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 6  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = NULL;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 7  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 7  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = NULL;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 8  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 8  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = NULL;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 9  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 9  -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = NULL;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 10 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 10 -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = NULL;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 11 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 11 -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = NULL;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 12 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 12 -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = NULL;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 13 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 13 -- FAIL \n");
    }

    /* Function */
    initData.puIMemArea = NULL;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 14 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 14 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_IsInit(NULL, &isInit) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 15 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 15 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_IsInit(&ctx, NULL) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 16 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 16 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_NewMessage(NULL, 10u) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 17 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 17 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_GetWherePutData(NULL, &dataP, &dataL) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 18 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 18 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_GetWherePutData(&ctx, NULL, &dataL) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 19 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 19 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, NULL) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 20 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 20 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_RestartMessage(NULL) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 21 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 21 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPOINTER == eFSP_MSGTX_SendChunk(NULL) )
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 22 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadPointer 22 -- FAIL \n");
    }
}

void eFSP_MSGTXTST_BadInit(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;
    bool_t isInit;

    /* Function */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    ctx.tMsgeCtx.tBSTFCtx.bIsInit = false;
    ctx.tMsgeCtx.ptCrcCtx = &ctxAdapterCrc;
    ctx.tMsgeCtx.fCrc = cbCrcPTest;
    ctx.puRxBuff = sendBuff;
    ctx.uTxBuffL = sizeof(sendBuff);
    ctx.uTxBuffCntr = 0u;
    ctx.uTxBuffFill = 0u;
    ctx.fTx = &eFSP_MSGTXTST_sendMsg;
    ctx.ptTxCtx = &ctxAdapterSend;
    ctx.tTxTim.ptTimCtx = &ctxAdapterTim;
    ctx.tTxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    ctx.tTxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    ctx.uTimeoutMs = 1000u;
    ctx.uTimePerSendMs = 100u;

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_IsInit(&ctx, &isInit) )
    {
        if( false == isInit )
        {
            (void)printf("eFSP_MSGTXTST_BadInit 1  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_BadInit 1  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadInit 1  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_NOINITLIB == eFSP_MSGTX_NewMessage(&ctx, 10u) )
    {
        (void)printf("eFSP_MSGTXTST_BadInit 2  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadInit 2  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_NOINITLIB == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        (void)printf("eFSP_MSGTXTST_BadInit 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadInit 3  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_NOINITLIB == eFSP_MSGTX_RestartMessage(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_BadInit 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadInit 4  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_NOINITLIB == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_BadInit 5  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadInit 5  -- FAIL \n");
    }
}

void eFSP_MSGTXTST_BadIniMsg(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    bool_t isInit;

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadIniMsg 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadIniMsg 1  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_NOINITMESSAGE == eFSP_MSGTX_RestartMessage(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_BadIniMsg 2  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadIniMsg 2  -- FAIL \n");
    }

    m_tim_remainingTime = 100u;
    if( e_eFSP_MSGTX_RES_NOINITMESSAGE == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_BadIniMsg 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadIniMsg 3  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_IsInit(&ctx, &isInit) )
    {
        if( true == isInit )
        {
            (void)printf("eFSP_MSGTXTST_BadIniMsg 4  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_BadIniMsg 4  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadIniMsg 4  -- FAIL \n");
    }
}

void eFSP_MSGTXTST_BadParamEntr(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = 0u;
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPARAM == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 1  -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 0u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPARAM == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 2  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 2  -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 0u;
    if( e_eFSP_MSGTX_RES_BADPARAM == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 3  -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 100u;
    initData.uITimePerSendMs = 1000u;
    if( e_eFSP_MSGTX_RES_BADPARAM == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 4  -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = 0u;
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_BADPARAM == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 5  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 5  -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 6  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 6  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_BADPARAM == eFSP_MSGTX_NewMessage(&ctx, 0u) )
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 2  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadParamEntr 2  -- FAIL \n");
    }
}

void eFSP_MSGTXTST_CorruptContext(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 1  -- FAIL \n");
    }

    /* Function */
    ctx.puRxBuff = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 2  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 2  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 3  -- FAIL \n");
    }

    /* Function */
    ctx.fTx = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 4  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 5  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 5  -- FAIL \n");
    }

    /* Function */
    ctx.ptTxCtx = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 6  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 6  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 7  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 7  -- FAIL \n");
    }

    /* Function */
    ctx.tTxTim.ptTimCtx = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 8  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 8  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 9  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 9  -- FAIL \n");
    }

    /* Function */
    ctx.tTxTim.fTimStart = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 10 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 10 -- FAIL \n");
    }


    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 11 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 11 -- FAIL \n");
    }

    /* Function */
    ctx.tTxTim.fTimGetRemain = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 12 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 12 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 13 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 13 -- FAIL \n");
    }

    /* Function */
    ctx.uTxBuffL = 0u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 14 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 14 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 15 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 15 -- FAIL \n");
    }

    /* Function */
    ctx.uTxBuffFill = ctx.uTxBuffL + 1u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 16 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 16 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 17 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 17 -- FAIL \n");
    }

    /* Function */
    ctx.uTxBuffCntr = ctx.uTxBuffFill + 1u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 18 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 18 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 19 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 19 -- FAIL \n");
    }

    /* Function */
    ctx.uTimeoutMs = 0u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 20 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 20 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 21 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 21 -- FAIL \n");
    }

    /* Function */
    ctx.uTimePerSendMs = 0u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 22 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 22 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 23 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 23 -- FAIL \n");
    }

    /* Function */
    ctx.uTimePerSendMs = ctx.uTimeoutMs + 1u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 24 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 24 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 25 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 25 -- FAIL \n");
    }

    /* Function */
    ctx.uTimePerSendMs = 0u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 26 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 26 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 27 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 27 -- FAIL \n");
    }

    /* Function */
    ctx.uTimePerSendMs = 0u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_RestartMessage(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 28 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 28 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 29 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 29 -- FAIL \n");
    }

    /* Function */
    ctx.uTimePerSendMs = 0u;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 30 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 30 -- FAIL \n");
    }

    /* Function */
    ctx.tMsgeCtx.ptCrcCtx = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 31 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 31 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 32 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 32 -- FAIL \n");
    }

    /* Function */
    ctx.tMsgeCtx.ptCrcCtx = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 33 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 33 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 34 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 34 -- FAIL \n");
    }

    /* Function */
    ctx.tMsgeCtx.ptCrcCtx = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_RestartMessage(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 35 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 35 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 36 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 36 -- FAIL \n");
    }

    /* Function */
    ctx.tMsgeCtx.ptCrcCtx = NULL;
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 37 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CorruptContext 37 -- FAIL \n");
    }
}

void eFSP_MSGTXTST_BadClBckCrc(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdaptEr;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckCrc 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckCrc 1  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_CRCCLBKERROR == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        if( e_eCU_CRC_RES_BADPOINTER == ctxAdapterCrc.eLastEr )
        {
            (void)printf("eFSP_MSGTXTST_BadClBckCrc 2  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_BadClBckCrc 2  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckCrc 2  -- FAIL \n");
    }
}

void eFSP_MSGTXTST_BadClBckSend(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsgErr;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckSend 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckSend 1  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("eFSP_MSGTXTST_BadClBckSend 2  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_BadClBckSend 2  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckSend 2  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckSend 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckSend 3  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_TXCLBKERROR == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckSend 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckSend 4  -- FAIL \n");
    }
}

void eFSP_MSGTXTST_BadClBckTim(void)
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStartErr;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemainingErr;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 1  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("eFSP_MSGTXTST_BadClBckTim 2  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_BadClBckTim 2  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 2  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    if( e_eFSP_MSGTX_RES_TIMCLBKERROR == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 3  -- FAIL \n");
    }

     /* Function */
    if( e_eFSP_MSGTX_RES_TIMCLBKERROR == eFSP_MSGTX_RestartMessage(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 4  -- FAIL \n");
    }

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemainingErr;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 5  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 5  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("eFSP_MSGTXTST_BadClBckTim 6  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_BadClBckTim 6  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 6  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 7  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 7  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_TIMCLBKERROR == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 8  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_BadClBckTim 8  -- FAIL \n");
    }

}

void eFSP_MSGTXTST_CornerCase(void)
{
    /* Init static */
    m_uSendWhen = 0u;
    m_uTxBuffCounter = 0u;

    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("msgTransmitterTestCornerCase 1  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 1  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("msgTransmitterTestCornerCase 2  -- OK \n");
        }
        else
        {
            (void)printf("msgTransmitterTestCornerCase 2  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 2  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    m_uTxBuffCounter = 0u;
    m_uSendWhen = 0u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("msgTransmitterTestCornerCase 3  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 3  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 4  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 4  -- FAIL \n");
    }

    if( ( ECU_SOF == m_auTxBuff[0u] ) || ( 0x30u == m_auTxBuff[1u] )  || ( 0x5Cu == m_auTxBuff[2u] ) ||
        ( 0xB1u == m_auTxBuff[3u]   ) || ( 0xD7u == m_auTxBuff[4u] )  || ( 0x02u == m_auTxBuff[5u] ) ||
        ( 0x00u == m_auTxBuff[6u]   ) || ( 0x00u == m_auTxBuff[7u] )  || ( 0x00u == m_auTxBuff[8u] ) ||
        ( 0x01u == m_auTxBuff[9u]   ) || ( 0x02u == m_auTxBuff[10u] ) || ( ECU_EOF == m_auTxBuff[11u] ) )
    {
        (void)printf("msgTransmitterTestCornerCase 5  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 5  -- FAIL \n");
    }


    /* Function */
    m_uTxBuffCounter = 0u;
    m_uSendWhen = 0u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_RestartMessage(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 6  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 6  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 7  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 7  -- FAIL \n");
    }

    if( ( ECU_SOF == m_auTxBuff[0u] ) || ( 0x30u == m_auTxBuff[1u] )  || ( 0x5Cu == m_auTxBuff[2u] ) ||
        ( 0xB1u == m_auTxBuff[3u]   ) || ( 0xD7u == m_auTxBuff[4u] )  || ( 0x02u == m_auTxBuff[5u] ) ||
        ( 0x00u == m_auTxBuff[6u]   ) || ( 0x00u == m_auTxBuff[7u] )  || ( 0x00u == m_auTxBuff[8u] ) ||
        ( 0x01u == m_auTxBuff[9u]   ) || ( 0x02u == m_auTxBuff[10u] ) || ( ECU_EOF == m_auTxBuff[11u] ) )
    {
        (void)printf("msgTransmitterTestCornerCase 8  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 8  -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = 1u;
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("msgTransmitterTestCornerCase 9  -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 9  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("msgTransmitterTestCornerCase 10 -- OK \n");
        }
        else
        {
            (void)printf("msgTransmitterTestCornerCase 10 -- FAIL \n");
        }
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 10 -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    m_uTxBuffCounter = 0u;
    m_uSendWhen = 0u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("msgTransmitterTestCornerCase 11 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 11 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 12 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 12 -- FAIL \n");
    }

    if( ( ECU_SOF == m_auTxBuff[0u] ) || ( 0x30u == m_auTxBuff[1u] )  || ( 0x5Cu == m_auTxBuff[2u] ) ||
        ( 0xB1u == m_auTxBuff[3u]   ) || ( 0xD7u == m_auTxBuff[4u] )  || ( 0x02u == m_auTxBuff[5u] ) ||
        ( 0x00u == m_auTxBuff[6u]   ) || ( 0x00u == m_auTxBuff[7u] )  || ( 0x00u == m_auTxBuff[8u] ) ||
        ( 0x01u == m_auTxBuff[9u]   ) || ( 0x02u == m_auTxBuff[10u] ) || ( ECU_EOF == m_auTxBuff[11u] ) )
    {
        (void)printf("msgTransmitterTestCornerCase 13 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 13 -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = 1u;
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 1u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("msgTransmitterTestCornerCase 14 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 14 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("msgTransmitterTestCornerCase 15 -- OK \n");
        }
        else
        {
            (void)printf("msgTransmitterTestCornerCase 15 -- FAIL \n");
        }
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 15 -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    m_uTxBuffCounter = 0u;
    m_uSendWhen = 0u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("msgTransmitterTestCornerCase 16 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 16 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 17 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 17 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 18 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 18 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 19 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 19 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 20 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 20 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 21 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 21 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 22 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 22 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 23 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 23 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 24 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 24 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 25 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 25 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 26 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 26 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 27 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 27 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 28 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 28 -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 29 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 29 -- FAIL \n");
    }


    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("msgTransmitterTestCornerCase 30 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 30 -- FAIL \n");
    }

    if( ( ECU_SOF == m_auTxBuff[0u] ) || ( 0x30u == m_auTxBuff[1u] )  || ( 0x5Cu == m_auTxBuff[2u] ) ||
        ( 0xB1u == m_auTxBuff[3u]   ) || ( 0xD7u == m_auTxBuff[4u] )  || ( 0x02u == m_auTxBuff[5u] ) ||
        ( 0x00u == m_auTxBuff[6u]   ) || ( 0x00u == m_auTxBuff[7u] )  || ( 0x00u == m_auTxBuff[8u] ) ||
        ( 0x01u == m_auTxBuff[9u]   ) || ( 0x02u == m_auTxBuff[10u] ) || ( ECU_EOF == m_auTxBuff[11u] ) )
    {
        (void)printf("msgTransmitterTestCornerCase 31 -- OK \n");
    }
    else
    {
        (void)printf("msgTransmitterTestCornerCase 31 -- FAIL \n");
    }
}

void eFSP_MSGTXTST_CornerCase2(void)
{
    /* Init static */
    m_uSendWhen = 0u;
    m_uTxBuffCounter = 0u;

    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = 5u;
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsgOnce;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 3u;
    initData.uITimePerSendMs = 3u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 1  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("eFSP_MSGTXTST_CornerCase2 2  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_CornerCase2 2  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 2  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    m_uTxBuffCounter = 0u;
    m_uSendWhen = 0u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 3  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGETIMEOUT == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 4  -- FAIL \n");
    }

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = 3u;
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsgOnce;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 12u;
    initData.uITimePerSendMs = 5u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 6  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 6  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("eFSP_MSGTXTST_CornerCase2 7  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_CornerCase2 7  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 7  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    m_uTxBuffCounter = 0u;
    m_uSendWhen = 0u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 8  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 8  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 9  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 9  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 10 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 10 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 11 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 11 -- FAIL \n");
    }


    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGESENDED == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 12 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 12 -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_MESSAGETIMEOUT == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 13 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 13 -- FAIL \n");
    }

    if( ( ECU_SOF == m_auTxBuff[0u] ) || ( 0x30u == m_auTxBuff[1u] )  || ( 0x5Cu == m_auTxBuff[2u] ) ||
        ( 0xB1u == m_auTxBuff[3u]   ) || ( 0xD7u == m_auTxBuff[4u] )  || ( 0x02u == m_auTxBuff[5u] ) ||
        ( 0x00u == m_auTxBuff[6u]   ) || ( 0x00u == m_auTxBuff[7u] )  || ( 0x00u == m_auTxBuff[8u] ) ||
        ( 0x01u == m_auTxBuff[9u]   ) || ( 0x02u == m_auTxBuff[10u] ) || ( ECU_EOF == m_auTxBuff[11u] ) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 14 -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase2 14 -- FAIL \n");
    }
}

void eFSP_MSGTXTST_CornerCase3()
{
    /* Local variable */
    t_eFSP_MSGTX_Ctx ctx;
    t_eFSP_MSGTX_InitData initData;
    f_eFSP_MSGE_CrcCb cbCrcPTest = &eFSP_MSGTXTST_c32SAdapt;
    t_eFSP_MSGE_CrcCtx ctxAdapterCrc;
    t_eFSP_MSGTX_TxCtx ctxAdapterSend;
    t_eFSP_MSGTX_TimCtx ctxAdapterTim;
    uint8_t  memArea[10u];
    uint8_t  sendBuff[10u];
    uint8_t* dataP;
    uint32_t dataL;

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsgCorr;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemaining;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 1  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 1  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("eFSP_MSGTXTST_CornerCase3 2  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_CornerCase3 2  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 2  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 3  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 3  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 4  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 4  -- FAIL \n");
    }

    /* Clear */
    (void)memset(&ctx, 0, sizeof(t_eFSP_MSGTX_Ctx));
    (void)memset(&initData, 0, sizeof(t_eFSP_MSGTX_InitData));

    /* Init */
    initData.puIMemArea = memArea;
    initData.uIMemAreaL = sizeof(memArea);
    initData.puITxBuffArea = sendBuff;
    initData.uITxBuffAreaL = sizeof(sendBuff);
    initData.fICrc = cbCrcPTest;
    initData.ptICbCrcCtx = &ctxAdapterCrc;
    initData.fITx = &eFSP_MSGTXTST_sendMsg;
    initData.ptICbTxCtx = &ctxAdapterSend;
    initData.tITxTim.ptTimCtx = &ctxAdapterTim;
    initData.tITxTim.fTimStart = &eFSP_MSGTXTST_timStart;
    initData.tITxTim.fTimGetRemain = &eFSP_MSGTXTST_timGetRemainingCorr;
    initData.uITimeoutMs = 1000u;
    initData.uITimePerSendMs = 100u;

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_InitCtx(&ctx, &initData) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 5  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 5  -- FAIL \n");
    }

    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_GetWherePutData(&ctx, &dataP, &dataL) )
    {
        if( 2u == dataL )
        {
            (void)printf("eFSP_MSGTXTST_CornerCase3 6  -- OK \n");
        }
        else
        {
            (void)printf("eFSP_MSGTXTST_CornerCase3 6  -- FAIL \n");
        }
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 6  -- FAIL \n");
    }

    /* Function */
    dataP[0u] = 0x01u;
    dataP[1u] = 0x02u;
    if( e_eFSP_MSGTX_RES_OK == eFSP_MSGTX_NewMessage(&ctx, 2u) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 7  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 7  -- FAIL \n");
    }

    /* Function */
    if( e_eFSP_MSGTX_RES_CORRUPTCTX == eFSP_MSGTX_SendChunk(&ctx) )
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 8  -- OK \n");
    }
    else
    {
        (void)printf("eFSP_MSGTXTST_CornerCase3 8  -- FAIL \n");
    }

}

#ifdef __IAR_SYSTEMS_ICC__
    #pragma cstat_restore = "MISRAC2012-Rule-10.3", "CERT-STR32-C", "MISRAC2012-Rule-11.5", "CERT-EXP36-C_b", \
                            "MISRAC2012-Rule-8.9_a", "MISRAC2012-Rule-8.13", "MISRAC2012-Rule-2.2_b", "MISRAC2004-16.7"
#endif