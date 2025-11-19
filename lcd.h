#ifndef LCD_H
#define LCD_H
#include <avr/io.h>
#include <util/delay.h>


/* ------------------------------------------------------------------------- */
/* ---------------------- PROTOTIPOS DE FUNCIONES LCD ---------------------- */
/* ------------------------------------------------------------------------- */

/* 
   Inicializa el display LCD.  
   Configura los pines, envía la secuencia de comandos obligatorios
   y prepara el módulo para trabajar en 4 bits 
*/
void lcd_init(void);

/* 
   Envía un comando al LCD.  
   
*/
void lcd_cmd(unsigned char cmd);

/* 
   Envía un dato al LCD.  
   
*/
void lcd_data(unsigned char data);

/* 
   Imprime una cadena de caracteres completa en el LCD.  
  
*/
void lcd_print(char *str);

/* 
   Ubica el cursor en la posición deseada dentro del display.  
   'x' representa la columna, 'y' representa la fila.
   Permite controlar exactamente dónde escribir el siguiente texto.
*/
void lcd_gotoxy(unsigned char x, unsigned char y);

#endif
/* 
   Fin de la directiva de inclusión condicional.  
   Marca el cierre del archivo de encabezado.
*/
