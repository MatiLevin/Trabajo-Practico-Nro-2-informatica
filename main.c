#include <inttypes.h>       
#include <avr/interrupt.h>  
#include <avr/sleep.h>      
#include <avr/io.h>         
#include <util/delay.h>     
#include "estados.h"        
#include "driver.h"         
#include <stdio.h>          
#include <string.h>         
#include <stdlib.h>         
#include "lcd.h"            

/****************************************************************
 *  Función principal
 ****************************************************************/
int main(void){

    driver_init();              /* Inicializa ADC, GPIO, Timers y Pull-ups.
                                  * Es fundamental para habilitar sensores
                                  * y configurar los periféricos del sistema.
                                  */

    lcd_init();                 /* Inicializa la pantalla LCD en modo 4 bits.
                                  * Configura líneas, limpia pantalla y habilita el display.
                                  */

    Estado estado_actual = INICIO;  
                                /* Variable que almacena el estado actual de la FSM.
                                 * Se inicializa en el estado INICIO, como corresponde.
                                 */

    char estado_texto[16];      
    char linea0[17];           
    char t_str[6], h_str[6], c_str[6];  
                              
    /*******************************************************
     * Bucle infinito del sistema (super-loop)
     *******************************************************/
    while (1){

        /***************************************************
         * 1) LECTURA DE SENSORES
         ***************************************************/
        float temp = leer_temperatura();   /* Obtiene temperatura en °C */
        float hum  = leer_humedad();       /* Obtiene humedad relativa % */
        float cal  = leer_calidad_aire();  /* Índice de calidad de aire */

        /* Conversión de valores float a strings
         * dtostrf(valor, ancho, decimales, buffer)
         * Usamos 0 decimales para que entren en 16 caracteres.
         */
        dtostrf(temp, 3, 0, t_str);
        dtostrf(hum,  3, 0, h_str);
        dtostrf(cal,  3, 0, c_str);

        /* Construcción de la línea del LCD sin espacios innecesarios.
         * Ej: "T:25H:60C:40"
         */
        snprintf(linea0, 17, "T:%sH:%sC:%s", t_str, h_str, c_str);

        /* Impresión de la línea 0 del LCD */
        lcd_gotoxy(0, 0);                 // Posicionar en fila 0, columna 0
        lcd_print("                ");     // Limpiar 16 caracteres
        lcd_gotoxy(0, 0);
        lcd_print(linea0);                // Mostrar valores


        /***************************************************
         * 2) MÁQUINA DE ESTADOS FINITOS
         ***************************************************/
        switch (estado_actual){

            case INICIO:
                estado_actual = inicio();         // Ejecuta lógica del estado
                strcpy(estado_texto, "INICIO ");  // Texto para LCD
                break;

            case ESPERA_MONITOREO:
                estado_actual = espera_monitoreo();
                strcpy(estado_texto, "ESPERA ");
                break;

            case ENFRIANDO:
                estado_actual = enfriando();
                strcpy(estado_texto, "ENFRIA ");
                break;

            case HUMIDIFICANDO:
                estado_actual = humidificando();
                strcpy(estado_texto, "HUMEDECE ");
                break;

            case VENTILANDO:
                estado_actual = ventilando();
                strcpy(estado_texto, "VENTILA ");
                break;

            case PUERTA_ABIERTA:
                estado_actual = puerta_abierta();
                strcpy(estado_texto, "PUERTA ");
                break;

            case EMERGENCIA_APAGADO:
                estado_actual = emergencia_apagado();
                strcpy(estado_texto, "EMERG ");
                break;

            default:                          // Caso de seguridad
                estado_actual = ESPERA_MONITOREO;
                strcpy(estado_texto, "DEFECTO");
                break;
        }


        /***************************************************
         * 3) MOSTRAR ESTADO EN LÍNEA 1 DEL LCD
         ***************************************************/
        lcd_gotoxy(0, 1);                 // Fila inferior
        lcd_print("                ");     // Limpiar línea
        lcd_gotoxy(0, 1);
        lcd_print("Estado:");            // Prefijo fijo
        lcd_print(estado_texto);         // Nombre del estado


        _delay_ms(200);                   /* Pequeño retardo para evitar
                                           * parpadeos y reducir carga.
                                           */
    }

    return 0;   // Nunca se ejecuta, pero se deja por estándar.
}
