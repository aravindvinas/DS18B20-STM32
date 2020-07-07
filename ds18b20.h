#ifndef __DS18B20_H
#define __DS18B20_H

#include "onewire.h"

//Resolution varies from 9-12 bits
#define RES_9B  9 
#define RES_10B  10 
#define RES_11B  11 
#define RES_12B  12 

(uint8_t) DS18B20_Init(datapin_Typedef* dq, searchstate_Typedef* ss);

(void)    Conv_Temp(datapin_Typedef* dq, searchstate_Typedef* ss, uint8_t* addr);    

(void)    Conv_All(datapin_Typdef* dq);

(uint8_t) Read_Temp(datapin_Typdef* dq, searchstate_Typedef* ss, float* ret_val);   

(uint8_t) Get_Res(datapin_Typedef* dq, searchstate_Typedef* ss);

(void)    Set_Res(datapin_Typedef* dq, searchstate_Typedef* ss, uint8_t res);

#endif

