#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H
#include <stdint.h>

#define DEFAULT_SIZE 16
#define MARGIN 3

typedef struct {
    uint32_t color;
    int x, y;       // Posición del hoyo
    int radius;     // Radio de colisión
} Circle;

typedef struct {
    uint32_t color;
    int x, y;       // Posición del jugador
    int width, height; // Dimensiones del jugador
} Rectangle;

// Pone un pixel en la posición (x, y) con el color dado
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

// Dibuja un carácter en la pantalla con el color especificado
void draw_char(char c, uint32_t color);

// Limpia el framebuffer con un color específico
void clear_framebuffer(uint32_t color);

// Limpia la pantalla con el color dado y resetea el cursor
void clear_screen(uint32_t color);

// Imprime un valor hexadecimal en la pantalla con el color especificado
void printHex(uint64_t value, uint32_t color);

// Imprime una cadena en la pantalla con el color especificado
void printString(const char *str, uint32_t color);


// Cambia el tamaño de la fuente (solo hay dos tamaños)
void changeFontSize();

// Obtiene el ancho de la pantalla
uint16_t get_screen_width();

// Obtiene el alto de la pantalla
uint16_t get_screen_height();

// Crea una nueva línea en la pantalla
void new_line();

// Desplaza la pantalla hacia arriba
void scroll_screen(uint32_t bg_color);

// Elimina el carácter anterior (backspace)
void delete_char(void);

// Define el color del texto
void set_text_color(uint32_t color);

// Obtiene el color del texto actual
uint32_t get_text_color(void);

// Imprime una cadena con un color específico
void printColoredString(const char *str, uint32_t color);

// Primitivas de dibujo adicionales
void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void draw_box(int x, int y, int width, int height, uint32_t color);
void draw_filled_box(int x, int y, int width, int height, uint32_t color);
void draw_text_with_background(const char *text, int x, int y, uint32_t text_color, uint32_t bg_color);

// Cursor de texto (opcional)
void draw_cursor();
void update_cursor_position(int x, int y);

// Control del cursor de texto: ocultar/mostrar
void set_cursor_visible();


#endif
