/* main.c - Versión final: muestra T:19H:90C:90 en LCD */
#include "system.h"
#include <string.h>  // para strcpy

int main(void){
    driver_init();
    lcd_init();

    Estado estado_actual = INICIO;
    char estado_texto[10];
    char linea0[17];

    while (1){
        /* ?? Lectura de sensores (ya calibrados en driver.c) */
        int16_t temp_x100 = leer_temperatura_x100();   // ej: 1900
        int16_t hum_x100  = leer_humedad_x100();       // ej: 9000
        int16_t cal_x100  = leer_calidad_aire_x100();  // ej: 9000

        /* ?? Convertir a enteros (sin decimales) */
        int temp_int = temp_x100 / 100;  // 1900 / 100 = 19
        int hum_int  = hum_x100 / 100;   // 9000 / 100 = 90
        int cal_int  = cal_x100 / 100;   // 9000 / 100 = 90

        /* ?? Formato compacto: "T:19H:90C:90" (12 caracteres) */
        snprintf(linea0, 17, "T:%dH:%dC:%d", temp_int, hum_int, cal_int);

        /* ??? Mostrar en LCD, línea 0 */
        lcd_gotoxy(0, 0);
        lcd_print("                ");
        lcd_gotoxy(0, 0);
        lcd_print(linea0);

        /* ?? Máquina de estados */
        switch (estado_actual){
            case INICIO:
                estado_actual = inicio();
                strcpy(estado_texto, "INICIO");
                break;
            case ESPERA_MONITOREO:
                estado_actual = espera_monitoreo();
                strcpy(estado_texto, "ESPERA");
                break;
            case ENFRIANDO:
                estado_actual = enfriando();
                strcpy(estado_texto, "ENFRIA");
                break;
            case HUMIDIFICANDO:
                estado_actual = humidificando();
                strcpy(estado_texto, "HUMEDE");
                break;
            case VENTILANDO:
                estado_actual = ventilando();
                strcpy(estado_texto, "VENTIL");
                break;
            case PUERTA_ABIERTA:
                estado_actual = puerta_abierta();
                strcpy(estado_texto, "PUERTA");
                break;
            case EMERGENCIA_APAGADO:
                estado_actual = emergencia_apagado();
                strcpy(estado_texto, "EMERG ");
                break;
            default:
                estado_actual = ESPERA_MONITOREO;
                strcpy(estado_texto, "ERROR ");
                break;
        }

        /*  Mostrar estado en línea 1 */
        lcd_gotoxy(0, 1);
        lcd_print("                ");
        lcd_gotoxy(0, 1);
        lcd_print("Estado:");
        lcd_print(estado_texto);

        _delay_ms(250);
    }
    return 0;
}