/**
 * @file       eFSP_MSGTXTST.h
 *
 * @brief      Message trasmitter test
 *
 * @author     Lorenzo Rosin
 *
 **********************************************************************************************************************/

#ifndef EFSPMSGTRANSMITTERTEST_H
#define EFSPMSGTRANSMITTERTEST_H



#ifdef __cplusplus
extern "C" {
#endif



/***********************************************************************************************************************
 *      INCLUDES
 **********************************************************************************************************************/
#include "eFSP_TYPE.h"
#include "eFSP_MSGTX.h"



/***********************************************************************************************************************
 * GLOBAL PROTOTYPES
 **********************************************************************************************************************/
/**
 * @brief       Test the message transmitter module
 *
 * @param[in]   none
 *
 * @return      none
 */
void eFSP_TEST_msgTransmitter(void);



#ifdef __cplusplus
} /* extern "C" */
#endif



#endif /* EFSPMSGTRANSMITTERTEST_H */