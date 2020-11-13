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
#ifndef __EEPROMCodes_H
    #define __EEPROMCodes_H
    #include "project.h"
    uint8_t EEPROM_Startup(void);
    void EEPROM_Startup_Update(uint8_t new_ctrl_reg1); 
#endif
// Address of EEPROM memory where i save the value of ctrl_reg1
#define EEPROM_ADDRESS 0x00

//This function read the value of the register 0x00 in EEPROM memory space and return it
uint8_t EEPROM_Startup(void){
    uint8_t eeprom_value;
    eeprom_value=EEPROM_ReadByte(EEPROM_ADDRESS);
    return eeprom_value;
}
//This function allows to update the register 0x00 in EEPROM memory
//by writing the new value of ctrl_reg1
void EEPROM_Startup_Update(uint8_t new_ctrl_reg1){
     EEPROM_UpdateTemperature();
     EEPROM_WriteByte(new_ctrl_reg1, EEPROM_ADDRESS);
}    
/* [] END OF FILE */
