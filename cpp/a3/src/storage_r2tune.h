#ifndef _STORAGE_R2TURN_
#define _STORAGE_R2TURN_

#include<R2TuneTable.h>
vk_result storage_r2tune_set(R2TuneTable *pr2, uint8_t *psection);
vk_result storage_r2tune_get(R2TuneTable *pr2, uint8_t *psection);

#endif
