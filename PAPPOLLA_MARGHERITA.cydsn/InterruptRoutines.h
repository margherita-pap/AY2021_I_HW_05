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
#ifndef __INTERRUTROUTINES_H
    #define __INTERRUTROUTINES_H
    #include "project.h"
    CY_ISR_PROTO(Custom_button_pressed_isr);
    volatile uint8_t flag_button_pressed;
#endif


/* [] END OF FILE */
