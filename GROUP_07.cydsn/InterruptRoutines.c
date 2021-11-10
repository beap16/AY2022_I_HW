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

//defining led status
#define LED_ON 1
#define LED_OFF 0

extern uint8_t average_samples;
extern uint8_t  buffer[];
extern uint8_t timer_period;

int32 value_digit_LDR=0;
int32 value_digit_temp=0;
int32 value_digit;
int i=0;
uint16_t temp_mean=0;
uint16_t ldr_mean=0;
uint8_t status = OFF;
uint8_t status_mask = 3;
uint8_t average_mask = 15<<2;
uint8_t check_status;
uint8_t check_average;


CY_ISR(My_ISR)
{   
    Timer_ReadStatusRegister();
    
    if(status == CH0 || status == BOTH){
    
    
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
        if(status == CH0 || status == BOTH){
            temp_mean=value_digit_temp/average_samples;
            buffer[MSB_TEMP] = temp_mean >>8;
            buffer[LSB_TEMP] = temp_mean & 0xFF;
        }
        
        if(status == CH1 || status == BOTH){
            ldr_mean=value_digit_LDR/average_samples;
            buffer[MSB_LDR] = ldr_mean >>8;
            buffer[LSB_LDR] = ldr_mean & 0xFF;
        }    
        
        value_digit_LDR=0;
        value_digit_temp=0;
        i=0;
    }
    
}

//interrupt in callback della trasmissione
void EZI2C_ISR_ExitCallback(void){
    check_status = (buffer[CR0] & status_mask);
    check_average = (buffer[CR0]& average_mask)>>2;
    //cambio stato e gestione led
    if (status != check_status){
        status = check_status ;
        if (status == BOTH) Pin_LED_Write(LED_ON);
        else Pin_LED_Write(LED_OFF);
        //azzera il conteggio dei sample per il calcolo della media e riinizializzo i registri di dati a zero
        i=0;
        value_digit_LDR=0;
        value_digit_temp=0;
        buffer[MSB_LDR]=0;
        buffer[LSB_LDR]=0;
        buffer[MSB_TEMP]=0;
        buffer[LSB_TEMP]=0;
    }
    //aggiornare valore di samples mediati, se variato
    if (average_samples != check_average)
    { 
        average_samples = check_average;
    }
    //aggiornare valore di periodo, se variato
    if (timer_period != buffer[TIMERP])
    {
        timer_period = buffer[TIMERP];
        Timer_WritePeriod(timer_period);
    }
    
}
/* [] END OF FILE */
