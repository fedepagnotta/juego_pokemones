#include "adversario.h"
#include "juego.h"
#include "lista.h"
#include "pokemon.h"
#include <stdlib.h>
#include <string.h>
#include "ataque.h"
#include "jugador.h"

/**
 * Crea un adversario asignandole un listado de pokemon_t que puede utilizar
 * durante el juego.
 *
 * El adversario NO deberá modificar la lista ni liberarla.
 *
 * Devuelve NULL en caso de error.
 */
adversario_t *adversario_crear(lista_t *pokemon)
{
	return false;
}

/**
 * El adversario selecciona los pokemon para utilizar durante el juego. Modifica
 * los 3 punteros pasados para hacerlos apuntar a los nombres de los 3 pokemon
 * que quiere seleccionar.
 *
 * Los primeros 2 pokemon pertenecen al adversario, el tercer pokemon es para el
 * jugador.
 *
 * Devuelve true si pudo seleccionar o false en caso de error,
 */
bool adversario_seleccionar_pokemon(adversario_t *adversario, char **nombre1,
				    char **nombre2, char **nombre3)
{
	return juego_seleccionar_pokemon(
		       jugador_juego_asociado((jugador_t *)adversario),
		       JUGADOR2, *nombre1, *nombre2, *nombre3) == TODO_OK;
}

/**
 * Informa al adversario los pokemon seleccionados por el jugador. Los primeros
 * 2 pokemon pertenecen al jugador, el tercer pokemon completa los pokemon del
 * adversario.
 *
 * Devuelve true en caso de exito o false en caso de error.
 */
bool adversario_pokemon_seleccionado(adversario_t *adversario, char *nombre1,
				     char *nombre2, char *nombre3)
{
	return false;
}

/**
 * Devuelve la próxima jugada del adversario para el turno actual. La jugada
 * debe tener en cuenta las reglas del juego y adherirse a ellas (no se puede
 * utilizar el mismo movimientos dos veces, etc).
 *
 * Este método es el principal del adversario y donde se puede implementar la
 * lógica para que el juego sea mas interesante.
 */
jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
	int index_pokemon_seleccionado = rand() % 3;
	while (lista_vacia(jugador_listar_ataques_de_pokemon(
		(jugador_t *)adversario, (size_t)index_pokemon_seleccionado)))
		index_pokemon_seleccionado = rand() % 3;
	pokemon_t *pokemon = lista_elemento_en_posicion(
		jugador_listar_pokemones((jugador_t *)adversario),
		(size_t)index_pokemon_seleccionado);
	const struct ataque *ataque = lista_elemento_en_posicion(
		jugador_listar_ataques_de_pokemon(
			(jugador_t *)adversario,
			(size_t)index_pokemon_seleccionado),
		(size_t)rand() % 3);
	while (!ataque)
		ataque = lista_elemento_en_posicion(
			jugador_listar_ataques_de_pokemon(
				(jugador_t *)adversario,
				(size_t)index_pokemon_seleccionado),
			(size_t)rand() % 3);
	char nombre_ataque[20], nombre_pokemon[20];
	strcpy(nombre_ataque, ataque->nombre);
	strcpy(nombre_pokemon, pokemon_nombre(pokemon));
	jugada_t j = { .ataque = { 0 }, .pokemon = { 0 } };
	strcpy(j.ataque, nombre_ataque);
	strcpy(j.pokemon, nombre_pokemon);
	return j;
}

/**
 * Le informa al adversario cuál fue la última jugada del jugador. Esta
 * información puede ser utilizada para cambiar la estrategia del juego.
 */
void adversario_informar_jugada(adversario_t *a, jugada_t j)
{
}

/**
 * Destruye el adversario y libera la memoria reservada.
 */
void adversario_destruir(adversario_t *adversario)
{
}
