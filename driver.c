/* driver.c - Implementación de la interacción hardware mediante ADC, GPIO y Timer0.
   Este módulo centraliza todas las funciones de bajo nivel necesarias para que
   la lógica de control pueda leer sensores, accionar actuadores y medir tiempo. */

#include "driver.h"              
#include <avr/io.h>              
#include <avr/interrupt.h>       
#include <util/delay.h>         

/* Variables para temporización. Todas son volatile porque son modificadas dentro
   de rutinas de interrupción, evitando optimizaciones del compilador. */
volatile uint32_t g_millis = 0;         /* Acumulador aproximado de milisegundos */
volatile uint32_t g_seconds_timer = 0;  /* Contador de segundos usado en el estado PUERTA_ABIERTA */
volatile bool g_timer_running = false;  /* Bandera que indica si el conteo de segundos está activo */

#ifndef F_CPU
#define F_CPU 8000000UL         /* Frecuencia del CPU fijada a 8 MHz  */
#endif

/*-----------------------------------------------------------------------------
   Inicialización del Timer0 para generar interrupciones periódicas
-----------------------------------------------------------------------------*/
void timer0_init_ms(void){

    /* Selecciono prescaler clk/64 configurando los bits CS01 y CS00 en TCCR0.
       Esto determina la velocidad a la cual incrementa el contador del Timer0. */
    TCCR0 = (1<<CS01) | (1<<CS00);

    /* Habilito interrupción por overflow del Timer0 (TOIE0).
       Cada vez que el timer desborde, se ejecutará la ISR asociada. */
    TIMSK |= (1<<TOIE0);
}

/*-----------------------------------------------------------------------------
   Servicio de interrupción del Timer0
-----------------------------------------------------------------------------*/
ISR(TIMER0_OVF_vect){

    /* Sumamos ~2 ms por cada overflow (según configuración de reloj y prescaler). */
    g_millis += 2;

    /* Solo acumulamos segundos si el timer está habilitado. */
    if(g_timer_running){

        static uint16_t accum = 0; /* Acumulador interno en milisegundos */
        accum += 2;                /* Sumamos los ~2 ms del overflow */

        /* Cada 1000 ms contamos 1 segundo real */
        if(accum >= 1000){
            accum -= 1000;
            g_seconds_timer++;
        }
    }
}

/*-----------------------------------------------------------------------------
   Control del temporizador de segundos
-----------------------------------------------------------------------------*/
void timer_start_seconds(void){
    g_seconds_timer = 0;     /* Reiniciamos contador */
    g_timer_running = true;  /* Habilitamos conteo */
}

void timer_stop_seconds(void){
    g_timer_running = false; /* Detenemos conteo */
}

uint32_t timer_get_seconds(void){
    return g_seconds_timer;  /* Devolvemos segundos transcurridos */
}

/*-----------------------------------------------------------------------------
   Inicialización del ADC del ATmega128
-----------------------------------------------------------------------------*/
void adc_init(void){

    /* REFS0 = 1  referencia AVcc (5V).
       El resto de bits se dejan en 0, configurando lectura en ADC0 por defecto. */
    ADMUX = (1<<REFS0);

    /* Habilito ADC (ADEN) y coloco prescaler 128 (ADPS2:0).
       Esto ajusta la velocidad del ADC para cumplir requisitos de frecuencia. */
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

/*-----------------------------------------------------------------------------
   Lectura genérica de un canal ADC
-----------------------------------------------------------------------------*/
uint16_t adc_read(uint8_t ch){

    /* Selecciono canal ADC manteniendo configuraciones previas de referencia. */
    ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);

    /* Inicio conversión A/D estableciendo ADSC. */
    ADCSRA |= (1<<ADSC);

    /* Mientras ADSC siga en 1, la conversión no terminó. */
    while(ADCSRA & (1<<ADSC));

    /* Devuelvo el valor convertido (10 bits). */
    return ADC;
}

