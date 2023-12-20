#include "menu_juego.h"
#include "hash.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct estructura_auxiliar_funcion_comando {
        RESULTADO_MENU (*funcion)(void *);
} funcion_aux_t;

/**
 * Crea un menu.
 * 
 * Devuelve el menu o NULL si no se pudo crear.
*/
menu_t *menu_crear()
{
        return (menu_t *)hash_crear(3);
}

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
                             RESULTADO_MENU (*funcion)(void *))
{
        funcion_aux_t *funcion_aux = calloc(1, sizeof(funcion_aux_t));
        if (!funcion_aux)
                return NULL;
        funcion_aux->funcion = funcion;
        return (menu_t *)hash_insertar((hash_t *)menu, comando,
                                       (void *)funcion_aux, NULL);
}

/**
 * Ejecuta la función de un comando. Recibe también un contexto que se pasa
 * como parámetro a la función del comando.
 * 
 * Devuelve el RESULTADO_MENU que retorna la función, o MENU_INEXISTENTE
 * si el comando no está disponible.
*/
RESULTADO_MENU menu_ejecutar_comando(menu_t *menu, char *comando,
                                     void *contexto)
{
        funcion_aux_t *funcion_aux = hash_obtener((hash_t *)menu, comando);
        if (!funcion_aux)
                return MENU_INEXISTENTE;
        RESULTADO_MENU (*funcion)(void *) = funcion_aux->funcion;
        return funcion(contexto);
}

/**
 * Destruye el menú y toda su memoria asociada.
*/
void menu_destruir(menu_t *menu)
{
        hash_destruir_todo((hash_t *)menu, free);
}

