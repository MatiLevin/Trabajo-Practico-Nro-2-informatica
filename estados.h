#ifndef ESTADOS_H                                             
#define ESTADOS_H                                             

/*
 * Definición del tipo enumerado 'Estado'.
 * Este enum representa todos los estados posibles dentro de la máquina de estados
 * del sistema de control (climatización, seguridad y ventilación).
 *
 * Cada identificador simboliza un estado funcional bien definido que permitirá
 * organizar las transiciones y acciones asociadas en el código principal.
 */
typedef enum {
    INICIO,                     // Estado inicial del sistema: configuración base y diagnóstico.
    ESPERA_MONITOREO,           // Estado pasivo donde se monitorean sensores.
    ENFRIANDO,                  // Estado donde se activa el sistema de enfriamiento.
    HUMIDIFICANDO,              // Estado encargado de aumentar la humedad ambiente.
    VENTILANDO,                 // Estado destinado a renovar o circular aire.
    PUERTA_ABIERTA,             // Estado que detecta la apertura de la puerta del recinto.
    EMERGENCIA_APAGADO          // Estado crítico para proteger el sistema ante fallas o peligros.
} Estado;

/*
 * A continuación se declaran los prototipos de las funciones asociadas
 * a cada uno de los estados definidos anteriormente.
 *
 * Cada función debe implementar la lógica interna del estado y retornar
 * el próximo estado al cual debe transicionar el sistema.
 */

Estado inicio(void);                   // Gestiona el estado INICIO.
Estado espera_monitoreo(void);         // Gestiona el estado ESPERA_MONITOREO.
Estado enfriando(void);                // Gestiona el estado ENFRIANDO.
Estado ventilando(void);               // Función correspondiente al estado VENTILANDO
Estado puerta_abierta(void);           // Gestiona el estado PUERTA_ABIERTA.
Estado emergencia_apagado(void);       // Gestiona el estado EMERGENCIA_APAGADO.

#endif                                 
                                       // Fin de la directiva de inclusión condicional.
