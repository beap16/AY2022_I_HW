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
#include "InterruptRoutines.h"
#define LED_ON 1
#define LED_OFF 0

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_Start();
    
    EZI2C_Start();
    
    ADC_DelSig_Start();
   
    Mux_Start();
    
    isr_ADC_Start();
    
    Timer_Start();
    
   

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
