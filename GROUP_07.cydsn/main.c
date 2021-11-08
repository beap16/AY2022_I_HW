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

#define BUFFERSIZE 7
//define buffer registers' indexes
#define CR0 0
#define TIMERP 1
#define WhoAmI 2
#define MSB_TEMP 3
#define LSB_TEMP 4
#define MSB_LDR 5
#define LSB_LDR 6


uint8_t buffer[BUFFERSIZE];
uint8_t average_samples = 5;
uint8_t timer_period;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_Start();
    
    EZI2C_Start();
    
    ADC_DelSig_Start();
   
    Mux_Start();
    
    isr_ADC_StartEx(My_ISR);
    
    Timer_Start();
    
    
    // read timer period from the component
    timer_period = Timer_ReadPeriod();
    
    //setting Buffer registers values
    buffer[CR0] = (average_samples << 2);
    buffer[TIMERP] = timer_period;
    buffer[WhoAmI] = 0xBC;
    buffer [MSB_TEMP] = 0x00;
    buffer [LSB_TEMP]= 0x00;
    buffer[MSB_LDR]= 0x00;
    buffer[LSB_LDR] = 0x00;
    
    // Set up EZI2C buffer
    EZI2C_SetBuffer1 (BUFFERSIZE, BUFFERSIZE-4, buffer);
    
    
    
    
    
    for(;;)
    { 
        //to do: implementare invio dati
    }
}

/* [] END OF FILE */
