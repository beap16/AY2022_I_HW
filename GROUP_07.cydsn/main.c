/* ========================================
 * GRUPPO 07
 * Fossati Veronica
 * Prato Beatrice
 * ========================================
*/
#include "project.h"
#include "InterruptRoutines.h"

/*DICHIARAZIONI*/

//definizione dimesione buffer (fissa)
#define BUFFERSIZE 7

//definizione indici dei registri del buffer (da specifiche di progetto)
#define CR0 0
#define TIMERP 1
#define WhoAmI 2
#define MSB_TEMP 3
#define LSB_TEMP 4
#define MSB_LDR 5
#define LSB_LDR 6

//dichiarazione buffer, periodo del timer e numero di campioni da mediare
uint8_t buffer[BUFFERSIZE];
uint8_t average_samples = 5; //inizializzato a 5, come da specifiche, ma modificabile da Bridge Control Panel per
                             //personalizzare la comunicazione
uint8_t timer_period;

int main(void)
{
    /*AVVIO INTERRUPT E COMPONENTI UTILI AL CAMPIONAMENTO*/
    
    CyGlobalIntEnable; 
    
    EZI2C_Start();
    
    ADC_DelSig_Start();
   
    Mux_Start();
    
    isr_ADC_StartEx(My_ISR);
    
    Timer_Start();
    
    
    /*SETTAGGIO DEL BUFFER DELLO SLAVE*/
    
    //periodo inizializzato al valore impostato da TopDesign (T=4 con frequenza di clock a 1kHz per ottenere 
    //periodo di campionamento T=4ms --> calcolo media e invio ogni 20ms (50Hz))
    timer_period = Timer_ReadPeriod();
    
    //settaggio dei valori iniziali dei registri del buffer
    buffer[CR0] = (average_samples << 2); //valore iniziale: 00010100 --> campionamento non attivo
    buffer[TIMERP] = timer_period;
    buffer[WhoAmI] = 0xBC; //fisso
    buffer [MSB_TEMP] = 0x00;
    buffer [LSB_TEMP]= 0x00;
    buffer[MSB_LDR]= 0x00;
    buffer[LSB_LDR] = 0x00;
    
    //creazione EZI2C buffer
    EZI2C_SetBuffer1 (BUFFERSIZE, BUFFERSIZE-4, buffer);
    
    
    for(;;)
    { 
      
    }
}

/* [] END OF FILE */
