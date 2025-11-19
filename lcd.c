/* -------------------------------------------------------------------------
   lcd.c - Implementación del controlador para display LCD en modo 4 bits
   ------------------------------------------------------------------------- */

#include "lcd.h" 

#define LCD_PORT PORTB
/* 
   Define el puerto físico del microcontrolador al cual está conectado el LCD.
   Aquí se utiliza PORTB como bus de datos y señales de control.
*/

#define LCD_DDR  DDRB
/* 
   Define el registro de dirección del puerto (DDR) asociado al LCD.
   Permite configurar cada pin como entrada o salida.
*/

#define RS PB0
/* 
   Pin RS (Register Select). Selecciona entre comando (0) o dato (1).
   PB0 del puerto B se asigna como línea RS.
*/

#define EN PB1
/* 
   Pin EN (Enable). Genera el pulso que permite al LCD capturar los datos.
   PB1 se utiliza como línea EN.
*/

#define D4 PB2
#define D5 PB3
#define D6 PB4
#define D7 PB5
/* 
   Líneas de datos para modo 4 bits.
*/

static void lcd_send_nibble(uint8_t nibble){

    /* Escribe bit 0 del nibble en D4 */
    if (nibble & 0x01) LCD_PORT |=  (1 << D4);
    else               LCD_PORT &= ~(1 << D4);

    /* Escribe bit 1 del nibble en D5 */
    if (nibble & 0x02) LCD_PORT |=  (1 << D5);
    else               LCD_PORT &= ~(1 << D5);

    /* Escribe bit 2 del nibble en D6 */
    if (nibble & 0x04) LCD_PORT |=  (1 << D6);
    else               LCD_PORT &= ~(1 << D6);

    /* Escribe bit 3 del nibble en D7 */
    if (nibble & 0x08) LCD_PORT |=  (1 << D7);
    else               LCD_PORT &= ~(1 << D7);

    /* Se genera pulso de habilitación para capturar los datos */
    LCD_PORT |= (1 << EN);   // flanco ascendente
    _delay_us(1);            // tiempo mínimo requerido por el LCD
    LCD_PORT &= ~(1 << EN);  // flanco descendente
    _delay_us(200);          // tiempo de espera entre envíos
}

/* ------------------------------------------------------------------------- */
void lcd_cmd(unsigned char cmd){
    LCD_PORT &= ~(1 << RS);      // RS = 0  modo comando
    lcd_send_nibble(cmd >> 4);   // Enviar nibble alto
    lcd_send_nibble(cmd & 0x0F); // Enviar nibble bajo
    _delay_ms(2);                // tiempo de procesamiento
}

/* -------------------------------------------------------------------------*/
void lcd_data(unsigned char data){
    LCD_PORT |= (1 << RS);       // RS = 1  modo datos
    lcd_send_nibble(data >> 4);  // Enviar nibble alto
    lcd_send_nibble(data & 0x0F);// Enviar nibble bajo
    _delay_ms(2);                // retardo para procesamiento
}

/* -------------------------------------------------------------------------*/
void lcd_init(void){

    /* Configurar los pines de control y datos como salidas */
    LCD_DDR |= (1<<RS)|(1<<EN)|(1<<D4)|(1<<D5)|(1<<D6)|(1<<D7);

    _delay_ms(20);  // Espera inicial obligatoria para estabilización del LCD

    /* Secuencia de arranque en 4 bits (orden específica del datasheet) */
    lcd_send_nibble(0x03);
    _delay_ms(5);

    lcd_send_nibble(0x03);
    _delay_us(150);

    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02);   // Indicar modo 4 bits

    /* Configuraciones del LCD */
    lcd_cmd(0x28);  // LCD de 2 líneas, matriz 5x7, modo 4 bits
    lcd_cmd(0x0C);  // Display ON, cursor OFF, sin parpadeo
    lcd_cmd(0x06);  // Auto-incremento del cursor
    lcd_cmd(0x01);  // Limpiar pantalla
    _delay_ms(2);
}

/* -------------------------------------------------------------------------
   lcd_print: Imprime una cadena completa de caracteres
   Recorre el string hasta encontrar el terminador nulo '\0'
   ------------------------------------------------------------------------- */
void lcd_print(char *str){
    while(*str)          // mientras no llegue al fin del string
        lcd_data(*str++);// imprimir carácter actual y avanzar
}


void lcd_gotoxy(unsigned char x, unsigned char y){
    unsigned char pos = (y == 0) ? 0x80 + x : 0xC0 + x;
    lcd_cmd(pos);        // enviar comando para mover cursor
}
