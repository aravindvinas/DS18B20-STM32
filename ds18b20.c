#include "ds18b20.h"

uint8_t count = 0;   //Global Variable to keep count of devices in the bus

/**brief:  Count number of dev on the bus
  *param:  searchstate_Typedef instance
  *retval: 1->Dev present, 0->No dev present
  */
uint8_t Count_Dev(searchstate_Typedef* ss)
{
	if(!OW_FirstDev(ss)){
		return 0;
	}

	count = 1;

	while(OW_NextDev(ss)){
		count++;
	}

	return 1;
}

/**brief:  Initialises OneWire protocol
  *param:  None
  *retval: 1->Dev present, 0->No device present
  */
uint8_t DS18B20_Init(datapin_Typdef* dq, searchstate_Typedef* ss)
{
	OW_Init(dq, GPIOx, GPIO_Pin);

	return Count_Dev(ss);
}

/**brief:  Start temp conversion on last searched device
  *param:  datapin_Typedef instance, searchstate_Typdef instance 
  *retval: None
  */
void Conv_Temp(datapin_Typedef* dq, searchstate_Typedef* ss)
{
	OW_Reset(dq);

	OW_SelectROM(dq, ss);

	OW_WriteByte(dq, CONV_TEMP);
}

/**brief:  Start temp conversion on all dev
  *param:  datapin_Typedef instance 
  *retval: None
  */
void Conv_All(datapin_Typdef* dq)
{
	OW_Reset(dq);

	OW_WriteByte(dq, ROM_SKIP);

	OW_WriteByte(dq, CONV_TEMP);
}

/**brief:  Read Temperature from last searched ROM
  *param:  datapin_Typdef instance, searchstate_Typdef instance, pointer to float memory
  *retval: 1->SUCCESS, 0->ERROR
  */
uint8_t Read_Temp(datapinTypedef* dq, searchstate_Typedef* ss, float* ret_val)
{
	uint16_t temp;
	int8_t   digit, neg = 0;
	float    decimal;
	uint8_t  data[9];
	uint8_t  res, i, crc;

	if(!OW_ReadBit(dq)){
		return 0;
	}

    OW_SelectROM(dq, ss);

	OW_WriteByte(dq, READ_SCP);

	for(i=0; i<9; i++){
		data[i] = OW_ReadByte(dq);
	}

 	crc =  CRC8_CHK(data, 8);

	if(crc != data[8]){
		return 0;
	}

	temp = data[0] | (data[1] << 8);

	OW_Reset(dq);

	if(temp & 0x8000){
		temp = ~temp+1;
		neg  = 1;
	}

	res = ((data[4] & 0x60) >> 5) + 9;

	digit = temp >> 4;
	digit |= ((temp >> 8) & 0x7) << 4;

	switch (resolution)
	{
		case 9:
			decimal = (temp >> 3) & 0x01;
			decimal *= (float)DS18B20_DECIMAL_STEPS_9BIT;
		break;

		case 10:
			decimal = (temp >> 2) & 0x03;
			decimal *= (float)DS18B20_DECIMAL_STEPS_10BIT;
		 break;

		case 11:
			decimal = (temp >> 1) & 0x07;
			decimal *= (float)DS18B20_DECIMAL_STEPS_11BIT;
		break;

		case 12:
			decimal = temp & 0x0F;
			decimal *= (float)DS18B20_DECIMAL_STEPS_12BIT;
		 break;

		default:
			decimal = 0xFF;
			digit = 0;
	}

    decimal = digit + decimal;
	if(minus){
		decimal = 0 - decimal;
	}

	*ret_val = decimal;

	return 1
}

