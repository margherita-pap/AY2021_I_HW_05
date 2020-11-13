/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
/* Accelerometer 8-bit registers addresses */
#define LIS3DH_DEVICE_ADDRESS 0x18       //brief 7-bit I2C address of the slave device.
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F    //brief Address of the WHO AM I register
#define LIS3DH_STATUS_REG 0x27           //brief Address of the Status register
#define LIS3DH_CTRL_REG1 0x20            //brief Address of the Control register 1
#define LIS3DH_CTRL_REG4 0x23            //brief Address of the Control register 4
/**
*   \Macro for setting ctrl_reg4 
*   \0x88 in binary:1000 1000
*   \BDU bit to 1, output register are not updated until MSB and LSB reading
*   \FS selection 00, so ±2g
*   \HR selection 1, so high-resolution enabled
*/
#define LIS3DH_CTRL_REG4_BDU_HR_ACTIVE 0x88
/**
*   \Macro for checking status register bit ZYXDA. 
    \0x08 in binary 0000 1000
*   \When ZYXDA bit is 1 means that X, Y and Z-axis new data are available. 
*   \Chapter 8.15 Datasheet 
*/
#define STATUS_REG_ZYXDA_HIGH 0x08

/*8-bit output registers addresses*/
//High resolution 12 bit data output. The value is expressed as two’s complement left-justified.
//This means all value bits are shifted to the left
//X-axis acceleration data
#define LIS3DH_OUT_X_L 0x28    //This register will contains XXXX 0000 LSbs of convertion
#define LIS3DH_OUT_X_H 0x29    // This register will contains XXXX XXXX MSb of convertion
//Y-axis acceleration data
#define LIS3DH_OUT_Y_L 0x2A    //This register will contains XXXX 0000 LSbs of convertion
#define LIS3DH_OUT_Y_H 0x2B    // This register will contains XXXX XXXX MSb of convertion
//Z-axis acceleration data
#define LIS3DH_OUT_Z_L 0x2C    //This register will contains XXXX 0000 LSbs of convertion
#define LIS3DH_OUT_Z_H 0x2D    // This register will contains XXXX XXXX MSb of convertion
#define OUTPUT_REGISTER_NUMBER 6

#define DATA_TO_TRANSFER 1+OUTPUT_REGISTER_NUMBER+1

#define HR_POWERMODE_1Hz   1
#define HR_POWERMODE_200Hz 6

void ConvertAcceleration(uint8_t* DataToSend, uint8_t* OutputRegisterData);
void SystemStart(void);


/* [] END OF FILE */
