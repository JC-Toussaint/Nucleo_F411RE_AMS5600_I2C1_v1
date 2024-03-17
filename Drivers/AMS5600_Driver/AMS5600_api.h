// datasheet: https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf

#ifndef AMS_5600_h
#define AMS_5600_h

#define AMS5600_BURN_ANGLE     0x80       /**< angle */
#define AMS5600_BURN_SETTING   0x40       /**< setting */

/*******************************************************
  AMS5600_setOutPut
  In: 0 for digital PWM
      1 for analog (full range 0-100% of GND to VDD)
      2 for analog (reduced range 10-90%)
  Out: none
  Description: sets output mode in CONF register.
*******************************************************/
uint8_t AMS5600_setOutPut(uint8_t mode);

/****************************************************
  AMS5600_getAddress
  In: none
  Out: i2c address of AMS 5600
  Description: returns i2c address of AMS 5600
****************************************************/
uint8_t AMS5600_getAddress();

/*******************************************************
  AMS5600_setMaxAngle
  In: new maximum angle to set OR none
  Out: value of max angle register
  Description: sets a value in maximum angle register.
  If no value is provided, method will read position of
  magnet.  Setting this register zeros out max position
  register.
*******************************************************/
uint8_t AMS5600_setMaxAngle(uint16_t newMaxAngle, uint16_t *max_angle_register);

/*******************************************************
  AMS5600_getMaxAngle
  In: none
  Out: value of max angle register
  Description: gets value of maximum angle register.
*******************************************************/
uint8_t AMS5600_getMaxAngle(uint16_t *max_angle_register);

/*******************************************************
  AMS5600_setStartPosition
  In: new start angle position
  Out: value of start position register
  Description: sets a value in start position register.
  If no value is provided, method will read position of
  magnet.  
*******************************************************/
uint8_t AMS5600_setStartPosition(uint16_t startAngle, uint16_t *zPosition);

/*******************************************************
  AMS5600_getStartPosition
  In: none
  Out: value of start position register
  Description: gets value of start position register.
*******************************************************/
uint8_t AMS5600_getStartPosition(uint16_t *start_position_register);

/*******************************************************
  AMS5600_setEndPosition
  In: new end angle position
  Out: value of end position register
  Description: sets a value in end position register.
  If no value is provided, method will read position of
  magnet.  
*******************************************************/
uint8_t AMS5600_setEndPosition(uint16_t endAngle, uint16_t *mPosition);

/*******************************************************
  AMS5600_getEndPosition
  In: none
  Out: value of end position register
  Description: gets value of end position register.
*******************************************************/
uint8_t AMS5600_getEndPosition(uint16_t *end_position_register);

/*******************************************************
  AMS5600_getRawAngle
  In: none
  Out: value of raw angle register
  Description: gets raw value of magnet position.
  start, end, and max angle settings do not apply
*******************************************************/
uint8_t AMS5600_getRawAngle(uint16_t *rawAngle);

/*******************************************************
  AMS5600_getScaledAngle
  In: none
  Out: value of scaled angle register
  Description: gets scaled value of magnet position.
  start, end, or max angle settings are used to 
  determine value
*******************************************************/
uint8_t AMS5600_getScaledAngle(uint16_t *scaledAngle);

/*******************************************************
  AMS5600_detectMagnet
  In: none
  Out: 1 if magnet is detected, 0 if not
  Description: reads status register and examines the 
  MD bit.
*******************************************************/
uint8_t AMS5600_detectMagnet(uint8_t *magStatus);

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
uint8_t AMS5600_getMagnetStrength(uint8_t *magnetStrength);

/*******************************************************
  AMS5600_getAgc
  In: none
  Out: value of AGC register
  Description: gets value of AGC register.
*******************************************************/
uint8_t AMS5600_getAgc(uint8_t *AGC_register);

/*******************************************************
  AMS5600_getMagnitude
  In: none
  Out: value of magnitude register
  Description: gets value of magnitude register.
*******************************************************/
uint8_t AMS5600_getMagnitude(uint16_t *magnitude_register);

/*******************************************************
  AMS5600_getConf
  In: none
  Out: value of CONF register 
  Description: gets value of CONF register.
*******************************************************/
uint8_t AMS5600_getConf(uint16_t *conf_register);

/*******************************************************
  AMS5600_setConf
  In: value of CONF register
  Out: none
  Description: sets value of CONF register.
*******************************************************/
uint8_t AMS5600_setConf(uint16_t _conf);

/*******************************************************
  AMS5600_getBurnCount
  In: none
  Out: value of zmco register
  Description: determines how many times chip has been
  permanently written to. 
*******************************************************/
uint8_t AMS5600_getBurnCount(uint8_t *zmco_register);

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
uint8_t AMS5600_burnAngle(int *retVal);

/*******************************************************
  AMS5600_burnMaxAngleAndConfig
  In: none
  Out: 1 success
      -1 burn limit exceeded
      -2 max angle is to small, must be at or above 18 degrees
  Description: burns max angle and config data to chip.
  THIS CAN ONLY BE DONE 1 TIME
*******************************************************/
uint8_t AMS5600_burnMaxAngleAndConfig(int *retVal);

// i2c address
static const uint8_t _ams5600_Address = (0x36 << 1);

// single byte registers
static const uint8_t _addr_status = 0x0b; // magnet status
static const uint8_t _addr_agc    = 0x1a; // automatic gain control
static const uint8_t _addr_burn   = 0xff; // permanent burning of configs (zpos, mpos, mang, conf)
static const uint8_t _addr_zmco   = 0x00; // number of times zpos/mpos has been permanently burned
                                          // zpos/mpos can be permanently burned 3x
                                          // mang/conf can be burned only once

// double byte registers, specify starting address (lower addr, but higher byte data)
// addr   = upper byte of data (MSB), only bits 0:3 are used
// addr+1 = lower byte of data (LSB)
static const uint8_t _addr_zpos      = 0x01; // zero position (start)
                                             // 0x02 - lower byte
static const uint8_t _addr_mpos      = 0x03; // maximum position (stop)
                                             // 0x04 - lower byte
static const uint8_t _addr_mang      = 0x05; // maximum angle
                                             // 0x06 - lower byte
static const uint8_t _addr_conf      = 0x07; // configuration
                                             // 0x08 - lower byte
static const uint8_t _addr_raw_angle = 0x0c; // raw angle
                                             // 0x0d - lower byte
static const uint8_t _addr_angle     = 0x0e; // mapped angle
                                             // 0x0f - lower byte
static const uint8_t _addr_magnitude = 0x1b; // magnitude of internal CORDIC
                                             // 0x1c - lower byte

#endif
