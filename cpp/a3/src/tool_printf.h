#ifndef _TOOL_PRINRF_
#define _TOOL_PRINRF_

#include<stdlib.h>
#include<string.h>
#include<chip_define.h>

#define _disp_err(fmt, ...) printf("	error - " fmt "\r\n", ##__VA_ARGS__)

void printf_chip_info(chip_info *pchip);
void printf_app_end(void);

#endif
