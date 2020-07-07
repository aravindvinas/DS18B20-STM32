/**
  * file:   OneWire.c 
  * author: Vinas
  * brief:  OneWire protcol driver file
  */

#include "onewire.h"
/*#################################################ADVANCED SEARCH API's#########################################*/
uint8_t OW_Verify(searchstate_Typedef* ss)
{
	uint8_t backup[11];    //0-7(ROM_ID), 8(LastDiscrepancy), 9(LastDeviceFlag), 10(LastFamilyDiscrepancy)
	uint8_t i=0, ret;
    
	while(i<8){
		backup[i] = ss->ROM[i];
		i++;
	}
	backup[i+1] = ss->LastDiscrepancy;
	backup[i+2] = ss->LastDeviceFlag;
	backup[i+3] = ss->LastFamilyDiscrepancy;

	ss->LastDiscrepancy = 64;
	ss->LastDeviceFlag = 0;

	if(OW_Search(dq, ss, ROM_SEARCH)){
		ret = 1;
		for(i=0; i<8; i++){
			if(backup[8] != ss->ROM[i]){
				ret = 1;
				break;
			}
		}
	}

	else{
		ret = 0;
	}

	for(i=0; i<8; i++){
		ss->ROM[i] = backup[i];
	}
	
    ss->LastDiscrepancy = backup[i+1];
	ss->LastDiscrepancy = backup[i+2];
    ss->LastFamilyDisrepancy = backup[i+3];

	return ret;
}

void OW_TargetSetup(searchstate_Typedef* ss, uint8_t family_code)
{
	uint8_t i = 0;
	
	ss->ROM[0] = family_code;
	while(i<8){
		ss->ROM[i] = 0;
		i++;
	}

	ss->LastDiscrepancy = 64;
	ss->LastDeviceFlag = 0;
	ss->LastFamilyDiscrepancy = 0;
}

void OW_FamilySkipSetup(searchstate_Typedef* ss)
{
	ss->LastDiscrepancy = ss->LastFamilyDiscrepancy;
	ss->LastFamilyDiscrepancy = 0;

	if(ss->LastDiscrepancy == 0){
		ss->LastDeviceFlag = 1;
	}
}

/*##################################################################################*/

/**brief:  Microsecond delay in blocking mode
  *param:  Time in us. Min_val = 1us
  *retval: None
  */
void Micro_Delay(uint16_t delay)
{
	tim_ins->CNT = 0;                 //Set Timer to zero
	while(tim->ins->CNT <= delay){}   //Wait in blocking mode until count reaches delay value
}

/**brief:  DataPin input mode
  *param:  datapin_Typedef instance
  *retval: None
  */
void Mode_Input(datapin_Typedef* dq)
{
	GPIO_InitStruct obj;

	obj.Pin   = dq->GPIO_Pin;
	obj.Mode  = GPIO_MODE_INPUT;
	obj.Pull  = GPIO_NOPULL;
	obj.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(dq->GPIOx, &obj);
}

/**brief:  DataPin output mode
  *param:  datapin_Typedef instance
  *retval: None
  */
void Mode_Output(datapin_Typedef* dq)
{
	GPIO_InitStruct obj;

	obj.Pin   = dq->GPIO_Pin;
	obj.Mode  = GPIO_MODE_OUTPUT_OD;
	obj.Pull  = GPIO_NOPULL;
	obj.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(dq->GPIOx, &obj);
}

/**brief:  Initialise OneWire with GPIO
  *param:  datapin_Typedef instance, GPIO port, GPIO pin
  *retval: None
  */
void OW_Init(datapin_Typedef* dq, GPIO_Typedef* GPIOx, uint16_t GPIO_Pin, )
{
	dq->GPIOx    = GPIOx;
	dq->GPIO_Pin = GPIO_Pin;
}

/**brief:  OneWire reset
  *param:  datapin_Typedef instance
  *retval: 0->SUCCESS, 1->ERROR
  */
uint8_t OW_Reset(datapin_Typedef* dq)
{
	Mode_Output(dq);
	HAL_GPIO_WritePin(dq->GPIOx, dq->GPIO_Pin, GPIO_PIN_RESET);
	Micro_Delay(480);

    Mode_Input(dq);	
	Micro_Delay(60);

	uint8_t ret = HAL_ReadPin(dq->GPIOx, dq->GPIO_Pin);
    MicroDelay(420);

	return ret;
}

/**brief:  OneWire Write Byte
  *param:  datapin_Typedef instance, 1-Byte data
  *retval: None 
  */
void WriteByte(datapin_Typedef* dq, uint8_t data)
{
	uint8_t i = 8;
	uint8_t data_msk = 0x01;

	while(i--){
		OW_WriteBit(dq, (data & data_msk));
		data_msk << 1;
	}
}

/**brief:  OneWire Write Bit
  *param:  datapin_Typedef instance, Bit val
  *retval: None 
  */
void WriteBit(datapin_Typdef* dq, uint8_t bit)
{
	if(bit){
		Mode_Output(dq);
		HAL_GPIO_WritePin(dq->GPIOx, dq->GPIO_Pin, GPIO_PIN_RESET);
		Micro_Delay(10);

		Mode_Input(dq);
		Micro_Delay(5);
	}

	else{
		Mode_Output(dq);
		HAL_GPIO_WritePin(dq->GPIOx, dq->GPIO_Pin, GPIO_PIN_RESET);
		Micro_Delay(65);

		Mode_Input(dq);
		Micro_Delay(5);
	}
}

