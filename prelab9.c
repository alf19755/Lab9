//UNIVERSIDAD DEL VALLE DE GUATEMALA
//IE2023 Programación de Microcontroladores
//Autor:		Mónica Alfaro
//Compilador:	pic-as (v2.36), MPLABX (v6.00)
//
//Programa:	PRELAB9  (ADC controalador de contador con sleep y wake up en 2 bot)	
//
//				
//Dispositivo:	PIC16F887
//Hardware:	LEDs en el puerto D, botones en el puerto B
//
//Creado:	       24 de abril , 2023
//Última modificación:   24 de abril , 2023


// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


//Librerìas
#include <xc.h>
#include <pic16f887.h>
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

/*VARIABLES*/

unsigned char check = 1;

/*DECLARACIÓN FUNCIONES*/
void setup(void);
void config_ADC(void);
void config_int(void);

/*INTERRUPCIONES*/


void __interrupt() isr(void)
{
    if(INTCONbits.RBIF)             // Si la interrupción proviene del pin RB0
    {
        INTCONbits.RBIF = 0;        // Limpia la bandera de interrupción
        if(!PORTBbits.RB0){         // Verificamos si el botón de sleep está presionado
        
            check = 0;            // Encendemos la bandera del while
        
        }
        
        if(!PORTBbits.RB1){         // Verificamos si el botón de wake up está presionado
                
            check = 1;            // Apagamos la bandera del while

         } 
    }
}

/*CÓDIGO PRINCIPAL*/


void main(void) {
    setup();
    config_ADC();
    config_int();

/*LOOP*/

    while(1){  
        
        // conversiòn adc
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO == 1);
        
        //Mostrar en puertoc
        __delay_ms(10);
        PORTC = ADRESH;           
     
        //Colocar en sleep
        while(!check){            // Mientras la bandera = 0
            SLEEP();                // Pone al microcontrolador en modo de suspensión
        
        }
        
    }
}

/*SUBRUTINAS O FUNCIONES DEFINIDAS*/


void setup(void){
    
//I/O
    //Pines analogicos/digitales
    ANSELH = 0;
    //Declaraciòn i/o
    TRISC = 0;                          // PORTC salida
    TRISB = 0b00000111;                 // PORTC entrada
    //Modo inicial PORTS
    PORTC = 0;                          // Limpiamos el puerto C
    PORTB = 0;
    
//clock    
    OSCCONbits.IRCF = 0b0110;           // BITS de OSSCON
    OSCCONbits.SCS = 1;                 // Habilitar reloj interno
    
//PUERTOB    
    
    OPTION_REGbits.nRBPU = 0;           // Habilitar pull up
    
    IOCBbits.IOCB0 = 1;                 // Hab puerto B para  ISR
    IOCBbits.IOCB1 = 1;                 
    WPUBbits.WPUB0 = 1;                 // Habilita la pull up RB0 y RB1
    WPUBbits.WPUB1 = 1;                 
    
    
    
    
}

void config_ADC(void){
    

    TRISAbits.TRISA0 = 1;       //Selección entrada
    ANSELbits.ANS0 = 1;
    
    // Check canal ADC
    
    ADCON0bits.ADCS1 = 0;
    ADCON0bits.ADCS0 = 1;       // Fosc/ 8
    
    ADCON1bits.VCFG1 = 0;       // Referencia VSS
    ADCON1bits.VCFG0 = 0;       // Referencia VDD
    
    ADCON1bits.ADFM = 0;        // Justificado hacia izquierda
    
    ADCON0bits.CHS3 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;        // Canal AN0
    
    ADCON0bits.ADON = 1;        // Habilitamos el ADC
    __delay_us(100);
    
}



void config_int(void){

    INTCONbits.GIE = 1;                 // Habilitamos las interrupciones GLOBALES
    INTCONbits.RBIE = 1;                // Habilitamos las interrupciones PORTB

}