/*-----------------------------------------------------------------------------
   Lectura de temperatura desde sensor LM35 (10 mV / °C)
-----------------------------------------------------------------------------*/
float leer_temperatura(void){

    uint16_t raw = adc_read(0);                 /* Leo ADC0 */
    float v = (raw * 5.0f) / 1023.0f;           /* Convierto a tensión con Vref = 5V */
    float tempC = v / 0.01f;                    /* LM35: 10 mV/°C ? V / 0.01 */
    return tempC;
}

/*-----------------------------------------------------------------------------
   Lectura de humedad (simulada usando la misma relación del LM35)
-----------------------------------------------------------------------------*/
float leer_humedad(void){

    uint16_t raw = adc_read(1);                 /* Leo ADC1 */
    float h = (raw * 5.0f) / 1023.0f;           /* Convierto a tensión */
    float humC = h / 0.01f;                     /* Relación ficticia para simulación */
    return humC;
}

/*-----------------------------------------------------------------------------
   Lectura de calidad de aire (simulada con el mismo modelo)
-----------------------------------------------------------------------------*/
float leer_calidad_aire(void){

    uint16_t raw = adc_read(2);                 /* Leo ADC2 */
    float a = (raw * 5.0f) / 1023.0f;           /* Convierto a tensión */
    float calC = a / 0.01f;                     /* Relación ficticia para simulación */
    return calC;
}

/*-----------------------------------------------------------------------------
   Inicialización general del driver de hardware
-----------------------------------------------------------------------------*/
void driver_init(void){

    /* PF0..PF7 como entradas para ADC */
    DDRF = 0x00;

    /*----------------------------------------------
       Configuración del puerto D
    ----------------------------------------------*/

    /* PD1 y PD3 se configuran como entradas digitales. */
    DDRD &= ~((1<<PD1) | (1<<PD3));

    /* Activamos sus resistencias pull-up internas. */
    PORTD |= (1<<PD1) | (1<<PD3);

    /* PD4 se configura como salida digital. */
    DDRD |= (1<<PD4);

    /* Nivel inicial en 0 (apagado). */
    PORTD &= ~(1<<PD4);

    /*----------------------------------------------
       Configuración del puerto C (PC0..PC4)
       LEDs y actuadores representados como salidas
    ----------------------------------------------*/
    DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC4);

    /* Inicializo todos los actuadores apagados. */
    PORTC &= ~((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC4));

    /* Inicialización de subsistemas */
    adc_init();         /* ADC listo para lecturas */
    timer0_init_ms();   /* Timer0 configurado */

    sei();              /* Habilitación de interrupciones globales */
}

/*-----------------------------------------------------------------------------
   Lectura digital del sensor de puerta
-----------------------------------------------------------------------------*/
bool lectura_puerta(void){

    /* Como PD1 tiene pull-up, leeremos:
       - 1 si el switch está abierto,
       - 0 si está presionado o conectado a GND. */
    return (PIND & (1<<PD1)) != 0;
}

/*-----------------------------------------------------------------------------
   Control de actuadores simulados con LEDs
-----------------------------------------------------------------------------*/
void led_ok_on(void){ PORTC |=  (1<<PC0); }
void led_ok_off(void){ PORTC &= ~(1<<PC0); }

void compresor_on(void){ PORTC |=  (1<<PC1); }
void compresor_off(void){ PORTC &= ~(1<<PC1); }

void humidificador_on(void){ PORTC |=  (1<<PC2); }
void humidificador_off(void){ PORTC &= ~(1<<PC2); }

void ventilador_on(void){ PORTC |=  (1<<PC3); }
void ventilador_off(void){ PORTC &= ~(1<<PC3); }

void led_puerta_on(void){ PORTC |=  (1<<PC4); }
void led_puerta_off(void){ PORTC &= ~(1<<PC4); }

void led_emergencia_on(void){ PORTD |=  (1<<PD4); }
void led_emergencia_off(void){ PORTD &= ~(1<<PD4); }
