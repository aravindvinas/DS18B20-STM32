/**
  * file:   OneWire.c 
  * author: Vinas
  * brief:  OneWire protocol header file
  */

#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include "stm32f3xx_hal.h"

//ROM COMMANDS
#define ROM_SEARCH    0xF0
#define ROM_READ      0X33
#define ROM_MATCH     0X55
#define ROM_SKIP      0XCC
#define ALARM_SEARCH  0XEC

//FUNCTION COMMANDS SPECIFIC TO DS18B20
#define CONV_TEMP     0x44
#define WRITE_SCP     0x4E
#define READ_SCP      0xBE
#define CPY_SCP		  0x48	
#define RECALL_E2     0xB8
#define READ_PWSUP    0xB4

//DATA LINE PIN CONFIG STRUCT
typedef struct{
	GPIO_Typedef GPIOx;
	uint16_t GPIO_Pin;
	}datapin_Typedef;	

//SEARCH STATE STRUCT
typedef struct{
	uint8_t LastDiscrepancy;
	uint8_t LastFamilyDiscrepancy;
	uint8_t LastDiscrepancy;
	uint8_t ROM[8];	
}searchstate_Typedef;

//PIN CONTROL API's
void Micro_Delay(uint16_t delay);
void Mode_Input(datapin_Typedef* dq);
void Mode_Output(datapin_Typedef* dq);

//ONEWIRE API's
(void)    OW_Init(datapin_Typedef* dq, GPIO_Typdef* GPIOx, uint16_t GPIO_Pin);

(void)    OW_Reset(datapin_Typedef* dq);

(void)    OW_WriteByte(datapin_Typdef* dq);

(void)    OW_WriteBit(datapin_Typdef* dq);

(uint8_t) OW_ReadByte(datapin_Typdef* dq);

(uint8_t) OW_ReadBit(datapin_Typdef* dq);

(uint8_t) OW_FirstDev(searchstate_Typedef* ss);

(uint8_t) OW_NextDev(searchstate_Typedef* ss)

(void)    OW_ResetSearch(searchstate_Typedef* ss);

(uint8_t) OW_Search(datapin_Typedef* dq, searchstate_Typedef* ss, uint8_t ROM_CMD);

(void)    OW_SelectROM(datapin_Typedef* dq, searchstate_Typedef* ss);

(uint8_t) CRC8_CHK(uint8_t* addr, uint8_t len); 

#endif
