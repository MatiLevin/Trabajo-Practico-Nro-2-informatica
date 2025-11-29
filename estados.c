/* CORRECCIÓN APLICADA 
 *    "Hay un abuso del tipo de datos float. 
 
 CAMBIOS REALIZADOS:
    Eliminado TODO uso de `float`: no hay variables, constantes ni comparaciones flotantes.
   Se usan `int16_t` con escala ×100 para representar:
          °C × 100   22.50 °C = 2250
          % × 100    75.25 %  = 7525
 */

 #include "system.h"


/* -------------------- ESTADO: INICIO -------------------------- */

/*  Función: inicio()
    - Ejecuta acciones de reset seguro al arrancar.
    - Apaga todos los actuadores y señales.
    - Detiene temporizadores residuales.
    - Transición:  ESPERA_MONITOREO
*/
Estado inicio(void){
    led_emergencia_off();   // Apagar LED de emergencia (seguridad)
    compresor_off();        // Asegurar compresor apagado
    humidificador_off();    // Asegurar humidificador apagado
    ventilador_off();       // Asegurar ventilador apagado
    led_ok_off();           // Apagar indicador OK (se reevaluará después)
    led_puerta_off();       // Apagar LED de puerta
    timer_stop_seconds();   // Detener cualquier conteo previo
    return ESPERA_MONITOREO; // Ir a monitoreo pasivo
}


/* --------------- ESTADO: ESPERA_MONITOREO --------------------- */

/*  Función: espera_monitoreo()
    - Estado principal de vigilancia.
    - Prioridad 1: puerta abierta ? transición inmediata.
    - Luego, lee sensores y evalúa condiciones.
    - Si todo OK  enciende LED OK y permanece.
 */
Estado espera_monitoreo(void){
    /*  PRIORIDAD ALTA: detección de puerta abierta */
    if (lectura_puerta()){
        timer_start_seconds();   // Comienza conteo de tiempo
        return PUERTA_ABIERTA;   // Transición inmediata
    }


    int16_t T = leer_temperatura_x100();     // °C × 100 (ej: 1900 = 19.00°C)
    int16_t H = leer_humedad_x100();         // % × 100  (ej: 9000 = 90.00%)
    int16_t CA = leer_calidad_aire_x100();   // % × 100  (ej: 9000 = 90.00%)


    led_emergencia_off();
    led_ok_off();
    compresor_off();
    humidificador_off();
    ventilador_off();
    led_puerta_off();

    /*  EVALUACIÓN: temperatura alta */
    if (T > (SetT_x100 + deltaT_x100)) {  // Ej: 2301 > 2300  activar
        compresor_on();
        return ENFRIANDO;
    }

    /*  EVALUACIÓN: humedad baja */
    if (H < (SetH_x100 - deltaH_x100)) {  // Ej: 6699 < 6700  activar
        humidificador_on();
        return HUMIDIFICANDO;
    }

    /*  EVALUACIÓN: calidad de aire baja */
    if (CA < (SetCA_x100 - deltaCA_x100)) {  // Ej: 7699 < 7700  activar
        ventilador_on();
        return VENTILANDO;
    }

    /*  TODO DENTRO DE RANGOS  sistema estable */
    led_ok_on();  // Indicador visual de funcionamiento correcto
    return ESPERA_MONITOREO;  // Permanecer en monitoreo
}


/* ------------------ ESTADO: ENFRIANDO ------------------------- */

/*  Función: enfriando()
    - Mantiene compresor encendido.
    - Si temperatura baja lo suficiente  apaga y vuelve a monitoreo.
*/
Estado enfriando(void){
    compresor_on();  // Mantener activo

    /*  Control prioritario de puerta */
    if (lectura_puerta()){
        timer_start_seconds();
        return PUERTA_ABIERTA;
    }

    int16_t T = leer_temperatura_x100();
    /*  Apagar cuando T < límite inferior */
    if (T < (SetT_x100 - deltaT_x100)) {  // Ej: 2099 < 2100  apagar
        compresor_off();
        return ESPERA_MONITOREO;
    }

    return ENFRIANDO;  // Continuar enfriando
}


/* ---------------- ESTADO: HUMIDIFICANDO ----------------------- */

/*  Función: humidificando()
    - Mantiene humidificador encendido.
    - Si humedad supera límite superior  apaga y vuelve a monitoreo.
 */
Estado humidificando(void){
    humidificador_on();

    if (lectura_puerta()){
        timer_start_seconds();
        return PUERTA_ABIERTA;
    }

    int16_t H = leer_humedad_x100();
    if (H >= (SetH_x100 + deltaH_x100)) {  // Ej: 7300 >= 7300  apagar
        humidificador_off();
        return ESPERA_MONITOREO;
    }

    return HUMIDIFICANDO;
}


/* ------------------ ESTADO: VENTILANDO ------------------------ */

/*  Función: ventilando()
     - Mantiene ventilador encendido.
   - Si calidad de aire mejora ? apaga y vuelve a monitoreo.
 */
Estado ventilando(void){
    ventilador_on();

    if (lectura_puerta()){
        timer_start_seconds();
        return PUERTA_ABIERTA;
    }

    int16_t CA = leer_calidad_aire_x100();
    if (CA > (SetCA_x100 + deltaCA_x100)) {  // Ej: 8301 > 8300 ? apagar
        ventilador_off();
        return ESPERA_MONITOREO;
    }

    return VENTILANDO;
}


/* ------------------ ESTADO: PUERTA_ABIERTA -------------------- */

/*  Función: puerta_abierta()
    - Señaliza visualmente puerta abierta.
    - Si se cierra  vuelve a monitoreo.
    - Si tiempo excede T_EXCESO  emergencia.
 */
Estado puerta_abierta(void){
    led_puerta_on();   // Enciende LED de puerta (señal visual)
    led_ok_off();      // Sistema no estable

    /*  ¿Se cerró? */
    if (!lectura_puerta()){
        timer_stop_seconds();  // Detiene conteo
        led_puerta_off();      // Apaga señal
        return ESPERA_MONITOREO;
    }

    /*  ¿Tiempo excesivo? */
    if (timer_get_seconds() >= T_EXCESO){
        return EMERGENCIA_APAGADO;
    }

    return PUERTA_ABIERTA;
}


/* -------------- ESTADO: EMERGENCIA_APAGADO ------------------- */

/*  Función: emergencia_apagado()
 *    - Estado de fallo crítico.
 *    - Apaga TODO.
 *    - Enciende señal de emergencia.
 *    - Permanece hasta reset manual (no hay transición automática).
 */
Estado emergencia_apagado(void){
    compresor_off();
    humidificador_off();
    ventilador_off();
    led_ok_off();
    led_puerta_on();        // Parpadeo no implementado, pero se enciende
    led_emergencia_on();    // Señal principal de alarma
    return EMERGENCIA_APAGADO;  // Estado estable (requiere intervención humana)
}