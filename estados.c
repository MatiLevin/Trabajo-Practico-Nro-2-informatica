/* estados.c - Implementación completa de la lógica de la máquina de estados */

#include "estados.h"     
#include "driver.h"      

/* Definición de constantes de control */
#define SetT    22.0f    // Temperatura objetivo del sistema (°C).
#define deltaT  1.0f     // Banda de tolerancia para la temperatura.
#define SetH    70.0f    // Humedad relativa objetivo (%).
#define deltaH  3.0f     // Banda de tolerancia para humedad.
#define SetCA   80.0f    // Calidad de aire objetivo (unidades arbitrarias).
#define deltaCA 3.0f     // Banda de tolerancia para calidad de aire.
#define T_EXCESO 10      // Tiempo máximo permitido con la puerta abierta (segundos).

/* Librerías auxiliares */
#include "lcd.h"         // Control del display LCD.
#include <stdio.h>       // Utilizada para funciones de formateo como sprintf.

/* ------------------------------------------------------------- */
/* -------------------- ESTADO: INICIO -------------------------- */
/* ------------------------------------------------------------- */

Estado inicio(void){
    /* Al iniciar el sistema, se realiza la desactivación de todos los actuadores
       y señales luminosas para garantizar un arranque seguro. */

    led_emergencia_off();   // Se apaga el LED de emergencia por seguridad.
    compresor_off();         // Se asegura que el compresor no esté activo.
    humidificador_off();     // Se apaga el humidificador.
    ventilador_off();        // Se mantiene apagado el ventilador.
    led_ok_off();            // Se apaga el indicador de funcionamiento correcto.
    led_puerta_off();        // Se apaga el LED asociado a la puerta.

    timer_stop_seconds();    // Se detiene cualquier temporización previa.

    return ESPERA_MONITOREO; // Transición al estado de monitoreo inicial.
}

/* ------------------------------------------------------------- */
/* --------------- ESTADO: ESPERA_MONITOREO --------------------- */
/* ------------------------------------------------------------- */

Estado espera_monitoreo(void){

    /* Se verifica primero la condición de puerta abierta,
       ya que tiene prioridad sobre otros controles. */
    if(lectura_puerta()){
        timer_start_seconds();    // Comienza el conteo del tiempo con la puerta abierta.
        return PUERTA_ABIERTA;    // Se transiciona al estado correspondiente.
    }

    /* Lectura de sensores principales */
    float T = leer_temperatura();     // Obtención de temperatura ambiente.
    float CA = leer_calidad_aire();   // Lectura de calidad de aire.
    float H = leer_humedad();         // Lectura de humedad relativa.

    /* Antes de evaluar condiciones, se apagan los indicadores y actuadores */
    led_emergencia_off();     // Se asegura que no haya alerta previa.
    led_ok_off();              // Se apaga indicador OK para volver a evaluar.
    compresor_off();           // Previene activación no controlada del compresor.
    humidificador_off();       // Apaga humidificador antes de decisiones.
    ventilador_off();          // Lo mismo para el ventilador.
    led_puerta_off();          // Se apaga señal de puerta.

    /* Evaluación de temperatura fuera del rango */
    if (T > (SetT + deltaT)){
        compresor_on();        // Activación del compresor.
        return ENFRIANDO;      // Transición al estado de enfriamiento.
    }

    /* Evaluación de humedad debajo del valor objetivo */
    if (H < (SetH - deltaH)){
        humidificador_on();    // Activación del humidificador.
        return HUMIDIFICANDO;  // Transición al estado correspondiente.
    }

    /* Evaluación de calidad de aire por debajo del umbral */
    if (CA < (SetCA + deltaCA)){
        ventilador_on();       // Activación del ventilador.
        return VENTILANDO;     // Transición al estado de ventilación.
    }

    /* Si ninguna condición requiere acción, se enciende el indicador OK */
    led_ok_on();               // Señal visual de que todo se encuentra dentro de parámetros normales.

    return ESPERA_MONITOREO;   // Se permanece en el mismo estado.
}

/* ------------------------------------------------------------- */
/* ------------------ ESTADO: ENFRIANDO ------------------------- */
/* ------------------------------------------------------------- */

Estado enfriando(void){

    compresor_on();            // Asegura que el compresor permanezca encendido.

    /* Verificación prioritaria de puerta abierta */
    if(lectura_puerta()){
        timer_start_seconds();
        return PUERTA_ABIERTA;
    }

    /* Reevaluación de la temperatura */
    float T = leer_temperatura();
    if (T < (SetT - deltaT)){  // Cuando la temperatura cae debajo de la banda aceptable:
        compresor_off();       // Se apaga el compresor.
        return ESPERA_MONITOREO;
    }

    return ENFRIANDO;          // Permanece en el modo de enfriamiento.
}

/* ------------------------------------------------------------- */
/* ---------------- ESTADO: HUMIDIFICANDO ----------------------- */
/* ------------------------------------------------------------- */

Estado humidificando(void){

    humidificador_on();        // Mantiene el humidificador activo.

    /* Control prioritario de puerta abierta */
    if(lectura_puerta()){
        timer_start_seconds();
        return PUERTA_ABIERTA;
    }

    float H = leer_humedad();  // Revisión de humedad ambiente.
    if (H >= (SetH + deltaH)){ // Si supera la banda permitida:
        humidificador_off();
        return ESPERA_MONITOREO;
    }

    return HUMIDIFICANDO;      // Continúa humidificando.
}

/* ------------------------------------------------------------- */
/* ------------------ ESTADO: VENTILANDO ------------------------ */
/* ------------------------------------------------------------- */

Estado ventilando(void){

    ventilador_on();           // Mantiene el ventilador encendido.

    /* Verificación de puerta abierta */
    if(lectura_puerta()){
        timer_start_seconds();
        return PUERTA_ABIERTA;
    }

    float CA = leer_calidad_aire();   // Reevaluación de calidad de aire.
    if (CA > (SetCA - deltaCA)){      // Si mejora lo suficiente:
        ventilador_off();
        return ESPERA_MONITOREO;
    }

    return VENTILANDO;                // Continúa la ventilación.
}

/* ------------------------------------------------------------- */
/* ------------------ ESTADO: PUERTA_ABIERTA -------------------- */
/* ------------------------------------------------------------- */

Estado puerta_abierta(void){

    led_puerta_on();           // Indica visualmente puerta abierta.
    led_ok_off();              // El sistema no está en estado estable.

    /* Si la puerta se cerró, se regresa a monitoreo */
    if (!lectura_puerta()){
        timer_stop_seconds();  // Detiene conteo de tiempo.
        led_puerta_off();
        return ESPERA_MONITOREO;
    }

    /* Si el tiempo supera el límite permitido:
       se fuerza el estado de emergencia */
    if (timer_get_seconds() >= T_EXCESO){
        return EMERGENCIA_APAGADO;
    }

    return PUERTA_ABIERTA;     // Permanece en este estado.
}

/* ------------------------------------------------------------- */
/* -------------- ESTADO: EMERGENCIA_APAGADO ------------------- */
/* ------------------------------------------------------------- */

Estado emergencia_apagado(void){

    /* Se apagan todos los actuadores para proteger el sistema */
    compresor_off();           // Apaga compresor.
    humidificador_off();       // Apaga humidificador.
    ventilador_off();          // Apaga ventilador.
    led_ok_off();              // Apaga indicador OK.

    led_puerta_on();           // Señaliza condición anómala.
    led_emergencia_on();       // Enciende indicador principal de emergencia.

    return EMERGENCIA_APAGADO; // Estado persistente hasta reinicio manual.
}
