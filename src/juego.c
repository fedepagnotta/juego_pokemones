#include "juego.h"
#include "lista.h"
#include "tipo.h"
#include <stdbool.h>
#include "pokemon.h"
#include "ataque.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jugador.h"
#include "adversario.h"
#include "comparadores.h"

struct juego {
	informacion_pokemon_t *pokemones_disponibles;
	lista_t *lista_pokemones;
	jugador_t *jugador1;
	jugador_t *jugador2;
	int turnos_jugados;
};

/**
 * Crea un juego. Devuelve NULL en caso de error.
 */
juego_t *juego_crear()
{
	juego_t *juego = calloc(1, sizeof(juego_t));
	juego->lista_pokemones = lista_crear();
	if (!juego->lista_pokemones) {
		juego_destruir(juego);
		return NULL;
	}
	juego->jugador1 = jugador_crear(juego);
	if (!juego->jugador1) {
		juego_destruir(juego);
		return NULL;
	}
	juego->jugador2 = jugador_crear(juego);
	return juego;
}

/**
 * Recibe un pokemon y una lista enlazada. Inserta el pokemon en la lista.
*/
void insertar_pokemon_en_lista(pokemon_t *pokemon, void *lista_pokemones)
{
	lista_insertar(lista_pokemones, pokemon);
}

/**
 * Recibe una lista simplemente enlazada y le quita todos sus elementos.
*/
void lista_vaciar(lista_t *lista)
{
	size_t i = 0, tamanio = lista_tamanio(lista);
	while (i < tamanio) {
		lista_quitar(lista);
		i++;
	}
}

/**
 * Recibe un juego.
 * 
 * Llena la lista de pokemones del juego con los pokemones disponibles.
 * 
 * Devuelve TODO_OK si se pudo llenar o ERROR_GENERAL en caso de error.
*/
JUEGO_ESTADO juego_llenar_lista_pokemones(juego_t *juego)
{
	lista_t *lista_pokemones = juego->lista_pokemones;
	con_cada_pokemon(juego->pokemones_disponibles,
			 insertar_pokemon_en_lista, lista_pokemones);
	int cantidad_pokemon = pokemon_cantidad(juego->pokemones_disponibles);
	if (lista_tamanio(lista_pokemones) < cantidad_pokemon) {
		lista_vaciar(lista_pokemones);
		pokemon_destruir_todo(juego->pokemones_disponibles);
		return ERROR_GENERAL;
	}
	return TODO_OK;
}

/**
 * Carga un archivo de pokemon (con el formato del TP1).
 *
 * Devuelve POKEMON_INSUFICIENTES si no hay pokemon suficientes como para jugar.
 *
 * Devuelve TODO_OK si no hay problemas o ERROR_GENERAL para cualquier otro
 * error.
 */
JUEGO_ESTADO juego_cargar_pokemon(juego_t *juego, char *archivo)
{
	if (!juego)
		return ERROR_GENERAL;
	juego->pokemones_disponibles = pokemon_cargar_archivo(archivo);
	if (!juego->pokemones_disponibles) {
		return ERROR_GENERAL;
	}
	if (pokemon_cantidad(juego->pokemones_disponibles) < 4) {
		pokemon_destruir_todo(juego->pokemones_disponibles);
		return POKEMON_INSUFICIENTES;
	}
	return juego_llenar_lista_pokemones(juego);
}

/**
 * Devuelve una lista con todos los pokemon_t* disponibles para jugar. Esta lista
 * no debe ser modificada ni liberada por el usuario (será liberada por el juego
 * cuando este sea destruido).
 *
 * Devuelve la lista o NULL en caso de error.
 */
lista_t *juego_listar_pokemon(juego_t *juego)
{
	if (!juego)
		return NULL;
	return juego->lista_pokemones;
}

/**
 * Recibe tres nombres de pokemones.
 * 
 * Devuelve true si alguno está repetido o false si son diferentes entre sí.
*/
bool pokemon_repetido(const char *nombre1, const char *nombre2,
		      const char *nombre3)
{
	if (strcmp(nombre1, nombre2) == 0 || strcmp(nombre1, nombre3) == 0 ||
	    strcmp(nombre2, nombre3) == 0)
		return true;
	return false;
}

/**
 * Informa los 3 pokemon seleccionados por el jugador. Cada jugador puede
 * seleccionar sus pokemon una sola vez.
 *
 * Retorna TODO_OK o ERROR_GENERAL en caso de error.
 *
 * Opcionalmente puede devolver POKEMON_REPETIDO o POKEMON_INEXISTENTE.
 */
