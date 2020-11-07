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
    void EEPROM_Startup(void);
    //void EEPROM_Startup_Update(...); gli devo passare il valore del registro
    volatile uint8_t eeprom_value;
#endif

void EEPROM_Startup(void){
    eeprom_value=EEPROM_ReadByte(0x00); //potrei fare una define si 0x00
}
/* [] END OF FILE */
