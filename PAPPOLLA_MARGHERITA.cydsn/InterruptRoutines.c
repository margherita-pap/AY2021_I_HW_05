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
#include "InterruptRoutines.h"

//I decided to leave the ISR practically empty and so all the operations
//of reding the control register 1, update its value and write the new value on
//the control register and on EEPROM memory inside the main function when the flag 
//flag_button_pressed goes high.

CY_ISR(Custom_button_pressed_isr){
    flag_button_pressed=1;
}

/* [] END OF FILE */
