#include <stdlib.h>
#include <stdbool.h>
#include "juego.h"
#include "lista.h"
#include "ataque.h"
#include "jugador.h"
#include "pokemon.h"
#include "comparadores.h"
#include <stdio.h>

struct jugador {
	juego_t *juego;
	lista_t *pokemones;
	lista_t *pokemon1_ataques_disponibles;
	lista_t *pokemon2_ataques_disponibles;
	lista_t *pokemon3_ataques_disponibles;
	unsigned puntaje;
};

/**
 * Recibe un jugador y crea todas sus listas de pokemones y ataques.
 * 
 * Devuelve true si se pudo crear todo o false en caso de error.
*/
bool crear_listas_jugador(jugador_t *jugador)
{
	jugador->pokemones = lista_crear();
	jugador->pokemon1_ataques_disponibles = lista_crear();
	jugador->pokemon2_ataques_disponibles = lista_crear();
	jugador->pokemon3_ataques_disponibles = lista_crear();

	if (!jugador->pokemones || !jugador->pokemon1_ataques_disponibles ||
	    !jugador->pokemon2_ataques_disponibles ||
	    !jugador->pokemon3_ataques_disponibles) {
		lista_destruir(jugador->pokemones);
		lista_destruir(jugador->pokemon1_ataques_disponibles);
		lista_destruir(jugador->pokemon2_ataques_disponibles);
		lista_destruir(jugador->pokemon3_ataques_disponibles);
		return false;
	}

	return true;
}

/**
 * Crea un jugador y se le asigna el juego pasado por parámetro.
 * 
 * Devuelve el jugador, o NULL en caso de error.
*/
jugador_t *jugador_crear(juego_t *juego)
{
	jugador_t *jugador = calloc(1, sizeof(jugador_t));
	if (!jugador)
		return NULL;
	jugador->juego = juego;
	if (!crear_listas_jugador(jugador)) {
		free(jugador);
		return NULL;
	}
	return jugador;
}

/**
 * Recibe una lista de pokemones y tres pokemones, y los inserta en la lista.
 * 
 * Devuelve TODO_OK si se pudieron insertar o ERROR_GENERAL si no se pudo.
*/
JUEGO_ESTADO insertar_pokemones(lista_t *lista_pokemones, pokemon_t *pokemon1,
				pokemon_t *pokemon2, pokemon_t *pokemon3)
{
	if (!lista_insertar(lista_pokemones, pokemon1)) {
		return ERROR_GENERAL;
	}
	if (!lista_insertar(lista_pokemones, pokemon2)) {
		lista_quitar(lista_pokemones);
		return ERROR_GENERAL;
	}
	if (!lista_insertar(lista_pokemones, pokemon3)) {
		lista_quitar(lista_pokemones);
		lista_quitar(lista_pokemones);
		return ERROR_GENERAL;
	}
	return TODO_OK;
}

/**
 * Recibe un juego, una lista enlazada y tres nombres de pokemones.
 * 
 * Si los pokemones están disponibles en el juego, los inserta en la lista.
 * 
 * Devuelve POKEMON_INEXISTENTE si alguno de los pokemones no está disponible,
 * ERROR_GENERAL en caso de que no se pueda insertar el pokemon en la lista o
 * TODO_OK si se pudieron insertar todos los pokemones.
*/
JUEGO_ESTADO llenar_lista_de_pokemones(juego_t *juego, lista_t *lista_pokemones,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
	pokemon_t *poke1 = lista_buscar_elemento(juego_listar_pokemon(juego),
						 comparador_nombres_pokemon,
						 (void *)nombre1),
		  *poke2 = lista_buscar_elemento(juego_listar_pokemon(juego),
						 comparador_nombres_pokemon,
						 (void *)nombre2),
		  *poke3 = lista_buscar_elemento(juego_listar_pokemon(juego),
						 comparador_nombres_pokemon,
						 (void *)nombre3);
	if (!poke1 || !poke2 || !poke3) {
		return POKEMON_INEXISTENTE;
	}
	return insertar_pokemones(lista_pokemones, poke1, poke2, poke3);
}

