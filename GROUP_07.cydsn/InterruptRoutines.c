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
//define buffer registers' indexes
#define CR0 0
#define TIMERP 1
#define WhoAmI 2
#define MSB_TEMP 3
#define LSB_TEMP 4
#define MSB_LDR 5
#define LSB_LDR 6

//defining status
#define CH0 1
#define CH1 2
#define OFF 0
#define BOTH 3

extern uint8_t average_samples;
extern uint8_t  buffer[];

int32 value_digit_LDR=0;
int32 value_digit_temp=0;
int32 value_digit;
int i=0;
uint16_t temp_mean=0;
uint16_t ldr_mean=0;
uint8_t status = OFF;
uint8_t status_mask = 00000011;


CY_ISR(My_ISR)
{
    status = (buffer[CR0] & status_mask) ;
    
    if(status == CH0 || status == BOTH){
    
        Timer_ReadStatusRegister();
    
        Mux_Select(TEMP);
    
        ADC_DelSig_StartConvert();
    
        value_digit= ADC_DelSig_Read32();
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_temp += value_digit;
    
        ADC_DelSig_StopConvert();
        
    }
    
    if(status == CH1 || status == BOTH){
        Mux_Select(LDR);
    
        ADC_DelSig_StartConvert();
    
        value_digit = ADC_DelSig_Read32();
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_LDR += value_digit;
    
        ADC_DelSig_StopConvert();
    
    }
    
    if (status != OFF) i++;
    
    if (i>=average_samples)
    {
        if(status == 1 || status == 3){
            temp_mean=value_digit_temp/average_samples;
            buffer[MSB_TEMP] = temp_mean >>8;
            buffer[LSB_TEMP] = temp_mean & 0xFF;
        }
        
        if(status == 2 || status == 3){
            ldr_mean=value_digit_LDR/average_samples;
            buffer[MSB_LDR] = ldr_mean >>8;
            buffer[LSB_LDR] = ldr_mean & 0xFF;
        }    
        
        value_digit_LDR=0;
        value_digit_temp=0;
        i=0;
    }
    
}

/* [] END OF FILE */
