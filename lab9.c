//UNIVERSIDAD DEL VALLE DE GUATEMALA
//IE2023 Programación de Microcontroladores
//Autor:		Mónica Alfaro
//Compilador:	pic-as (v2.36), MPLABX (v6.00)
//
//Programa:	LAB9  (ADC controalador de contador con sleep y wake up en 2 bot)
//                 (+ 2do contador que guarde el estado anterior )
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

#include <xc.h>
#include <pic16f887.h>
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

/*variable*/

unsigned char check = 1;

/*DECLARACIÓN FUNCIONES*/


void setup(void);
void config_ADC(void);
void config_int(void);
void EEPROM(void);
void cargar_a_EEPROM();



/*INTERRUPCIONES*/


void __interrupt() isr(void)
{
    if(INTCONbits.RBIF)             // Si int viene RB0
    {
        INTCONbits.RBIF = 0;        
        
        if(!PORTBbits.RB0){         // Check
            check = 0;            //activar flag
        }
        
        if(!PORTBbits.RB1){         // Rev si push de wake up está presionado
            check = 1;            // desactivar flag
         }
        
        if(!PORTBbits.RB2){         // Rev si el push de guardar está presionado
            
            check = 1;            /// desactivar flag
            cargar_a_EEPROM();         // Almacener el valor del ADC en la EEPROM
        }
        if(!PORTBbits.RB3){         // Rev si el push de guardar está presionado
            
            cargar_a_EEPROM();         // Leer el valor guardado en la EEPROM
        }
    }
}

/*codigo principall*/


void main(void) {
    setup();
    config_ADC();
    config_int();

/*loop */

    
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
    TRISD = 0;                          // PORTD salida

    TRISB = 0b00000111;                 // PORTC entrada
    //Modo inicial PORTS
    PORTC = 0;                          // Limpiamos el puerto 
    PORTB = 0;    
    PORTD = 0;    

    
    
    
//clock    
    OSCCONbits.IRCF = 0b0110;           // BITS de OSSCON
    OSCCONbits.SCS = 1;                 // Habilitar reloj interno
    
//PUERTOB    
    
    OPTION_REGbits.nRBPU = 0;           // Habilitar pull up
    
    IOCBbits.IOCB0 = 1;                 // Habilitamos puerto B para las ISR
    IOCBbits.IOCB1 = 1;                 // Habilitamos puerto B para las ISR
    IOCBbits.IOCB2 = 1;                 // Habilitamos puerto B para las ISR
    IOCBbits.IOCB3 = 1;                 // Habilitamos puerto B para las ISR
    WPUBbits.WPUB0 = 1;                 // Habilita la pull up RB0, rb1,rb2 y rb3
    WPUBbits.WPUB1 = 1;               
    WPUBbits.WPUB2 = 1;                
    WPUBbits.WPUB3 = 1;                
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



void cargar_a_EEPROM(){
    
    uint8_t address = 0x0A;
    uint8_t data = ADRESH;
    
    while(EECON1bits.WR);    // Esperar  la escritura
    
    EEADR = address;         // Seleccionar la dirección 
    EEDATA = data;           // Cargar el dato 
    EECON1bits.EEPGD = 0;    // Seleccionar la memoria EEPROM
    EECON1bits.WREN = 1;     // Habilitar escritura en la EEPROM
    INTCONbits.GIE = 0;      // Deshabilitar inte globales
    EECON2 = 0x55;          
    EECON2 = 0xAA;           // habilitar escritura
    EECON1bits.WR = 1;       // Iniciar escritura
    
    EECON1bits.WREN = 0;    
   
    __delay_ms(10);          // Esperar para  la escritura en la EEPROM
    
    EEADR = address;         // Seleccionar la dirección de memoria
    EECON1bits.EEPGD = 0;    // Seleccionar   EEPROM
    EECON1bits.RD = 1;       //  lectura
    PORTD = EEDATA;          // Mostrar dato en PORTD
    INTCONbits.GIE = 1;      // Volver a activar interrupciones globales
}

void readToEEPROM(){

    uint8_t address = 0x0A;
    
    INTCONbits.GIE = 0;      // Deshabilitar interrupciones globales
    EEADR = address;         // Seleccionar la dirección de memoria
    EECON1bits.EEPGD = 0;    // Seleccionar EEPROM
    EECON1bits.RD = 1;       //  lectura
    PORTD = EEDATA;          // Mostrar el dato leído en PORTD
    INTCONbits.GIE = 1;      // Volver a activar interrupciones globales

}