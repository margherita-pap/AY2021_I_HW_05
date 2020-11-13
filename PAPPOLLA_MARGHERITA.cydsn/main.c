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
#include "define.h"

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
    
    /* I2C Reading Status Register */
    
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
    
    //Set ctrl_reg1 variable equal to the value saved in EEPROM memory at addres 0x00
    uint8_t ctrl_reg1; 
    ctrl_reg1=EEPROM_Startup();
    
    UART_PutString("\r\nWriting new values..\r\n");
    /*      I2C Writing Control Register       
    *       \Writing on the control register the value saved in the EEPROM memory
    *       \and so setting the value of sampling rate, abilitating the 3-axis of accelerometer and power mode operation.
    */
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
    /*      I2C Setting Control Register 4       
    *       \First I read the value of control register 4
    *       \if this register is not set to high-resolution mode, BDU bit active, FS ±2g
    *       \I write on the ctrl_reg4 the correct value for the operation mode that i want   
    */
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
        else{
            UART_PutString("Error occurred during I2C comm to read CTRL REG 4\r\n");   
        }
    /* Declaration and initialization of variables */    
    uint8_t bits_ODR=0;             //I save the 4 bits that set the sampling frequency
    uint8_t new_bits_ODR=0;         //New value of the sampling frequency
    uint8_t new_ctrl_reg1=0;        //New value of ctrl_reg1 to write on the register of the accelerometer and on EEPROM memory
    uint8_t OutArray[DATA_TO_TRANSFER];                  //Packet to send thanks to UART communication
    uint8_t AccelerationData[OUTPUT_REGISTER_NUMBER];    // In this array i save the values of  the output registers of accelerometer
    OutArray[0]=0xA0;                                    //Header of the packet
    OutArray[DATA_TO_TRANSFER-1]=0xC0;                   // Tail of teh packet
        
    for(;;)
    {   /*I executed this part of the main function only if the button has been pressed*/
        if(flag_button_pressed){
            flag_button_pressed=0;
            //I start by reading the current value of ctrl_reg1 on the accelerometer
            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
            if (error == NO_ERROR){
                
                //In this variable i save the 4 MSbs of ctrl_reg1 related to data rate selection
                //ODR[3:0]
                bits_ODR= ctrl_reg1>>4;
                if(bits_ODR==HR_POWERMODE_200Hz){
                    new_bits_ODR=HR_POWERMODE_1Hz;
                //The new value of ctrl_reg1 will have the 4 LSbs equal to the start-up 
                //configuration because this bits are related to abilitation to Z,Y and X axes and high-resolution mode selection
                //Instead the 4MSbs are changed in a way that if i was sampling at 200Hz i restart from 1 Hz of sampling rate    
                    new_ctrl_reg1=(new_bits_ODR<<4)|(ctrl_reg1 & 0x0F);
                }
                else{
                //In the datasheet, chapter 8.8 are shown the data rate configuration
                // 0 0 0 1  HR / Normal / Low-power mode (1 Hz)
                // 0 0 1 0  HR / Normal / Low-power mode (10 Hz)
                // 0 0 1 1  HR / Normal / Low-power mode (25 Hz)
                // 0 1 0 0  HR / Normal / Low-power mode (50 Hz)
                // 0 1 0 1  HR / Normal / Low-power mode (100 Hz)
                // 0 1 1 0  HR / Normal / Low-power mode (200 Hz)    
                //The user can cycle from 1Hz up to 200Hz simply adding 1 to the previous value   
                    new_bits_ODR=bits_ODR+1;
                    new_ctrl_reg1=(new_bits_ODR<<4)|(ctrl_reg1 & 0x0F);
                }
                //After the evaluation of the new control register i can proceed
                //by saving this value in EEPROM memory
                EEPROM_Startup_Update(new_ctrl_reg1);
                //Writing on the control register the new value frequency
                error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                     LIS3DH_CTRL_REG1,
                                                     new_ctrl_reg1);
                //I leave some debugging messages of occurrance of errors in case of reading or writing the control register
                if (error ==ERROR)
                {                      
                 UART_PutString("Error occurred during I2C comm to set control register 1\r\n");
                 
                }
            }
            else
            {
                UART_PutString("Error occurred during I2C comm to read control register 1\r\n");   
            }    
         
    }
    /*Reading the value of acceleration data from accelerometer */
    //The first step is reading the status register    
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                            LIS3DH_STATUS_REG,
                                            &status_register);
    
        if (error == NO_ERROR){
    //I check if there are new data available using & bit a bit operation
    //If condtion will give a value different from 0 if ZYXDA bit is high in status register.
    //Otherwise the if condition will be false and so i don't read the output registers of accelerometer
    //because there are not new data available. This prevent reading the same value and plotting it many times        
            if(status_register & STATUS_REG_ZYXDA_HIGH ){
    /*If new data are available, i make a multiple data reading and save the values of 8-bit output registers
    * conteining the ADC convertion of the accelerations. The output registers are mapped in a way that their addresses are           
    * consecutive starting from OUT_X_L(28h) up to OUT_Z_H(2Dh). At the end of the multiple reading all the values of
    * output registers are saved inside the array of uint8 AccelerationData and we can proceed by making the convertion
    * to retrive back the physical value of the acceleration. 
    */            
                error= I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                        LIS3DH_OUT_X_L,
                                                        OUTPUT_REGISTER_NUMBER, AccelerationData);
                if(error==NO_ERROR){    
                //AccelerationData[0] contains the value of register OUT_X_L
                //AccelerationData[1] contains the value of register OUT_X_L    
                //OutArray[1] contains the LSB of X-axis acceleration expressed in m/s^2
                //OutArray[2] contains the MSB of X-axis acceleration expressed in m/s^2  
                    ConvertAcceleration(&OutArray[1],&AccelerationData[0]);
                //AccelerationData[2] contains the value of register OUT_Y_L
                //AccelerationData[3] contains the value of register OUT_Y_H    
                //OutArray[3] contains the LSB of Y-acceleration expressed in m/s^2 
                //OutArray[4] contains the MSB of Y-acceleration expressed in m/s^2    
                    ConvertAcceleration(&OutArray[3],&AccelerationData[2]);
                //AccelerationData[4] contais the value of register OUT_Z_L
                //AccelerationData[5] contains the value of register OUT_Z_H    
                //OutArray[5] contains the LSB of Z-acceleration expressed in m/s^2 
                //OutArray[&] contains the MSB of Z-acceleration expressed in m/s^2 
                    ConvertAcceleration(&OutArray[5],&AccelerationData[4]);
                    UART_PutArray(OutArray,DATA_TO_TRANSFER);
                    
                }
    //I leave some debug messages in case of occurrance of errors in reading the status registers or 
    //output registers data            
                else{
                    UART_PutString("Error occurred during I2C comm to read output registers data\r\n");
                }
        
    }
        }
    else
    {
        UART_PutString("Error occurred during I2C comm to read status register\r\n");   
    }
    
}
    }
   

 void ConvertAcceleration(uint8_t* DataToSend, uint8_t* OutputRegisterData)
{
    int16 out_acceleration;
    //The first step is to retrive back the two’s complement coding, new know that for high resolution
    //we obtain 12 bit output data. So a casting to an integer of 16 bit is enough.
    //*OutputRegisterData contains XXXX 0000 LSbs
    //*(OutputRegisterData+1) contains XXXX XXXX MSbs
    //In order to retrive the 12 bits i shift the MSbs to left 8 times 
    // Now what i get with an or bit a bit is XXXX XXXX XXXX 0000 so the last step is shift to the right 4 times
    // 0000 XXXX XXXX XXXX
    out_acceleration= (int16) (*OutputRegisterData |(*(OutputRegisterData+1)<<8))>>4 ;
    //From datashhet:for high resolution mode, 12-bit data output, FS ±2g, the sensitivy is  1 [mg/digit]
    float sensitivity= 9.81/1000; 
    int16 trick=1000;
    //This variable contains the value of acceleration of 1 axis expressed in m/s^2
    float out_acceleration_m_s2= out_acceleration*sensitivity; 
    //In order to send this value of acceleration up to the third decimal number, i multiple 
    // for 10^3 and save this value as an integer.
    out_acceleration= out_acceleration_m_s2*trick;
    //The value of acceleration which is expressed as int16 variable is send through UART 
    //comminucation using 2 bytes
    *(DataToSend)=(uint8_t) (out_acceleration & 0xFF); //This byte contains the LSB
    *(DataToSend+1)= (uint8_t) (out_acceleration >>8); //This byte contains teh MSB
    //In bridge control panel i retrive the 3 decimals by rescaling the value of 0.001
}
            

 void SystemStart(void){
    I2C_Peripheral_Start();
    UART_Start();
    EEPROM_Start();
    isr_PressedButton_StartEx(Custom_button_pressed_isr);
}
    
        
        
/* [] END OF FILE */