JUEGO_ESTADO juego_seleccionar_pokemon(juego_t *juego, JUGADOR jugador,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
	if (juego_finalizado(juego))
		return ERROR_GENERAL;
	jugador_t *jugador_seleccionado = NULL;
	if (jugador == JUGADOR1)
		jugador_seleccionado = juego->jugador1;
	else
		jugador_seleccionado = juego->jugador2;
	if (jugador_pokemones_seleccionados(jugador_seleccionado))
		return ERROR_GENERAL;
	if (!nombre1 || !nombre2 || !nombre3)
		return POKEMON_INEXISTENTE;
	if (pokemon_repetido(nombre1, nombre2, nombre3))
		return POKEMON_REPETIDO;
	JUEGO_ESTADO estado = jugador_asignar_pokemones_y_ataques(
		jugador_seleccionado, nombre1, nombre2, nombre3);
	if (estado != TODO_OK)
		return estado;
	if (jugador_pokemones_seleccionados(juego->jugador1) &&
	    jugador_pokemones_seleccionados(juego->jugador2))
		return jugadores_intercambiar_tercer_pokemon(juego->jugador1,
							     juego->jugador2);
	return TODO_OK;
}

/**
 * Recibe un juego, un jugador y un nombre de pokemon.
 * 
 * Si el jugador tiene ese pokemon disponible, devuelve el pokemon. Si no
 * lo está, devuelve NULL.
*/
pokemon_t *inicializar_pokemon(juego_t *juego, JUGADOR jugador,
			       char *nombre_pokemon)
{
	lista_t *lista_pokemones = NULL;
	if (jugador == JUGADOR1)
		lista_pokemones = jugador_listar_pokemones(juego->jugador1);
	else
		lista_pokemones = jugador_listar_pokemones(juego->jugador2);
	return lista_buscar_elemento(
		lista_pokemones, comparador_nombres_pokemon, nombre_pokemon);
}

/**
 * Recibe un ataque y el pokemon al que se atacó.
 * 
 * Devuelve el resultado del ataque.
*/
RESULTADO_ATAQUE jugador_resultado_ataque(const struct ataque *ataque,
					  pokemon_t *pokemon_atacado)
{
	enum TIPO tipo_pokemon = pokemon_tipo(pokemon_atacado),
		  tipo_ataque = ataque->tipo;
	if (tipo_ataque == NORMAL || tipo_pokemon == NORMAL) {
		return ATAQUE_REGULAR;
	} else if (tipo_ataque == FUEGO) {
		if (tipo_pokemon == PLANTA)
			return ATAQUE_EFECTIVO;
		if (tipo_pokemon == AGUA)
			return ATAQUE_INEFECTIVO;
	} else if (tipo_ataque == PLANTA) {
		if (tipo_pokemon == ROCA)
			return ATAQUE_EFECTIVO;
		if (tipo_pokemon == FUEGO)
			return ATAQUE_INEFECTIVO;
	} else if (tipo_ataque == ROCA) {
		if (tipo_pokemon == ELECTRICO)
			return ATAQUE_EFECTIVO;
		if (tipo_pokemon == PLANTA)
			return ATAQUE_INEFECTIVO;
	} else if (tipo_ataque == ELECTRICO) {
		if (tipo_pokemon == AGUA)
			return ATAQUE_EFECTIVO;
		if (tipo_pokemon == ROCA)
			return ATAQUE_INEFECTIVO;
	} else if (tipo_ataque == AGUA) {
		if (tipo_pokemon == FUEGO)
			return ATAQUE_EFECTIVO;
		if (tipo_pokemon == ELECTRICO)
			return ATAQUE_INEFECTIVO;
	}
	return ATAQUE_REGULAR;
}

/**
 * Devuelve la división de n1 por n2 redondeando para arriba.
*/
unsigned division_redondeando_para_arriba(unsigned n1, unsigned n2)
{
	if (n1 % n2 == 0)
		return n1 / n2;
	return n1 / n2 + 1;
}

/**
 * Recibe un juego, un jugador, un ataque de ese jugador y el resultado
 * de ese ataque en una jugada.
 * 
 * Según la eficacia del ataque, actualiza el puntaje del jugador.
*/
void actualizar_puntaje_jugador(juego_t *juego, JUGADOR jugador_seleccionado,
				const struct ataque *ataque,
				RESULTADO_ATAQUE resultado_ataque)
{
	jugador_t *jugador = NULL;
	if (jugador_seleccionado == JUGADOR1) {
		jugador = juego->jugador1;
	} else {
		jugador = juego->jugador2;
	}
	if (resultado_ataque == ATAQUE_EFECTIVO)
		jugador_sumar_puntos(jugador, ataque->poder * 3);
	else if (resultado_ataque == ATAQUE_INEFECTIVO)
		jugador_sumar_puntos(jugador, division_redondeando_para_arriba(
						      ataque->poder, 2));
	else
		jugador_sumar_puntos(jugador, ataque->poder);
}

