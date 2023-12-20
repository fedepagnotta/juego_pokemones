#ifndef _MENU_JUEGO_H_
#define _MENU_JUEGO_H_

#include <stdlib.h>
#include <stdbool.h>

typedef struct menu menu_t;

typedef enum {
        MENU_OK,
        MENU_ERROR,
        MENU_INEXISTENTE,
        MENU_SALIR,
} RESULTADO_MENU;

/**
 * Crea un menu.
 * 
 * Devuelve el menu o NULL si no se pudo crear.
*/
menu_t *menu_crear();


/**
 * Agrega un comando al menú.
 * 
 * Cada comando está acompañado por una función, que se ejecuta
 * si se ingresa ese comando. La función recibe un void * como parámetro,
 * y devuelve un RESULTADO_MENU.
 * 
 * Devuelve el menu si se pudo agregar el comando o NULL en caso de
 * error.
*/
menu_t *menu_agregar_comando(menu_t *menu, char *comando,
                             RESULTADO_MENU (*funcion)(void *));

/**
 * Ejecuta la función de un comando. Recibe también un contexto que se pasa
 * como parámetro a la función del comando.
 * 
 * Devuelve el RESULTADO_MENU que retorna la función, o MENU_INEXISTENTE
 * si el comando no está disponible.
*/
RESULTADO_MENU menu_ejecutar_comando(menu_t *menu, char *comando,
                                     void *contexto);

/**
 * Destruye el menú y toda su memoria asociada.
*/
void menu_destruir(menu_t *menu);

#endif // _MENU_JUEGO_H_