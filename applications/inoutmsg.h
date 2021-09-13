
#ifndef APPLICATIONS_YGP_INOUTMSG_H_
#define APPLICATIONS_YGP_INOUTMSG_H_

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOPIN4  GET_PIN(C, 0)
#define DOPIN3  GET_PIN(B, 12)
#define DOPIN2  GET_PIN(B, 8)
#define DOPIN1  GET_PIN(B, 9)

typedef enum
{
    INLEN_ERR = -1,                 /*!< in strlen����sizeofin. */
    CMD_ERR = -2,                   /*!< CMD����. */
    KEY_ERR = -3,                   /*!< ������. */
    SETVALUE_ERR = -4,              /*!< ����ֵ����. */
    MB_MRE_TIMEDOUT,                /*!< timeout error occurred. */
    MB_MRE_MASTER_BUSY,             /*!< master is busy now. */
    MB_MRE_EXE_FUN                  /*!< execute function error. */
} iomErrCode;

int iom1(char* in,int sizeofin,char* out,int sizeofout);

#endif /* APPLICATIONS_YGP_INOUTMSG_H_ */