/**
 * El juego procesa las jugadas de ambos jugadores. Retorna una estructura con
 * el resultado de las jugadas.
 *
 * Los pokemon seleccionados se enfrentan en combate. Los puntos asignados a
 * cada jugador por la jugada dependen del poder del ataque y el tipo del ataque
 * y el pokemon que es atacado. Si el ataque es efectivo se multiplica el poder
 * de ataque por 3. Si el ataque es no efectivo se divide por dos el poder de
 * ataque. Si no es ni una cosa ni la otra se asigna como puntaje el poder de
 * ataque.
 *
 * - Los ataques NORMAL no son efectivos ni inefectivos contra nada.
 * - Los ataques FUEGO son efectivos contra PLANTA e inefectivos contra AGUA.
 * - Los ataques PLANTA son efectivos contra ROCA e inefectivos contra FUEGO.
 * - Los ataques ROCA son efectivos contra ELECTRICO e inefectivos contra PLANTA
 * - Los ataques ELECTRICO son efectivos contra AGUA e inefectivos contra ROCA
 * - Los ataques AGUA son efectivos contra FUEGO e inefectivos contra ELECTRICO
 *
 * Un ataque puede ser utilizado una sola vez a lo largo de todo el juego. El
 * juego finaliza cuando no quedan ataques sin usar. Gana quien sume mas puntos.
 *
 * Si alguno de los resultados es ATAQUE_ERROR la jugada no ha sido procesada
 * por algún error (por ejemplo el pokemon o ataque no existe o no estan
 * disponibles).
 */
resultado_jugada_t juego_jugar_turno(juego_t *juego, jugada_t jugada_jugador1,
				     jugada_t jugada_jugador2)
{
	resultado_jugada_t resultado = { .jugador1 = ATAQUE_ERROR,
					 .jugador2 = ATAQUE_ERROR };
	if (juego_finalizado(juego))
		return resultado;
	pokemon_t *pokemon1 =
		inicializar_pokemon(juego, JUGADOR1, jugada_jugador1.pokemon);
	pokemon_t *pokemon2 =
		inicializar_pokemon(juego, JUGADOR2, jugada_jugador2.pokemon);
	if (!pokemon1 || !pokemon2)
		return resultado;
	const struct ataque *ataque1 = jugador_quitar_ataque_de_pokemon(
		juego->jugador1, jugada_jugador1.pokemon,
		jugada_jugador1.ataque);
	const struct ataque *ataque2 = jugador_quitar_ataque_de_pokemon(
		juego->jugador2, jugada_jugador2.pokemon,
		jugada_jugador2.ataque);
	if (!ataque1 || !ataque2)
		return resultado;
	resultado.jugador1 = jugador_resultado_ataque(ataque1, pokemon2);
	resultado.jugador2 = jugador_resultado_ataque(ataque2, pokemon1);
	actualizar_puntaje_jugador(juego, JUGADOR1, ataque1,
				   resultado.jugador1);
	actualizar_puntaje_jugador(juego, JUGADOR2, ataque2,
				   resultado.jugador2);
	juego->turnos_jugados++;
	return resultado;
}

/**
 * Devuelve el puntaje actual del jugador solicitado o 0 en caso de error.
 */
int juego_obtener_puntaje(juego_t *juego, JUGADOR jugador)
{
	if (!juego)
		return 0;
	if (jugador == JUGADOR1) {
		return jugador_obtener_puntaje(juego->jugador1);
	}
	return jugador_obtener_puntaje(juego->jugador2);
}

/*
 * Devuelve true si el juego ha finalizado o no existe. Devuelve false si aun
 * quedan movimientos por jugar.
 */
bool juego_finalizado(juego_t *juego)
{
	return !juego || juego->turnos_jugados == 9;
}

/**
 * Destruye el juego y toda la memoria asociada.
 */
void juego_destruir(juego_t *juego)
{
	if (!juego)
		return;
	pokemon_destruir_todo(juego->pokemones_disponibles);
	lista_destruir(juego->lista_pokemones);
	jugador_destruir(juego->jugador1);
	jugador_destruir(juego->jugador2);
	free(juego);
}
