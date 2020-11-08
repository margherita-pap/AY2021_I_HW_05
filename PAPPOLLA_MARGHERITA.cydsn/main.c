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
#define LIS3DH_CTRL_REG4_BDU_ACTIVE 0x88

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    I2C_Peripheral_Start();
    UART_Start();
    EEPROM_Start();
    isr_PressedButton_StartEx(Custom_button_pressed_isr);
    
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
        if (error == NO_ERROR)
    {
        sprintf(message, "CTRL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
        UART_PutString(message); 
    }
    else
    {
        UART_PutString("Error occurred during I2C comm to read CTRL REG 4\r\n");   
    }
        if(ctrl_reg4!=LIS3DH_CTRL_REG4_BDU_ACTIVE){
            ctrl_reg4=LIS3DH_CTRL_REG4_BDU_ACTIVE;
            error=I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                LIS3DH_CTRL_REG4,
                                                ctrl_reg4);
            if (error == NO_ERROR)
            {
                sprintf(message, "CTRL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
                UART_PutString(message); 
            }
            else
            {
                UART_PutString("Error occurred during I2C comm to write CTRL REG 4\r\n");   
            }
        }
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    uint8_t bits_ODR=0;
    uint8_t new_bits_ODR=0;
    uint8_t new_ctrl_reg1=0;    
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
                UART_PutString("\r\nWriting new values..\r\n");
    
                error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                     LIS3DH_CTRL_REG1,
                                                     new_ctrl_reg1);
    
                if (error == NO_ERROR)
                {
                        sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", new_ctrl_reg1);
                        UART_PutString(message);
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
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
