/* ========================================
 * GRUPPO 07
 * Fossati Veronica
 * Prato Beatrice
 * ========================================
*/

#include "InterruptRoutines.h"
#include "project.h"

/*DICHIARAZIONI*/

//definizione canali (CH0 --> temperatura, CH1 --> LDR)
#define TEMP 0
#define LDR 1 

//definizione indici dei registri del buffer (da specifiche di progetto)
#define CR0 0
#define TIMERP 1
#define WhoAmI 2
#define MSB_TEMP 3
#define LSB_TEMP 4
#define MSB_LDR 5
#define LSB_LDR 6

//definizione stati del dispositivo (a seconda del valore dei bit 1-0 del control register 0x00)
#define OFF 0   //valore 00
#define CH0 1   //valore 01
#define CH1 2   //valore 10
#define BOTH 3  //valore 11

//definizione stati del led
#define LED_ON 1
#define LED_OFF 0

//dichiarazione variabili globali (vedi main)
extern uint8_t average_samples;
extern uint8_t  buffer[];
extern uint8_t timer_period;

//dichiarazione variabili funzionali al campionamento e al calcolo della media
int32 value_digit_LDR=0;
int32 value_digit_temp=0;
int32 value_digit;
int i=0;
uint16_t temp_mean;
uint16_t ldr_mean;

//dichiarazione variabile di stato (contiene lo stato corrente del dispositivo)
uint8_t status = OFF;

//dichiarazione maschere per il controllo della eventuale modifica dei registri da Bridge Control Panel
uint8_t status_mask = 3;
uint8_t average_mask = 15<<2;
uint8_t check_status;
uint8_t check_average;


/*ISR PER LA GESTIONE DEL CAMPIONAMENTO, DEL CALCOLO DELLA MEDIA E DELLA SCRITTURA DEI 16 BIT DI DATO DEI DUE CANALI
  NEI RISPETTIVI REGISTRI DEL BUFFER (frequenza di campionamento e numero di samples da mediare possono essere modificate
  tramite la scrittura sui rispettivi registri da Bridge Control Panel, i loro valori determinano la frequenza di 
  trasmissione del dato; nel codice i valori sono inizializzati secondo le specifiche di progetto, ma l'utente
  può personalizzare la comunicazione settando i valori opportuni)*/

CY_ISR(My_ISR)
{   
    Timer_ReadStatusRegister();
    
    /*CAMPIONAMENTO CANALE CH0 (TEMPERATURA)*/
    
    if(status == CH0 || status == BOTH)    //avviene solo se lo stato è 01 o 11
    {
        Mux_Select(TEMP);                  //collegare il multiplexer al canale opportuno
        ADC_DelSig_StartConvert();         //accendere l'ADC (per ragioni di stablità della conversione, da datasheet 
                                           //si suggerisce di mantenerlo spento durante il cambio di canale)
        value_digit= ADC_DelSig_Read32();  //campionare il segnale
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_temp += value_digit;   //monitorare la sommatoria dei samples da mediare
        ADC_DelSig_StopConvert();          //spegnere l'ADC
    }
    
    /*CAMPIONAMENTO CANALE CH1 (LDR)*/
    
    if(status == CH1 || status == BOTH)    //avviene solo se lo stato è 10 o 11
    {
        Mux_Select(LDR);
        ADC_DelSig_StartConvert();
        value_digit = ADC_DelSig_Read32();
    
        if (value_digit<0) value_digit=0;
        if (value_digit>65535) value_digit=65535;
    
        value_digit_LDR += value_digit;
        ADC_DelSig_StopConvert();
    }
    
    if (status != OFF) i++; //se il campionamento è acceso, aggiornare il conteggio del numero di samples nella sommatoria
    
    /*CALCOLO DELLA MEDIA E SCRITTURA DEI BIT DI DATO SUGLI OPPORTUNI REGISTRI*/
    
    if (i>=average_samples) //avviene se è stato raggiunto il numero di samples da mediare
    {
        if(status == CH0 || status == BOTH)
        {
            temp_mean=value_digit_temp/average_samples;
            buffer[MSB_TEMP] = temp_mean >>8;        //per separare il byte più e meno significativo
            buffer[LSB_TEMP] = temp_mean & 0xFF;
        }
        if(status == CH1 || status == BOTH){
            ldr_mean=value_digit_LDR/average_samples;
            buffer[MSB_LDR] = ldr_mean >>8;
            buffer[LSB_LDR] = ldr_mean & 0xFF;
        }    
        
        value_digit_LDR=0;  //azzerare sommatoria in modo da prepararsi all'acquisizione successiva
        value_digit_temp=0;
        i=0;                //azzerare il conteggio dei samples da mediare
    }
    
}


/*FUNZIONE ESEGUITA ALL'USCITA DI OGNI CALLBACK DEL MASTER PER MODIFICARE I PARAMETRI DEL CODICE (STATO, SAMPLES DA 
  MEDIARE E PERIODO DI CAMPIONAMENTO) SE MODIFICATI DALL'UTENTE E GESTIRE IL LED*/

void EZI2C_ISR_ExitCallback(void)
{
    check_status = (buffer[CR0] & status_mask);       //variabili per il controllo di eventuale modifica dei registri
    check_average = (buffer[CR0]& average_mask)>>2;
    
    if (status != check_status) //se l'utente ha modificato lo stato del dispositivo (bit 1-0 del control register 0)
    {
        status = check_status; //aggiornare la variabile di stato in funzione del cambiamento avvenuto su registro
        
        if (status == BOTH) Pin_LED_Write(LED_ON); //acceso se lo stato è 11
        else Pin_LED_Write(LED_OFF);               //spento altrimenti
        
        i=0;                 //azzerare il conteggio dei samples da mediare
        value_digit_LDR=0;   //azzerare sommatoria in modo da prepararsi all'acquisizione successiva
        value_digit_temp=0;
        buffer[MSB_LDR]=0;   //ad ogni cambio di stato i resitri di dati vengono azzerati
        buffer[LSB_LDR]=0;
        buffer[MSB_TEMP]=0;
        buffer[LSB_TEMP]=0;
    }
    
    if (average_samples != check_average)  //se l'utente ha modificato il numero di samples da mediare (bit 5-2 del
                                           //control register 0)
    { 
        average_samples = check_average;   //aggiornare la variabile in funzione del cambiamento avvenuto su registro
    }
    
    if (timer_period != buffer[TIMERP])    //se l'utente ha modificato il periodo di campionamento
    {
        timer_period = buffer[TIMERP];     //aggiornare la variabile in funzione del cambiamento avvenuto su registro
        Timer_WritePeriod(timer_period);   //riportare il cambiamento sul timer
    } 
}
/* [] END OF FILE */
