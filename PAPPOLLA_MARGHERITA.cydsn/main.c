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
#include <stdio.h>
#include "ErrorCodes.h"
#include "I2C_Interface.h"
#include "EEPROMCodes.h"
#include "InterruptRoutines.h"

/**
*   \brief 7-bit I2C address of the slave device.
*/
#define LIS3DH_DEVICE_ADDRESS 0x18

/**
*   \brief Address of the WHO AM I register
*/
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F

/**
*   \brief Address of the Status register
*/
#define LIS3DH_STATUS_REG 0x27

/**
*   \brief Address of the Control register 1
*/
#define LIS3DH_CTRL_REG1 0x20
/**
*   \brief Address of the Control register 4
*/
#define LIS3DH_CTRL_REG4 0x23
/**
*   \0x88 in binary:1000 1000, macro for setting ctrl_reg4 
*   \BDU bit to 1, output register are not updated until MSB and LSB reading
*   \FS selection 00, so ±2g
*   \ HR selection 1, so high-resolution enabled
*/
#define LIS3DH_CTRL_REG4_BDU_HR_ACTIVE 0x88
/**
*   \Macro for status register, if ZYXDA bit is 1
    \X, Y and Z-axis new data available
*/
#define STATUS_REG_ZYXDA_HIGH 0x08
/**
*   \brief Address of the OUT_X_L 
*/
#define OUT_X_L 0x28

void ConvertAcceleration(uint8_t* DataToSend, uint8_t* DataRead);
void SystemStart(void);
   

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
   
    SystemStart();    
    CyDelay(5); //"The boot procedure is complete about 5 milliseconds after device power-up."
    
    // String to print out messages on the UART
    char message[50];

    // Check which devices are present on the I2C bus
    for (int i = 0 ; i < 128; i++)
    {
        if (I2C_Peripheral_IsDeviceConnected(i))
        {
            // print out the address is hex format
            sprintf(message, "Device 0x%02X is connected\r\n", i);
            UART_PutString(message); 
        }
        
    }
    /******************************************/
    /*            I2C Reading                 */
    /******************************************/
    
    /* Read WHO AM I REGISTER register */
    uint8_t who_am_i_reg;
    ErrorCode error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                  LIS3DH_WHO_AM_I_REG_ADDR, 
                                                  &who_am_i_reg);
    if (error == NO_ERROR)
    {
        sprintf(message, "WHO AM I REG: 0x%02X [Expected: 0x33]\r\n", who_am_i_reg);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm\r\n");   
    }
    
    /*      I2C Reading Status Register       */
    
    uint8_t status_register; 
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "STATUS REGISTER: 0x%02X\r\n", status_register);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read status register\r\n");   
    }
    
    /******************************************/
    /* Set Control register to EEPROM value   */
    /******************************************/
    uint8_t ctrl_reg1; 
    ctrl_reg1=EEPROM_Startup();
    
    /******************************************/
    /*            I2C Writing                 */
    /******************************************/
    
    UART_PutString("\r\nWriting new values..\r\n");
    
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG1,
                                             ctrl_reg1);
    
    if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", ctrl_reg1);
            UART_PutString(message); 
        }
    else
        {
            UART_PutString("Error occurred during I2C comm to set control register 1\r\n");   
        }
        // setto il ctrl reg 4
        uint8_t ctrl_reg4;
        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
        if (error == NO_ERROR){
            if(ctrl_reg4!=LIS3DH_CTRL_REG4_BDU_HR_ACTIVE){
                ctrl_reg4=LIS3DH_CTRL_REG4_BDU_HR_ACTIVE;
                error=I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                LIS3DH_CTRL_REG4,
                                                ctrl_reg4);
                if (error == NO_ERROR){
                    sprintf(message, "CTRL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
                    UART_PutString(message); 
                }
                else{
        
                    UART_PutString("Error occurred during I2C comm to write CTRL REG 4\r\n");   
                }
        }
            
        }
        else
        {
            UART_PutString("Error occurred during I2C comm to read CTRL REG 4\r\n");   
        }
        
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
        
    uint8_t bits_ODR=0;
    uint8_t new_bits_ODR=0;
    uint8_t new_ctrl_reg1=0; 
    uint8_t OutArray[8];
    uint8_t DataAcceleration[6];
    OutArray[0]=0xA0;
    OutArray[7]=0xC0;
        
    for(;;)
    {
        if(flag_button_pressed){
            
            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
            if (error == NO_ERROR)
            {   bits_ODR= ctrl_reg1>>4;
                if(bits_ODR==6){
                    new_bits_ODR=1;
                    new_ctrl_reg1=(new_bits_ODR<<4)|(ctrl_reg1 & 0x0F);
                }
                else{
                    new_bits_ODR=bits_ODR+1;
                    new_ctrl_reg1=(new_bits_ODR<<4)|(ctrl_reg1 & 0x0F);
                }
                EEPROM_Startup_Update(new_ctrl_reg1);
                //UART_PutString("\r\nWriting new values..\r\n");
    
                error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                     LIS3DH_CTRL_REG1,
                                                     new_ctrl_reg1);
    
                if (error == NO_ERROR)
                {
                        //sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", new_ctrl_reg1);
                        //UART_PutString(message);
                        flag_button_pressed=0;
                }
                else
                {
                    UART_PutString("Error occurred during I2C comm to set control register 1\r\n");   
                }
            }
            else
            {
                UART_PutString("Error occurred during I2C comm to read control register 1\r\n");   
            }    
         
    }
       
     
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                            LIS3DH_STATUS_REG,
                                            &status_register);
    
        if (error == NO_ERROR){
            if(status_register & STATUS_REG_ZYXDA_HIGH ){
                error= I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                        OUT_X_L,
                                                        6, DataAcceleration);
                if(error==NO_ERROR){
                    ConvertAcceleration(&OutArray[1],&DataAcceleration[0]);
                    ConvertAcceleration(&OutArray[3],&DataAcceleration[2]);
                    ConvertAcceleration(&OutArray[5],&DataAcceleration[4]);
                    UART_PutArray(OutArray,8);
                    
                }
                else{
                    UART_PutString("qualcosa non funziona");
                }
        
    }
        }
    else
    {
        UART_PutString("Error occurred during I2C comm to read status register\r\n");   
    }
    
}
    }
   
 void ConvertAcceleration(uint8_t* DataToSend, uint8_t* DataRead)
{
    int16 out_acceleration;
    out_acceleration= (int16) (*DataRead |(*(DataRead+1)<<8))>>4 ;
    float sensitivity= 9.8/1000;
    int16 trick=1000;
    float out_acceleration_m_s2= out_acceleration*sensitivity;
    out_acceleration= out_acceleration_m_s2*trick;
    *(DataToSend)=(uint8_t) (out_acceleration & 0xFF);
    *(DataToSend+1)= (uint8_t) (out_acceleration >>8);
}
            

 void SystemStart(void){
    I2C_Peripheral_Start();
    UART_Start();
    EEPROM_Start();
    isr_PressedButton_StartEx(Custom_button_pressed_isr);
}
    
        
        
/* [] END OF FILE */