/**brief:  OneWire Read Byte
  *param:  datapin_Typedef instance 
  *retval: 1-Byte Data 
  */
uint8_t ReadByte(datapin_Typedef* dq)
{
	uint8_t i = 8;
	uint8_t data;
	uint8_t data_msk = 0x01;

	while(i--){
		data |= (data_msk & OW_ReadBit(dq));
		data_msk <<= 1;
	}

	return data;
}

/**brief:  OneWire Read Bit
  *param:  datapin_Typedef instance 
  *retval: Bit val 
  */
uint8_t ReadBit(datapin_Typedef* dq)
{
	uint8_t bit = 0;

	Mode_Output(dq);
	HAL_GPIO_WritePin(dq->GPIOx, dq->GPIO_Pin, GPIO_PIN_RESET);
	Micro_Delay(2);

	if(HAL_GPIO_ReadPin(dq->GPIOx, dq->GPIO_Pin)){
		bit = 1;
	}

	return bit;
}

/**brief:  
  *param:  searchstate_Typedef instance
  *retval: 
  */
uint8_t OW_FirstDev(searchstate_Typedef* ss)
{
	OW_ResetSearch(ss);
	return OW_Search(ss, ROM_SEARCH);
}

/**brief:  
  *param:  searchstate_Typedef instance
  *retval: 
  */
uint8_t OW_NextDev(searchstate_Typedef* ss)
{
	return OW_Search(ss, ROM_SEARCH)
}

/**brief:  
  *param:  searchstate_Typedef instance
  *retval: 
  */
void OW_ResetSearch(searchstate_Typedef* ss)
{
	ss->LastDiscrepancy = 0;
	ss->LastDeviceFlag = 0;
	ss->LastFamilyDiscrepancy = 0;
}

/**brief:  Searches the bus and returns ROM ID  
  *param:  datapin_Typedef instance, searchstate_Typedef instance
  *retval: 1->SUCCESS, 0->FAIL 
  */
uint8_t OW_Search(datapin_Typedef* dq, searchstate_Typedef* ss)
{
	uint8_t id_bit_number;
	uint8_t last_zero, rom_byte_number, search_result;
	uint8_t id_bit, cmp_id_bit;
	uint8_t rom_byte_mask, search_direction;

	id_bit_number = 1;
	last_zero = 0;
	rom_byte_number = 0;
	rom_byte_mask = 1;
	search_result = 0;

	if (!(ss->LastDeviceFlag)){
		
		if (OneWire_Reset(OneWireStruct)){
			ss->LastDiscrepancy = 0;
			ss->LastDeviceFlag = 0;
			ss->LastFamilyDiscrepancy = 0;
			return 0;
		}

		OW_WriteByte(dq, command);

		do {
			id_bit = OW_ReadBit(dq);
			cmp_id_bit = OW_ReadBit(dq);

			if ((id_bit == 1) && (cmp_id_bit == 1)) {
				break;
			} 

			else {
				if (id_bit != cmp_id_bit) {
					search_direction = id_bit;  
				} 

				else {
					if (id_bit_number < ss->LastDiscrepancy) {
						search_direction = ((ss->ROM[rom_byte_number] & rom_byte_mask) > 0);
					} 

					else {
						search_direction = (id_bit_number == ss->LastDiscrepancy);
					}

					if (search_direction == 0) {

						last_zero = id_bit_number;

						if (last_zero < 9) {
							ss->LastFamilyDiscrepancy = last_zero;
						}
					}
				}
				
				if (search_direction == 1) {
					ss->ROM[rom_byte_number] |= rom_byte_mask;
				} 

				else {
					ss->ROM[rom_byte_number] &= ~rom_byte_mask;
				}

				OW_WriteBit(dq, search_direction);
				id_bit_number++;
				rom_byte_mask <<= 1;

				if (rom_byte_mask == 0) {
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		} while (rom_byte_number < 8);  

		if (!(id_bit_number < 65)) {
			ss->LastDiscrepancy = last_zero;
			if (ss->LastDiscrepancy == 0) {
				ss->LastDeviceFlag = 1;
			}
			search_result = 1;
		}
	}

	if (!search_result || !OneWireStruct->ROM_NO[0]) {
		ss->LastDiscrepancy = 0;
		ss->LastDeviceFlag = 0;
		ss->LastFamilyDiscrepancy = 0;
		search_result = 0;
	}

	return search_result;
}

/**brief:  Select the Dev with given ROM
  *param:  datapin_Typedef instance, searchstate_Typedef instance
  *retval: None
  */
void OW_SelectROM(datapin_Typedef* dq, searchstate_Typedef* ss)
{
	uint8_t i = 0;

	OW_WriteByte(dq, ROM_MATCH);
	
	while(i<8){
		OW_WriteByte(dq, ss->ROM[i]);
		i++;
	}
}

/**brief:  CRC Check 
  *param:  Pointer to data and length 
  *retval: 1-> CRC match success, 0-> CRC match fail
  */
uint8_t CRC8_CHK(uint8_t* addr, uint8_t len)
{
	uint8_t crc = 0, inbyte, i, mix;

	while (len--) {
		inbyte = *addr++;
		for (i = 8; i; i--) {
			mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) {
				crc ^= 0x8C;
			}
			inbyte >>= 1;
		}
	}
	return crc;
}

