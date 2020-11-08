#ifndef __CHECK_BUFFER_H
#define __CHECK_BUFFER_H
#include "crc.h"
#include "flash_if.h"
#include "Communication.h"
#pragma anon_unions
typedef union {
    struct {
        uint32_t crc32;
        uint32_t version;
        uint32_t length;
        uint8_t user_define[1012];
        uint8_t  program_data[1];
    };
    uint8_t buffer[1024];
} IAP_HeadFrame;
uint8_t IAP_IsValidProgram( const IAP_HeadFrame *p_iap_frame );
void check_buffer(void);
void Jump_Appcation(void);

#endif
