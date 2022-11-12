/**
 * @file       eFSPMsgTransmitter.h
 *
 * @brief      Message transmitter utils
 *
 * @author     Lorenzo Rosin
 *
 **********************************************************************************************************************/

#ifndef EFSPMSGTRANSMITTER_H
#define EFSPMSGTRANSMITTER_H



#ifdef __cplusplus
extern "C" {
#endif



/***********************************************************************************************************************
 *      INCLUDES
 **********************************************************************************************************************/
#include "eFSPType.h"
#include "eFSPMsgEncoder.h"



/***********************************************************************************************************************
 *      TYPEDEFS
 **********************************************************************************************************************/

/* Call back to a function that will trasmit the passed data.
 * the cntx parameter is a custom pointer that can be used by the creator of this TX callback, and will not be used
 * by the MSG TRANSMITTER module */
typedef bool_t (*cb_tx_msge) ( void* cntx, const uint8_t dataToSend[], const uint32_t dataToSendLen,
                               uint32_t* const dataSended, const uint32_t timeToSend, uint32_t* const timeElapsed );

typedef enum
{
    MSGTX_RES_OK = 0,
    MSGTX_RES_BADPARAM,
    MSGTX_RES_BADPOINTER,
	MSGTX_RES_CORRUPTCTX,
    MSGTX_RES_MESSAGESENDED,
    MSGTX_RES_MESSAGETIMEOUT,
    MSGTX_RES_NOINITLIB,
    MSGTX_RES_NOINITMESSAGE,
	MSGTX_RES_CRCCLBKERROR,
    MSGTX_RES_TXCLBKERROR,
}e_eFSP_MSGTX_Res;

typedef struct
{
    s_eFSP_MSGE_Ctx msgEncoderCtnx;
	uint8_t*        sendBuff;
	uint32_t        sendBuffSize;
	uint32_t        sendBuffCntr;
    uint32_t        sendBuffFill;
    cb_tx_msge      cbTxP;
    void*           cbTxCtx;
    uint32_t        timeCounterMs;
    uint32_t        frameTimeoutMs;
    uint32_t        timePerSendMs;
}s_eFSP_MSGTX_Ctx;

typedef struct
{
    uint8_t*        i_memArea;
    uint32_t        i_memAreaSize;
    uint8_t*        i_sendBuffArea;
    uint32_t        i_sendBuffAreaSize;
    cb_crc32_msge   i_cbCrcP;
    void*           i_cbCrcCtx;
    cb_tx_msge      i_cbTxP;
    void*           i_cbTxCtx;
    uint32_t        i_frameTimeoutMs;
    uint32_t        i_timePerSendMs;
}s_eFSP_MSGTX_InitData;


/***********************************************************************************************************************
 * GLOBAL PROTOTYPES
 **********************************************************************************************************************/
/**
 * @brief       Initialize the message transmitter context
 *
 * @param[in]   ctx         - Message transmitter context
 * @param[in]   initData    - Init data
 *
 * @return      MSGTX_RES_BADPOINTER   - In case of bad pointer passed to the function
 *		        MSGTX_RES_BADPARAM     - In case of an invalid parameter passed to the function
 *              MSGTX_RES_OK           - Operation ended correctly
 */
e_eFSP_MSGTX_Res MSGTX_InitCtx(s_eFSP_MSGTX_Ctx* const ctx, const s_eFSP_MSGTX_InitData* initData);

/**
 * @brief       Start to encode a new msg given the dimension of raw payload it self. This function suppouse that
 *              data payload that need to be encoded were already copied in memory.( see MSGTX_GetPayloadLocation
 *              in order to know how get the data pointer, and copy the data )
 *
 * @param[in]   ctx         - Message Transmitter context
 * @param[in]   messageLen  - lenght of the raw payload present in the frame that we need to encode ( no header )
 *
 * @return      MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        MSGTX_RES_BADPARAM       - In case of an invalid parameter passed to the function
 *		        MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *				MSGTX_RES_CRCCLBKERROR   - The crc callback function returned an error
 *              MSGTX_RES_OK             - Operation ended correctly
 */
e_eFSP_MSGTX_Res MSGTX_StartNewMessage(s_eFSP_MSGTX_Ctx* const ctx, const uint32_t messageLen);

/**
 * @brief       Retrive the pointer of the buffer that the user can use to insert data payload that need to be encoded
 *
 * @param[in]   ctx         - Message Transmitter context
 * @param[out]  dataP       - Pointer to a Pointer where the raw data needs to be copied before starting a message
 * @param[out]  maxDataSize - Pointer to a uint32_t variable where the max number of data that can be copied in dataP
 *                            will be placed
 *
 * @return      MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *              MSGTX_RES_OK             - Operation ended correctly
 */
e_eFSP_MSGTX_Res MSGTX_GetPayloadLocation(s_eFSP_MSGTX_Ctx* const ctx, uint8_t** dataP, uint32_t* const maxDataSize);

/**
 * @brief       Restart to encode and send the already passed payload/the current frame
 *
 * @param[in]   ctx         - Message Transmitter context
 *
 * @return      MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        MSGTX_RES_NOINITMESSAGE  - Need to start a message before restarting it
 *		        MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *              MSGTX_RES_OK             - Operation ended correctly
 */
e_eFSP_MSGTX_Res MSGTX_RestartCurrentMessage(s_eFSP_MSGTX_Ctx* const ctx);

/**
 * @brief       Send encoded data chunk. The raw data copied in the buffer by using the function
 *              MSGTX_GetPayloadLocation will be encoded (header and byte stuffing) and sended by this function.
 *              The whole message can be sended calling multiple times this function. Eache time this function will
 *              try to send all the data that can be send in "i_timePerSendMs". The whole frame instead can be sended
 *              in "i_frameTimeoutMs" milliseconds.
 *
 * @param[in]   ctx         - Message Transmitter context
 *
 * @return      MSGTX_RES_BADPOINTER     - In case of bad pointer passed to the function
 *		        MSGTX_RES_NOINITLIB      - Need to init the data encoder context before taking some action
 *		        MSGTX_RES_BADPARAM       - In case of an invalid parameter passed to the function
 *		        MSGTX_RES_NOINITMESSAGE  - Need to start a message before taking some action
 *		        MSGTX_RES_CORRUPTCTX     - In case of an corrupted context
 *              MSGTX_RES_MESSAGESENDED  - No more data that we can send, restart or start a new msg to proceed.
 *                                         This means that we have finished sending the current message, without
 *                                         any particular error or timeout.
 *              MSGTX_RES_MESSAGETIMEOUT - The message is not sended before "i_frameTimeoutMs". Restart to continue.
 *              MSGTX_RES_TXCLBKERROR    - Some error reported by the user send function. Restart to continue.
 *              MSGTX_RES_OK             - Operation ended correctly, message is not still fully encoded. This happnes
 *                                         when the whole message wasn't sended in "i_timePerSendMs" millisecond, but
 *                                         the "i_frameTimeoutMs" timeout is still not reached. Call this function again
 *                                         to send another chunk of data.
 */
e_eFSP_MSGTX_Res MSGTX_SendChunk(s_eFSP_MSGTX_Ctx* const ctx);



#ifdef __cplusplus
} /* extern "C" */
#endif



#endif /* EFSPMSGTRANSMITTER_H */