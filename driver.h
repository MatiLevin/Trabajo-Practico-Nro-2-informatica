#ifndef DRIVER_H                
#define DRIVER_H                
#include <stdbool.h>            
#include <stdint.h>             

 /* Inicializaciones ---------------------------------------------------*/
                                // Sección dedicada a la declaración de funciones de inicialización
                                // de periféricos y configuración hardware requerida por el driver.

void driver_init(void);         // Prototipo de la función de inicialización del driver.
                                // Responsabilidad: configurar ADC, puertos GPIO, timers, interrupciones
                                
 /* Lecturas de sensores ------------------------------------------------*/
                                // Declaración de las funciones encargadas de leer los sensores físicos.
                                // Se definen como funciones que retornan valores en unidades físicas
                                // (por ejemplo °C, % de humedad, unidades relativas de calidad de aire).

float leer_temperatura(void);   // Lee la temperatura medida por el sensor "LM35" conectado al ADC.
                                // Convención: retorna la temperatura en grados Celsius (°C).
                                // la función convirte la lectura del ADC
                                // a temperatura aplicando la escala y calibración correspondiente.

float leer_humedad(void);       // Lee la humedad relativa desde el sensor ,tambien utilizando el lm35 .
                                // Convención: retorna humedad en porcentaje (%).
                                // la función convirte la lectura del ADC
                                // a un valor de humedad porcentual representado en una escala de 0 a 100
			        // aplicando la escala y calibración correspondiente.

float leer_calidad_aire(void);  // Lee un valor representativo de la calidad del aire (CO2, VOCs, etc.)
                                // Convención: retorna un valor de calidad de aire  en porcentaje (%) 
				// la función convirte la lectura del ADC
                                // a un valor de calidad de aire porcentual representado en una escala de 0 a 100
			        // aplicando la escala y calibración correspondiente.

bool lectura_puerta(void);      // Lee el estado del interruptor de puerta (SW1).
                                // Convención: retorna true si la puerta está abierta, false si está cerrada.
                                // Comentario: el sensor puede ser un switch normalmente cerrado/abierto
                                // y la función debe adaptar la lógica de lectura según el hardware.

 /* Actuadores (representados por LEDs en PC0..PC4) ---------------------*/
                                // Funciones para controlar actuadores. En la placa de prototipo
                                // se mapean a salidas GPIO (LEDs) según el comentario, pero en
                                // un sistema real controlarían relés o drivers de potencia.

void led_ok_on(void);           // Enciende el LED de "OK" (mapeado a PC0).
                                // Uso: indicar que el sistema está en funcionamiento normal.

void led_ok_off(void);          // Apaga el LED de "OK" (PC0).
                                // Uso: indicar salida de condición "OK" o apagado del sistema.

void compresor_on(void);        // Activa el compresor (mapeado a PC1).
                                // En la implementación real: habilita la salida que alimenta
                                // al relé o driver del compresor para iniciar el enfriamiento.

void compresor_off(void);       // Desactiva el compresor (PC1).
                                // Detiene la acción de refrigeración y devuelve la salida a estado inactivo.

void humidificador_on(void);    // Activa el humidificador (PC2).
                                // Acción contraria: el circuito de salida proporciona alimentación
                                // al humidificador o a su driver.

void humidificador_off(void);   // Desactiva el humidificador (PC2).
                                // Garantiza que no haya aporte adicional de humedad.

void ventilador_on(void);       // Activa el ventilador (PC3).
                                // Uso: mejorar la calidad del aire o extraer aire contaminado.

void ventilador_off(void);      // Desactiva el ventilador (PC3).

void led_puerta_on(void);       // Enciende el LED indicador de puerta abierta (PC4).
                                // Este LED sirve como señal visual de que la puerta está en estado abierto.

void led_puerta_off(void);      // Apaga el LED indicador de puerta (PC4).

 /* Temporizador para PUERTA_ABIERTA ----------------------------------*/
                                // Prototipos relacionados con la medición del tiempo que la puerta
                                // permanece abierta. El temporizador se usa en el estado PUERTA_ABIERTA.

void timer_start_seconds(void); // Inicia o reinicia el temporizador que cuenta segundos.
                                // Debe dejar el contador interno en cero y comenzar la cuenta ascendente.
                                // Puede usar un timer hardware con prescaler adecuado para medir segundos.

void timer_stop_seconds(void);  // Detiene el temporizador. Útil para pausar la medición cuando
                                // la puerta se cierra y se evalúa si hubo exceso de tiempo.

uint32_t timer_get_seconds(void); /* retorna segundos desde start */
                                // Devuelve el valor del temporizador en segundos transcurridos
                                // desde la última llamada a timer_start_seconds().
                                
 /* Señalización de emergencia ----------------------------------------*/
                                // Controles específicos para indicar estado de emergencia en el tablero.

void led_emergencia_on(void);   // Enciende el LED de emergencia (p.ej. PD4).
                                // Debe utilizarse cuando el sistema entra en EMERGENCIA_APAGADO.

void led_emergencia_off(void);  // Apaga el LED de emergencia (PD4).

#endif                         
