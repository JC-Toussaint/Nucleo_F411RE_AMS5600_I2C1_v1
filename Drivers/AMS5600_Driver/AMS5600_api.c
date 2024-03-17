// datasheet: https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf

#include "debug.h"
#include "platform.h"
#include "AMS5600_api.h"

/*******************************************************
  AMS5600_setOutPut
  In: 0 for digital PWM
      1 for analog (full range 0-100% of GND to VDD)
      2 for analog (reduced range 10-90%)
  Out: none
  Description: sets output mode in CONF register.
*******************************************************/
uint8_t AMS5600_setOutPut(uint8_t mode)
{
  uint16_t _conf_lo = _addr_conf+1; // lower byte address
  uint8_t config_status;
  uint8_t status = AMS5600_RdByte(_ams5600_Address, _conf_lo, &config_status);
  config_status &= 0b11001111; // bits 5:4 = 00, default
  if (mode == 0) {
    config_status |= 0b100000; // bits 5:4 = 10
  } else if (mode == 2) {
    config_status |= 0b010000; // bits 5:4 = 01
  }
  status |= AMS5600_WrByte(_ams5600_Address, _conf_lo, config_status);
  return status;
}

/****************************************************
  AMS5600_getAddress
  In: none
  Out: i2c address of AMS 5600
  Description: returns i2c address of AMS 5600
****************************************************/
uint8_t AMS5600_getAddress()
{
  return _ams5600_Address;
}

/*******************************************************
  AMS5600_setMaxAngle
  In: new maximum angle to set OR none
  Out: value of max angle register
  Description: sets a value in maximum angle register.
  If no value is provided, method will read position of
  magnet.  Setting this register zeros out max position
  register.
*******************************************************/
uint8_t AMS5600_setMaxAngle(uint16_t newMaxAngle, uint16_t *max_angle_register)
{
  uint16_t maxAngle;
  uint8_t status=0;
  if (newMaxAngle == -1)
	status |= AMS5600_getRawAngle(&maxAngle);
  else
    maxAngle = newMaxAngle;

  status |= AMS5600_WrWord(_ams5600_Address, _addr_mang, maxAngle);
  status |= AMS5600_RdWord(_ams5600_Address, _addr_mang, max_angle_register);
  return status;
}

/*******************************************************
  AMS5600_getMaxAngle
  In: none
  Out: value of max angle register
  Description: gets value of maximum angle register.
*******************************************************/
uint8_t AMS5600_getMaxAngle(uint16_t *max_angle_register)
{
  uint8_t status = AMS5600_RdWord(_ams5600_Address, _addr_mang, max_angle_register);
  return status;
}

/*******************************************************
  AMS5600_setStartPosition
  In: new start angle position
  Out: value of start position register
  Description: sets a value in start position register.
  If no value is provided, method will read position of
  magnet.  
*******************************************************/
uint8_t AMS5600_setStartPosition(uint16_t startAngle, uint16_t *zPosition)
{
  uint16_t rawStartAngle;
  uint8_t status=0;
  if (startAngle == -1)
     status = AMS5600_getRawAngle(&rawStartAngle);
  else
    rawStartAngle = startAngle;

  status |= AMS5600_WrWord(_ams5600_Address, _addr_zpos, rawStartAngle);
  status |= AMS5600_RdWord(_ams5600_Address, _addr_zpos, zPosition);
  return status;
}

/*******************************************************
  AMS5600_getStartPosition
  In: none
  Out: value of start position register
  Description: gets value of start position register.
*******************************************************/
uint8_t AMS5600_getStartPosition(uint16_t *start_position_register)
{
  uint8_t status = AMS5600_RdWord(_ams5600_Address, _addr_zpos, start_position_register);
  return status;
}

/*******************************************************
  AMS5600_setEndPosition
  In: new end angle position
  Out: value of end position register
  Description: sets a value in end position register.
*******************************************************/
uint8_t AMS5600_setEndPosition(uint16_t rawEndAngle, uint16_t *mPosition)
{
  uint8_t status=0;
  status |= AMS5600_WrWord(_ams5600_Address, _addr_mpos, rawEndAngle);
  status |= AMS5600_RdWord(_ams5600_Address, _addr_mpos, mPosition);
  return status;
}

/*******************************************************
  AMS5600_getEndPosition
  In: none
  Out: value of end position register
  Description: gets value of end position register.
*******************************************************/
uint8_t AMS5600_getEndPosition(uint16_t *end_position_register)
{
  uint8_t status = AMS5600_RdWord(_ams5600_Address, _addr_mpos, end_position_register);
  return status;
}

/*******************************************************
  AMS5600_getRawAngle
  In: none
  Out: value of raw angle register
  Description: gets raw value of magnet position.
  start, end, and max angle settings do not apply
*******************************************************/
uint8_t AMS5600_getRawAngle(uint16_t *rawAngle)
{
  uint8_t status = AMS5600_RdWord(_ams5600_Address, _addr_raw_angle, rawAngle);
  return status;
}

/*******************************************************
  AMS5600_getScaledAngle
  In: none
  Out: value of scaled angle register
  Description: gets scaled value of magnet position.
  start, end, or max angle settings are used to 
  determine value
*******************************************************/
uint8_t AMS5600_getScaledAngle(uint16_t *scaledAngle)
{
  uint8_t status = AMS5600_RdWord(_ams5600_Address, _addr_angle, scaledAngle);
  return status;
}

