/**
 * @file eCUType.h
 *
 */

#ifndef ECU_TYPE_H
#define ECU_TYPE_H



#ifdef __cplusplus
extern "C" {
#endif



/***********************************************************************************************************************
 *      INCLUDES
 **********************************************************************************************************************/
#include "eCUStandardType.h"



/***********************************************************************************************************************
 *      TYPEDEFS
 **********************************************************************************************************************/
typedef enum
{
    ECU_RES_OK = 0,
    ECU_RES_BADPARAM,
    ECU_RES_BADPOINTER,
    ECU_RES_OUTOFMEM,
    ECU_RES_NOINITLIB,
}e_eCU_Res;



#ifdef __cplusplus
} /* extern "C" */
#endif



#endif /* ECU_TYPE_H */
