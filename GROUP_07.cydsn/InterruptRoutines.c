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
#include "project.h"

// define channels
#define TEMP 0
#define LDR 1
#define SAMPLES 5

int32 value_digit_LDR=0;
int32 value_digit_temp=0;
int32 value_digit;
int i=0;
float temp_mean=0;
float lux_mean=0;


CY_ISR(My_ISR)
{
    Timer_ReadStatusRegister();
    
    Mux_Select(TEMP);
    
    value_digit= ADC_DelSig_Read32();
    
    if (value_digit<0) value_digit=0;
    if (value_digit>65535) value_digit=65535;
    
    value_digit_temp += value_digit;
    
    
    Mux_Select(LDR);
    
    value_digit = ADC_DelSig_Read32();
    
    if (value_digit<0) value_digit=0;
    if (value_digit>65535) value_digit=65535;
    
    value_digit_LDR += value_digit;
    
    i++;
    
    if (i>=SAMPLES)
    {
        temp_mean=value_digit_temp/SAMPLES;
        lux_mean=value_digit_LDR/SAMPLES; 
        //conversione
        value_digit_LDR=0;
        value_digit_temp=0;
        i=0;
    }
    
}

/* [] END OF FILE */
