#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>


#include "driver.h"    // Capa de hardware: sensores y actuadores
#include "estados.h"   // Lógica de control: máquina de estados
#include "lcd.h"       // Capa de visualización

#define SetT_x100    (2200)   // Temperatura objetivo: 22.00 °C
#define deltaT_x100  (100)    // Banda de histéresis: ±1.00 °C
#define SetH_x100    (7000)   // Humedad objetivo: 70.00 %
#define deltaH_x100  (300)    // Banda: ±3.00 %
#define SetCA_x100   (8000)   // Calidad de aire objetivo: 80.00 %
#define deltaCA_x100 (300)    // Banda: ±3.00 %
#define T_EXCESO     (10)     // Tiempo máximo con puerta abierta: 10 segundos

#endif /* SYSTEM_H_ */