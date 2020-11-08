#ifndef __SYSTEM_MANAGE_H_
#define __SYSTEM_MANAGE_H_

#include <stdint.h>

typedef struct _BASE_OPS 
{
	
}BASE_STRUCT;

typedef struct _DEVICE_BASE_OPS 
{
  int8_t (* init)          (void);
  int8_t (* deinit)        (void);
  int8_t (* read)       	 (uint8_t, uint8_t * , uint16_t);   
  int8_t (* write)       (uint8_t *, uint32_t *);  

}DEV_BASE_STRUCT;








#endif




