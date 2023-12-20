#ifndef _JUGADOR_H_
#define _JUGADOR_H_

#include <stdlib.h>
#include <stdbool.h>
#include "juego.h"
#include "lista.h"
#include "ataque.h"

typedef struct jugador jugador_t;

/**
 * Crea un jugador y se le asigna el juego pasado por parámetro.
 * 
 * Devuelve el jugador, o NULL en caso de error.
*/
jugador_t *jugador_crear(juego_t *juego);

/**
 * Recibe tres nombres de pokemones y los carga en el jugador junto con
 * sus ataques.
 * 
 * Devuelve el jugador, o NULL en caso de error.
*/
JUEGO_ESTADO jugador_asignar_pokemones_y_ataques(jugador_t *jugador,
						 const char *nombre1,
						 const char *nombre2,
						 const char *nombre3);

/**
 * Devuelve el juego al que está asociado el jugador, o NULL en caso de error.
*/
juego_t *jugador_juego_asociado(jugador_t *jugador);

/**
 * Devuelve true si el jugador ya seleccionó sus pokemones.
*/
bool jugador_pokemones_seleccionados(jugador_t *jugador);

/**
 * Devuelve el puntaje del jugador o 0 en caso de error.
*/
int jugador_obtener_puntaje(jugador_t *jugador);

/**
 * Suma puntos al puntaje del jugador.
 * Devuelve el puntaje actual o 0 en caso de error.
*/
int jugador_sumar_puntos(jugador_t *jugador, unsigned puntos);

/**
 * Devuelve la lista de pokemones del jugador, o NULL en caso de error.
*/
lista_t *jugador_listar_pokemones(jugador_t *jugador);

/**
 * Devuelve la lista de ataques del pokemon que se encuentra en cierta
 * posición de la lista de pokemones del jugador (la posición pasada
 * por parámetro).
 * 
 * Devuelve NULL en caso de error.
*/
lista_t *jugador_listar_ataques_de_pokemon(jugador_t *jugador, size_t posicion);

/**
 * Quita un ataque de un pokemon del jugador. Se pasan por parámetro los
 * nombres de ambos.
 * 
 * Devuelve el ataque quitado o NULL en caso de error.
*/
const struct ataque *jugador_quitar_ataque_de_pokemon(jugador_t *jugador,
						      char *nombre_pokemon,
						      char *nombre_ataque);

/**
 * Intercambia el tercer pokemon de cada jugador (el jugador1 pasa a tener
 * como tercer pokemon el tercero del jugador2, y viceversa).
 * 
 * Devuelve true si se pudieron intercambiar o false en caso de error.
*/
JUEGO_ESTADO jugadores_intercambiar_tercer_pokemon(jugador_t *jugador1,
						   jugador_t *jugador2);

/**
 * Destruye el jugador y toda su memoria asociada.
*/
void jugador_destruir(jugador_t *jugador);

#endif // _JUGADOR_H_