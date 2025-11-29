#include "system.h"

volatile uint32_t g_millis = 0;
volatile uint32_t g_seconds_timer = 0;
volatile bool g_timer_running = false;

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

void timer0_init_ms(void){
    TCCR0 = (1<<CS01) | (1<<CS00);
    TIMSK |= (1<<TOIE0);
}

ISR(TIMER0_OVF_vect){
    g_millis += 2;
    if(g_timer_running){
        static uint16_t accum = 0;
        accum += 2;
        if(accum >= 1000){
            accum -= 1000;
            g_seconds_timer++;
        }
    }
}

void timer_start_seconds(void){
    g_seconds_timer = 0;
    g_timer_running = true;
}

void timer_stop_seconds(void){
    g_timer_running = false;
}

uint32_t timer_get_seconds(void){
    return g_seconds_timer;
}

void adc_init(void){
    ADMUX = (1<<REFS0);  // AVcc = 5V
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // prescaler 128
}

uint16_t adc_read(uint8_t ch){
    ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC));
    return ADC;
}


int16_t leer_temperatura_x100(void){
    uint16_t raw = adc_read(0);
   
    int32_t temp_x100 = (raw * 4872L + 50) / 100;
    if (temp_x100 > 32767) temp_x100 = 32767;
    if (temp_x100 < -32768) temp_x100 = -32768;
    return (int16_t)temp_x100;
}

int16_t leer_humedad_x100(void){
    uint16_t raw = adc_read(1);
  
    int32_t hum_x100 = (raw * 4865L + 50) / 100;
    if (hum_x100 < 0) hum_x100 = 0;
    if (hum_x100 > 10000) hum_x100 = 10000;
    return (int16_t)hum_x100;
}

int16_t leer_calidad_aire_x100(void){
    uint16_t raw = adc_read(2);
    int32_t cal_x100 = (raw * 4865L + 50) / 100;
    if (cal_x100 < 0) cal_x100 = 0;
    if (cal_x100 > 10000) cal_x100 = 10000;
    return (int16_t)cal_x100;
}

void driver_init(void){
    DDRF = 0x00;
    DDRD &= ~((1<<PD1) | (1<<PD3));
    PORTD |= (1<<PD1) | (1<<PD3);
    DDRD |= (1<<PD4);
    PORTD &= ~(1<<PD4);
    DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC4);
    PORTC &= ~((1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3)|(1<<PC4));
    adc_init();
    timer0_init_ms();
    sei();
}

bool lectura_puerta(void){
    return (PIND & (1<<PD1)) != 0;
}

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