#ifndef __FRAME_STATISTICAL_H__
#define __FRAME_STATISTICAL_H__
#include "protocol.h"
extern unsigned char CONTOTX2_INFO_ID;
extern STATISTICS_CONTOTX2_MSG *contotx2_buf;
extern STATISTICS_TX2toCON_msg *tx2tocon_buf;
void MainBoard_SendFrame_Statistical(unsigned char *data);
void MainBoard_RecivceFrame_Statistical(unsigned char INFO_ID);
void Cache_contotx2_framebuffer(STATISTICS_CONTOTX2_MSG *data);
void Cache_tx2tocon_framebuffer(STATISTICS_TX2toCON_msg *data);
void Free_contotx2_buffer(void);
void Free_tx2tocon_buffer(void);
#endif