/**
 * Recibe un ataque y una lista enlazada.
 * 
 * Inserta el ataque en la lista. 
*/
void insertar_ataque_en_lista(const struct ataque *ataque, void *lista_ataques)
{
	lista_insertar((lista_t *)lista_ataques, (void *)ataque);
}

/**
 * Recibe una lista simplemente enlazada y le quita todos sus elementos.
*/
void vaciar_lista(lista_t *lista)
{
	size_t i = 0, tamanio = lista_tamanio(lista);
	while (i < tamanio) {
		lista_quitar(lista);
		i++;
	}
}

/**
 * Recibe un jugador y un índice entre 0 y 2, que indica la posición de un
 * pokemon en la lista de pokemones disponibles del jugador.
 * 
 * Llena la lista de ataques disponibles del pokemon correspondiente al índice.
*/
JUEGO_ESTADO llenar_lista_ataques_de_pokemon(jugador_t *jugador,
					     size_t indice_pokemon)
{
	pokemon_t *pokemon =
		lista_elemento_en_posicion(jugador->pokemones, indice_pokemon);
	lista_t *ataques_de_pokemon =
		jugador_listar_ataques_de_pokemon(jugador, indice_pokemon);
	con_cada_ataque(pokemon, insertar_ataque_en_lista, ataques_de_pokemon);
	if (lista_tamanio(ataques_de_pokemon) < 3) {
		vaciar_lista(ataques_de_pokemon);
		return ERROR_GENERAL;
	}
	return TODO_OK;
}

/**
 * Recibe un juego y un jugador.
 * 
 * Llena las tres listas de ataques disponibles del jugador, una por cada
 * pokemon.
*/
JUEGO_ESTADO llenar_listas_de_ataques(jugador_t *jugador)
{
	for (size_t i = 0; i < 3; i++)
		if (llenar_lista_ataques_de_pokemon(jugador, i) != TODO_OK)
			return ERROR_GENERAL;
	return TODO_OK;
}

/**
 * Recibe tres nombres de pokemones y los carga en el jugador junto con
 * sus ataques.
 * 
 * Devuelve TODO_OK si se pudo asignar correctamente, POKEMON_INEXISTENTE si
 * algún nombre no está disponible en el juego o ERROR_GENERAL en caso de error
 * de memoria u otros.
*/
JUEGO_ESTADO jugador_asignar_pokemones_y_ataques(jugador_t *jugador,
						 const char *nombre1,
						 const char *nombre2,
						 const char *nombre3)
{
	JUEGO_ESTADO estado = llenar_lista_de_pokemones(
		jugador->juego, jugador->pokemones, nombre1, nombre2, nombre3);
	if (estado != TODO_OK)
		return estado;
	if (llenar_listas_de_ataques(jugador) != TODO_OK)
		return ERROR_GENERAL;
	return TODO_OK;
}

/**
 * Devuelve el juego al que está asociado el jugador, o NULL en caso de error.
*/
juego_t *jugador_juego_asociado(jugador_t *jugador)
{
	if (!jugador)
		return NULL;
	return jugador->juego;
}

/**
 * Devuelve true si el jugador ya seleccionó sus pokemones.
*/
bool jugador_pokemones_seleccionados(jugador_t *jugador)
{
	return !lista_vacia(jugador->pokemones);
}

/**
 * Devuelve el puntaje del jugador o 0 en caso de error.
*/
int jugador_obtener_puntaje(jugador_t *jugador)
{
	if (!jugador)
		return 0;
	return (int)jugador->puntaje;
}

/**
 * Suma puntos al puntaje del jugador.
 * Devuelve el puntaje actual o 0 en caso de error.
*/
int jugador_sumar_puntos(jugador_t *jugador, unsigned puntos)
{
	if (!jugador)
		return jugador_obtener_puntaje(jugador);
	jugador->puntaje = jugador->puntaje + puntos;
	return jugador_obtener_puntaje(jugador);
}

