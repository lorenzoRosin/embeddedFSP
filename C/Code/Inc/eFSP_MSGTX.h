/**
 * @file       eFSP_MSGTX.h
 *
 * @brief      Message transmitter utils
 *
 * @author     Lorenzo Rosin
 *
 **********************************************************************************************************************/

#ifndef EFSP_MSGTX_H
#define EFSP_MSGTX_H



#ifdef __cplusplus
extern "C" {
#endif



/***********************************************************************************************************************
 *      INCLUDES
 **********************************************************************************************************************/
#include "eFSP_TYPE.h"
#include "eFSP_MSGE.h"



/***********************************************************************************************************************
 *      TYPEDEFS
 **********************************************************************************************************************/

/* Call back to a function that will trasmit the passed data.
 * the p_ptCtx parameter is a custom pointer that can be used by the creator of this TX callback, and will not be used
 * by the MSG TRANSMITTER module */
typedef bool_t (*f_eFSP_MSGTX_TxCb) ( void* p_ptCtx, const uint8_t* p_puDataToTx, const uint32_t p_uDataToTxL,
                               uint32_t* const p_puDataTxL, const uint32_t p_uTimeTx );

/* Call backs to functions that act as a timer */
typedef bool_t (*f_eFSP_MSGTX_TimStart) ( void* p_ptCtx, const uint32_t p_uTimeout );
typedef bool_t (*f_eFSP_MSGTX_TimGetRemaing) ( void* p_ptCtx, uint32_t* const p_puRemain );

typedef struct
{
    void*                       ptTimCtx;
    f_eFSP_MSGTX_TimStart       fTimStart;
    f_eFSP_MSGTX_TimGetRemaing  fTimGetRemain;
}t_eFSP_MSGTX_Timer;

typedef enum
{
    e_eFSP_MSGTX_RES_OK = 0,
    e_eFSP_MSGTX_RES_BADPARAM,
    e_eFSP_MSGTX_RES_BADPOINTER,
	e_eFSP_MSGTX_RES_CORRUPTCTX,
    e_eFSP_MSGTX_RES_MESSAGESENDED,
    e_eFSP_MSGTX_RES_MESSAGETIMEOUT,
    e_eFSP_MSGTX_RES_NOINITLIB,
    e_eFSP_MSGTX_RES_NOINITMESSAGE,
	e_eFSP_MSGTX_RES_CRCCLBKERROR,
    e_eFSP_MSGTX_RES_TXCLBKERROR,
    e_eFSP_MSGTX_RES_TIMCLBKERROR
}e_eFSP_MSGTX_RES;

typedef struct
{
    t_eFSP_MSGE_Ctx msge_Ctx;
	uint8_t*        p_rxBuff;
	uint32_t        txBuffSize;
	uint32_t        txBuffCntr;
    uint32_t        txBuffFill;
    f_eFSP_MSGTX_TxCb      f_Tx;
    void*           p_TxCtx;
    t_eFSP_MSGTX_Timer  txTim;
    uint32_t        timeoutMs;
    uint32_t        timePerSendMs;
}t_eFSP_MSGTX_Ctx;

typedef struct
{
    uint8_t*        puIMemArea;
    uint32_t        uIMemAreaL;
    uint8_t*        p_i_txBuffArea;
    uint32_t        i_txBuffAreaSize;
    f_eFSP_MSGE_CrcCb   fICrc;
    t_eFSP_MSGE_CrcCtx*           ptICbCrcCtx;
    f_eFSP_MSGTX_TxCb      f_i_Tx;
    void*           p_i_cbTxCtx;
    t_eFSP_MSGTX_Timer  i_txTim;
    uint32_t        uITimeoutMs;
    uint32_t        i_timePerSendMs;
}t_eFSP_MSGTX_InitData;



/***********************************************************************************************************************
 * GLOBAL PROTOTYPES
 **********************************************************************************************************************/
/**
 * @brief       Initialize the message transmitter context
 *
 * @param[in]   p_ptCtx         - Message transmitter context
 * @param[in]   p_initData    - Init data
 *
 * @return      e_eFSP_MSGTX_RES_BADPOINTER   - In case of bad pointer passed to the function
 *		        e_eFSP_MSGTX_RES_BADPARAM     - In case of an invalid parameter passed to the function
 *              e_eFSP_MSGTX_RES_OK           - Operation ended correctly
 */
e_eFSP_MSGTX_RES eFSP_MSGTX_InitCtx(t_eFSP_MSGTX_Ctx* const p_ptCtx, const t_eFSP_MSGTX_InitData* p_initData);

/**
 * @brief       Check if the lib is initialized
 *
 * @param[in]   p_ptCtx         - Message transmitter context
 * @param[out]  p_pbIsInit      - Pointer to a bool_t variable that will be filled with true if the lib is initialized
 *
 * @return      e_eFSP_MSGTX_RES_BADPOINTER    - In case of bad pointer passed to the function
 *              e_eFSP_MSGTX_RES_OK            - Operation ended correctly
 */
e_eFSP_MSGTX_RES eFSP_MSGTX_IsInit(t_eFSP_MSGTX_Ctx* const p_ptCtx, bool_t* p_pbIsInit);

/**
 * @brief       Retrive the pointer of the buffer that the user can use to insert data payload that need to be encoded
 *
 * @param[in]   p_ptCtx         - Message Transmitter context
 * @param[out]  pp_data       - Pointer to a Pointer where the raw data needs to be copied before starting a message
 * @param[out]  p_maxDL       - Pointer to a uint32_t variable where the max number of data that can be copied in
 *                              pp_data will be placed
 *
 * @return      e_eFSP_MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        e_eFSP_MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        e_eFSP_MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *              e_eFSP_MSGTX_RES_OK             - Operation ended correctly
 */
e_eFSP_MSGTX_RES eFSP_MSGTX_GetWherePutData(t_eFSP_MSGTX_Ctx* const p_ptCtx, uint8_t** pp_data, uint32_t* const p_maxDL);

/**
 * @brief       Start to encode a new msg given the dimension of raw payload it self. This function suppouse that
 *              data payload that need to be encoded were already copied in memory.( see MSGTX_GetPayloadLocation
 *              in order to know how get the data pointer, and copy the data )
 *
 * @param[in]   p_ptCtx         - Message Transmitter context
 * @param[in]   messageLen    - lenght of the raw payload present in the frame that we need to encode ( no header )
 *
 * @return      e_eFSP_MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        e_eFSP_MSGTX_RES_BADPARAM       - In case of an invalid parameter passed to the function
 *		        e_eFSP_MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        e_eFSP_MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *				e_eFSP_MSGTX_RES_CRCCLBKERROR   - The crc callback function returned an error
 *              e_eFSP_MSGTX_RES_TIMCLBKERROR   - The timer function returned an error
 *              e_eFSP_MSGTX_RES_OK             - Operation ended correctly
 */
e_eFSP_MSGTX_RES eFSP_MSGTX_NewMessage(t_eFSP_MSGTX_Ctx* const p_ptCtx, const uint32_t messageLen);

/**
 * @brief       Restart to encode and send the already passed payload/the current frame
 *
 * @param[in]   p_ptCtx         - Message Transmitter context
 *
 * @return      e_eFSP_MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        e_eFSP_MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        e_eFSP_MSGTX_RES_NOINITMESSAGE  - Need to start a message before restarting it
 *		        e_eFSP_MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *              e_eFSP_MSGTX_RES_TIMCLBKERROR   - The timer function returned an error
 *              e_eFSP_MSGTX_RES_OK             - Operation ended correctly
 */
e_eFSP_MSGTX_RES eFSP_MSGTX_RestartMessage(t_eFSP_MSGTX_Ctx* const p_ptCtx);

/**
 * @brief       Send encoded data chunk. The raw data copied in the buffer by using the function
 *              MSGTX_GetPayloadLocation will be encoded (header and byte stuffing) and sended by this function.
 *              The whole message can be sended calling multiple times this function. Eache time this function will
 *              try to send all the data that can be send in "i_timePerSendMs". The whole frame instead can be sended
 *              in "uITimeoutMs" milliseconds. This function can return different status, but if we keep call
 *              this function even after uITimeoutMs it will start returning only e_eFSP_MSGTX_RES_MESSAGETIMEOUT.
 *
 * @param[in]   p_ptCtx         - Message Transmitter context
 *
 * @return      e_eFSP_MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        e_eFSP_MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        e_eFSP_MSGTX_RES_BADPARAM       - In case of an invalid parameter passed to the function
 *		        e_eFSP_MSGTX_RES_NOINITMESSAGE  - Need to start a message before taking some action
 *		        e_eFSP_MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *              e_eFSP_MSGTX_RES_MESSAGESENDED  - No more data that we can send, restart or start a new msg to proceed.
 *                                         This means that we have finished sending the current message, without
 *                                         any particular error or timeout.
 *              e_eFSP_MSGTX_RES_MESSAGETIMEOUT - The message is not sended before "uITimeoutMs". Restart to continue.
 *              e_eFSP_MSGTX_RES_TXCLBKERROR    - Some error reported by the user send function. Restart to continue.
 *              e_eFSP_MSGTX_RES_TIMCLBKERROR   - The timer function returned an error
 *              e_eFSP_MSGTX_RES_OK             - Operation ended correctly, message is not fully sended. This happnes
 *                                         when the whole message wasn't sended in "i_timePerSendMs" millisecond, but
 *                                         the "uITimeoutMs" timeout is still not reached. Call this function again
 *                                         to send another chunk of data.
 */
e_eFSP_MSGTX_RES eFSP_MSGTX_SendChunk(t_eFSP_MSGTX_Ctx* const p_ptCtx);



#ifdef __cplusplus
} /* extern "C" */
#endif



#endif /* EFSP_MSGTX_H */