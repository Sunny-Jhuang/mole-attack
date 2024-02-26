#include <xc.h>

void UART_Initialize() {
           
    TRISCbits.TRISC6 = 1;            
    TRISCbits.TRISC7 = 1;            
    
    TXSTAbits.SYNC = 0;           
    BAUDCONbits.BRG16 = 0;          
    SPBRG = 51;      
    
    RCSTAbits.SPEN = 1;              
    PIR1bits.TXIF = 1;
    PIR1bits.RCIF = 0;
    TXSTAbits.TXEN = 1;           
    RCSTAbits.CREN = 1; 
    
    }

void UART_Write(unsigned char data)  
{
    while(!TXSTAbits.TRMT);
    TXREG = data;             
}


void UART_Write_Text(char* text) { 
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}