/**
 * Devuelve la lista de pokemones del jugador, o NULL en caso de error.
*/
lista_t *jugador_listar_pokemones(jugador_t *jugador)
{
	return jugador->pokemones;
}

/**
 * Devuelve la lista de ataques del pokemon que se encuentra en cierta
 * posición de la lista de pokemones del jugador (la posición pasada
 * por parámetro).
 * 
 * Devuelve NULL en caso de error.
*/
lista_t *jugador_listar_ataques_de_pokemon(jugador_t *jugador, size_t posicion)
{
	if (!jugador)
		return NULL;
	if (posicion == 0)
		return jugador->pokemon1_ataques_disponibles;
	if (posicion == 1)
		return jugador->pokemon2_ataques_disponibles;
	return jugador->pokemon3_ataques_disponibles;
}

/**
 * Quita un ataque de un pokemon del jugador. Se pasan por parámetro los
 * nombres de ambos.
 * 
 * Devuelve el ataque quitado o NULL en caso de error.
*/
const struct ataque *jugador_quitar_ataque_de_pokemon(jugador_t *jugador,
						      char *nombre_pokemon,
						      char *nombre_ataque)
{
	size_t posicion = lista_con_cada_elemento(
		jugador->pokemones, nombres_pokemon_son_diferentes,
		nombre_pokemon);
	if (posicion == 3)
		return NULL;
	lista_t *lista_ataques =
		jugador_listar_ataques_de_pokemon(jugador, posicion);
	posicion = lista_con_cada_elemento(
		lista_ataques, nombres_ataque_son_diferentes, nombre_ataque);
	if (posicion == lista_tamanio(lista_ataques))
		return NULL;
	return lista_quitar_de_posicion(lista_ataques, posicion);
}

/**
 * Recibe un jugador y quita todos los elementos de sus listas de pokemones
 * y ataques.
*/
void vaciar_listas_jugador(jugador_t *jugador)
{
	vaciar_lista(jugador->pokemones);
	vaciar_lista(jugador->pokemon1_ataques_disponibles);
	vaciar_lista(jugador->pokemon2_ataques_disponibles);
	vaciar_lista(jugador->pokemon3_ataques_disponibles);
}

/**
 * Intercambia el tercer pokemon de cada jugador (el jugador1 pasa a tener
 * como tercer pokemon el tercero del jugador2, y viceversa).
 * 
 * Devuelve true si se pudieron intercambiar o false en caso de error.
*/
JUEGO_ESTADO jugadores_intercambiar_tercer_pokemon(jugador_t *jugador1,
						   jugador_t *jugador2)
{
	if (!lista_insertar(jugador1->pokemones,
			    lista_quitar(jugador2->pokemones))) {
		vaciar_listas_jugador(jugador1);
		vaciar_listas_jugador(jugador2);
		return ERROR_GENERAL;
	}
	if (!lista_insertar(jugador2->pokemones,
			    lista_quitar_de_posicion(jugador1->pokemones, 2))) {
		vaciar_listas_jugador(jugador1);
		vaciar_listas_jugador(jugador2);
		return ERROR_GENERAL;
	};
	lista_t *lista_ataques_aux = jugador1->pokemon3_ataques_disponibles;
	jugador1->pokemon3_ataques_disponibles =
		jugador2->pokemon3_ataques_disponibles;
	jugador2->pokemon3_ataques_disponibles = lista_ataques_aux;
	return TODO_OK;
}

/**
 * Recibe un jugador y libera toda su memoria asociada, sin liberar al jugador.
*/
void destruir_listas_jugador(jugador_t *jugador)
{
	lista_destruir(jugador->pokemones);
	lista_destruir(jugador->pokemon1_ataques_disponibles);
	lista_destruir(jugador->pokemon2_ataques_disponibles);
	lista_destruir(jugador->pokemon3_ataques_disponibles);
}

/**
 * Destruye el jugador y toda su memoria asociada.
*/
void jugador_destruir(jugador_t *jugador)
{
	destruir_listas_jugador(jugador);
	free(jugador);
}