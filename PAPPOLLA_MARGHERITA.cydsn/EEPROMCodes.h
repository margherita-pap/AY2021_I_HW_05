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

uint8_t EEPROM_Startup(void){
    uint8_t eeprom_value;
    eeprom_value=EEPROM_ReadByte(0x00); //potrei fare una define si 0x00
    return eeprom_value;
}

void EEPROM_Startup_Update(uint8_t new_ctrl_reg1){
    EEPROM_UpdateTemperature();
    EEPROM_WriteByte(new_ctrl_reg1, 0x00);
}    
/* [] END OF FILE */
