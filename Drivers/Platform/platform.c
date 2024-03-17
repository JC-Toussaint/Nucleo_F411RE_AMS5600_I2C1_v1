/*******************************************************************************
*
* Copyright (c) 2020 STMicroelectronics - All Rights Reserved
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*******************************************************************************/

#include "platform.h"

extern I2C_HandleTypeDef 	hi2c1;

/*
 * beware AMS5600 sensor register addresses are 8-bit only
 */

uint8_t AMS5600_RdByte(uint16_t dev, uint8_t RegisterAddr, uint8_t *value)
{
	uint8_t status = 0;
	uint8_t data_write[1];
	uint8_t data_read[1];

	data_write[0] = RegisterAddr & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, dev, data_write, 1, 100);
	status|= HAL_I2C_Master_Receive(&hi2c1, dev, data_read, 1, 100);
	*value = data_read[0];
	return status;
}

uint8_t AMS5600_RdWord(uint16_t dev, uint8_t RegisterAddr, uint16_t *value)
{
	uint8_t status = 0;
	uint8_t data_write[1];
	uint8_t data_read[2];

	data_write[0] = RegisterAddr & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, dev, data_write, 1, 100);
	status|= HAL_I2C_Master_Receive(&hi2c1, dev, data_read, 2, 100);
	*value = (data_read[0] << 8) | (data_read[1]);
	return status;
}

uint8_t AMS5600_WrByte(uint16_t dev, uint8_t RegisterAddr, uint8_t value)
{
	uint8_t data_write[2];
	uint8_t status = 0;

	data_write[0] = RegisterAddr & 0xFF;
	data_write[1] = value & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, dev, data_write, 2, 100);
	return status;
}

uint8_t AMS5600_WrWord(uint16_t dev, uint8_t RegisterAddr, uint16_t value)
{
	uint8_t data_write[3];
	uint8_t status = 0;
	data_write[0] = RegisterAddr & 0xFF;
	data_write[1] = (value >> 8) & 0xFF;
	data_write[2] = value & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, dev, data_write, 3, 100);
	return status;
}

void WaitMs(uint32_t TimeMs)
{
	HAL_Delay(TimeMs);
}