/*******************************************************
  AMS5600_detectMagnet
  In: none
  Out: 1 if magnet is detected, 0 if not
  Description: reads status register and examines the 
  MD bit.
*******************************************************/
uint8_t AMS5600_detectMagnet(uint8_t *magStatus)
{
  // Status bits: 0 0 MD ML MH 0 0 0 
  // MD high = magnet detected  
  uint8_t status = AMS5600_RdByte(_ams5600_Address, _addr_status, magStatus);
  *magStatus = (*magStatus & 0x20) ? 1 : 0;
  return status;
}

/*******************************************************
  AMS5600_getMagnetStrength
  In: none
  Out: 0 if magnet not detected
       1 if magnet is too weak
       2 if magnet is just right
       3 if magnet is too strong
  Description: reads status register and examines the 
  MH,ML,MD bits.
*******************************************************/
uint8_t AMS5600_getMagnetStrength(uint8_t *magnetStrength)
{
  *magnetStrength = 0; // no magnet
  // Status bits: 0 0 MD ML MH 0 0 0 
  // MD high = magnet detected  
  // ML high = AGC maximum overflow, magnet too weak
  // MH high = AGC minimum overflow, magnet too strong
  uint8_t magStatus;
  uint8_t status = AMS5600_RdByte(_ams5600_Address, _addr_status, &magStatus);
  if (magStatus & 0x20) {
    *magnetStrength = 2;   // magnet detected
    if (magStatus & 0x10)
      *magnetStrength = 1; // too weak
    else if (magStatus & 0x08)
      *magnetStrength = 3; // too strong
  }
  
  return status;
}

/*******************************************************
  AMS5600_getAgc
  In: none
  Out: value of AGC register
  Description: gets value of AGC register.
*******************************************************/
uint8_t AMS5600_getAgc(uint8_t *AGC_register)
{
  uint8_t status = AMS5600_RdByte(_ams5600_Address, _addr_agc, AGC_register);
  return status;
}

/*******************************************************
  AMS5600_getMagnitude
  In: none
  Out: value of magnitude register
  Description: gets value of magnitude register.
*******************************************************/
uint8_t AMS5600_getMagnitude(uint16_t *magnitude_register)
{
  uint8_t status = AMS5600_RdWord(_ams5600_Address, _addr_magnitude, magnitude_register);
  return status;
}

/*******************************************************
  AMS5600_getConf
  In: none
  Out: value of CONF register 
  Description: gets value of CONF register.
*******************************************************/
uint8_t AMS5600_getConf(uint16_t *conf_register)
{
  uint8_t status = AMS5600_RdWord(_ams5600_Address, _addr_conf, conf_register);
  return status;
}

/*******************************************************
  AMS5600_setConf
  In: value of CONF register
  Out: none
  Description: sets value of CONF register.
*******************************************************/
uint8_t AMS5600_setConf(uint16_t _conf)
{
  uint8_t status = AMS5600_WrWord(_ams5600_Address, _addr_conf, _conf);
  return status;
}

/*******************************************************
  AMS5600_getBurnCount
  In: none
  Out: value of zmco register
  Description: determines how many times chip has been
  permanently written to. 
*******************************************************/
uint8_t AMS5600_getBurnCount(uint8_t *zmco_register)
{
  uint8_t status = AMS5600_RdByte(_ams5600_Address, _addr_zmco, zmco_register);
  return status;
}

/*******************************************************
  AMS5600_burnAngle
  In: none
  Out: 1 success
      -1 no magnet
      -2 burn limit exceeded
      -3 start and end positions not set (useless burn)
  Description: burns start and end positions to chip.
  THIS CAN ONLY BE DONE 3 TIMES
*******************************************************/
uint8_t AMS5600_burnAngle(int *retVal)
{
  uint16_t _zPosition, _mPosition, _maxAngle;
  uint8_t status = AMS5600_getStartPosition(& _zPosition);
  status |= AMS5600_getEndPosition(& _mPosition);
  status |=AMS5600_getMaxAngle(& _maxAngle);

  *retVal = 1;
  uint8_t magStatus;
  status |= AMS5600_detectMagnet(&magStatus);
  if (magStatus == 1) {
    uint8_t zmco_register;
    status |= AMS5600_getBurnCount(&zmco_register);
    if (zmco_register < 3) {
      if ((_zPosition == 0) && (_mPosition == 0))
        *retVal = -3;
      else{
        PRINT_MESG_DBG("burn angle function desactivated\n"); 
        //status |= AMS5600_WrByte(_ams5600_Address, _addr_burn, AMS5600_BURN_ANGLE);
      }
    }
    else
      *retVal = -2;
  } else
    *retVal = -1;

  return status;
}

/*******************************************************
  AMS5600_burnMaxAngleAndConfig
  In: none
  Out: 1 success
      -1 burn limit exceeded
      -2 max angle is to small, must be at or above 18 degrees
  Description: burns max angle and config data to chip.
  THIS CAN ONLY BE DONE 1 TIME
*******************************************************/
uint8_t AMS5600_burnMaxAngleAndConfig(int *retVal)
{
  uint16_t _maxAngle;
  uint8_t status =AMS5600_getMaxAngle(& _maxAngle);
  uint8_t zmco_register;

  *retVal = 1;
  status |= AMS5600_getBurnCount(&zmco_register);
  if ( zmco_register == 0) {
    if (_maxAngle * 0.087 < 18)
      *retVal = -2;
    else{
      PRINT_MESG_DBG("burn angle function desactivated\n");
      //status |= AMS5600_WrByte(_ams5600_Address, _addr_burn, AMS5600_BURN_SETTING);
    }
  }
  else
    *retVal = -1;

  return status;
}


