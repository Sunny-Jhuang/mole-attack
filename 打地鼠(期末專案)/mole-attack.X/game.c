#include <xc.h>
#include <pic18f4520.h>
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <string.h>
#include "uart.h"

#pragma config OSC = INTIO67  //OSCILLATOR SELECTION BITS (INTERNAL OSCILLATOR BLOCK, PORT FUNCTION ON RA6 AND RA7)
#pragma config WDT = OFF      //Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config PWRT = OFF     //Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON     //Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config PBADEN = OFF   //PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LVP = OFF      //Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config CPD = OFF      //Data EEPROM Code Protection bit (Data EEPROM not code-protected)

#define _XTAL_FREQ  1000000
char str[10];
int check_0 = 0;
int check_1 = 0;
int check_2 = 0;
int score = 0;
int T = 30;
int motor_num = 5;

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}

void motor_angle(int number, int state){
    if(number==0){
        if(state == 0){
            CCPR1L = 0x78;
            CCP1CONbits.DC1B = 0b01;
        }
        else{
            CCPR1L = 0x3d;
            CCP1CONbits.DC1B = 0b00;
        }
    }
    if(number==1){
        if(state == 0){
            CCPR2L = 0x78;
            CCP2CONbits.DC2B = 0b01;
        }
        else{
            CCPR2L = 0x3d;
            CCP2CONbits.DC2B = 0b00;
        }
    }
    return;
}

void __interrupt(high_priority)H_ISR(){
    if(INTCONbits.INT0IF == 1 || INTCON3bits.INT1IF == 1){
        if(INTCONbits.INT0IF == 1){
            LATAbits.LA0 = 1;
            check_0 = 1;
        }
        if(INTCON3bits.INT1IF == 1){
            LATAbits.LA1 = 1;
            check_1 = 1;
        }
        if((motor_num==0&&check_0==1) || (motor_num==1&&check_1==1)){
            LATAbits.LA4 = 1;
            __delay_ms(50);
            LATAbits.LA4 = 0;
            __delay_ms(50);
            LATAbits.LA4 = 1;
            __delay_ms(50);
            LATAbits.LA4 = 0;
            __delay_ms(50);
            score++;
            UART_Write('\r');
            UART_Write_Text("Score : ");
            UART_Write_Text(itoa(score, 10));
        }

        motor_angle(0, 0);
        motor_angle(1, 0);
        __delay_ms(500);

        INTCONbits.INT0IF = 0;
        INTCON3bits.INT1IF = 0;
        LATA = 0;
        check_0 = 0;
        check_1 = 0;
    }
    
    return;
}

void __interrupt(low_priority)L_ISR(){

if(TMR0IF==1){
    T0CONbits.TMR0ON = 0;
    UART_Write('\r');
    UART_Write_Text("Score : ");
    UART_Write_Text(itoa(score, 10));
    motor_num = rand()%3;
    motor_angle(motor_num, 1);
    if(score<5)
        __delay_ms(4000);
    else if(score>=5&&score<10)
        __delay_ms(2000);
    else
        __delay_ms(1500);
    motor_angle(motor_num, 0);
    motor_num = 5;
    if(score>=5&&score<10)
        __delay_ms(2000);
    else if(score >=10)
        __delay_ms(2500);
    
    
    
    TMR0IF=0;
    TMR0H = 0xF0;
    TMR0L = 0xB0;
    T0CONbits.TMR0ON = 1;
}
}

void main(void) 
{
    
    srand( time(NULL) );
    
    ADCON1 = 0x0F;
    // timer0
    OSCCON = 0x60; // 4MHz
    T0CON = 0x07;
    TMR0H = 0xF0;
    TMR0L = 0xB0;
    
    UART_Initialize();
    //CCP
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b11;
    CCP1CONbits.CCP1M = 0b1100;
    CCP2CONbits.CCP2M = 0b1100;
    TRISC = 0;
    LATC = 0;
    PR2 = 0x9b;
    CCPR1L = 0x3d;
    CCP1CONbits.DC1B = 0b00;
    CCPR2L = 0x3d;
    CCP2CONbits.DC2B = 0b00;
    
    // I/O setting
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;
    TRISAbits.RA0 = 0;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    LATA = 0;
    LATB = 0;
    //interrupt setting
    RCONbits.IPEN = 1;
    INTCONbits.GIE = 1;
    INTCONbits.GIEL = 1;
    
    UART_Initialize();
    
    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IP = 1;
    
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP = 0;
    T0CONbits.TMR0ON = 1;
    
    
    //test
    TRISAbits.RA3 = 0;
    TRISAbits.RA4 = 0;
    
    UART_Write('\r');
    UART_Write_Text("Score : " );
    __delay_ms(4000);
    motor_angle(0,0);
    motor_angle(1,0);
    while(1);
    
    return;
}
