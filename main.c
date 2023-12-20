#include "src/adversario.h"
#include "src/ataque.h"
#include "src/juego.h"
#include "src/lista.h"
#include "src/pokemon.h"
#include "src/menu_juego.h"
#include "src/jugador.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "ctype.h"

#define RESET_COLOR    "\x1b[0m"
#define NEGRO_T        "\x1b[30m"
#define NEGRO_F        "\x1b[40m"
#define ROJO_T     "\x1b[1;31m"
#define ROJO_F     "\x1b[41m"
#define VERDE_T        "\x1b[32m"
#define VERDE_F        "\x1b[42m"
#define AMARILLO_T "\x1b[1;33m"
#define AMARILLO_F  "\x1b[43m"
#define AZUL_T     "\x1b[1;34m"
#define AZUL_F      "\x1b[44m"
#define MAGENTA_T  "\x1b[35m"
#define MAGENTA_F  "\x1b[45m"
#define CYAN_T     "\x1b[1;36m"
#define CYAN_F     "\x1b[46m"
#define BLANCO_T   "\x1b[37m"
#define BLANCO_F   "\x1b[47m"
#define TECLA_SALIR ROJO_T" (q para salir)\n"RESET_COLOR

typedef struct jugadores
{
	juego_t *juego;
	jugador_t *jugador1;
	jugador_t *jugador2;
} jugadores_t;

/**
 * Destruye la estructura jugadores y toda su memoria asociada.
*/
void jugadores_destruir(jugadores_t *jugadores)
{
	if (!jugadores)
		return;
	jugador_destruir(jugadores->jugador1);
	jugador_destruir(jugadores->jugador2);
	free(jugadores);
}

/**
 * Crea la estructura jugadores, asignando el juego pasado por parámetro.
 * 
 * Devuelve la estructura o NULL en caso de error.
*/
jugadores_t *jugadores_crear(juego_t *juego)
{
	if (!juego)
		return NULL;
	jugadores_t *jugadores = calloc(1, sizeof(jugadores_t));
	if (!jugadores)
		return NULL;
	jugadores->juego = juego;
	jugadores->jugador1 = jugador_crear(juego);
	if (!jugadores->jugador1) {
		jugadores_destruir(jugadores);
		return NULL;
	}
	jugadores->jugador2 = jugador_crear(juego);
	if (!jugadores->jugador2) {
		jugadores_destruir(jugadores);
		return NULL;
	}
	return jugadores;
}

/**
 * Función para cargar el archivo con pokemones en el juego, mostrando
 * por pantalla información necesaria para el usuario.
 * 
 * Devuelve true si se pudo cargar el archivo, o false si el usuario presionó
 * la tecla q para salir del juego.
*/
bool cargar_archivo(juego_t *juego)
{
	printf(
	"Ingresa el nombre del archivo con pokemones." TECLA_SALIR "> ");
	char nombre_archivo[200];
	char *leido = fgets(nombre_archivo, 200, stdin);
	if (leido)
		nombre_archivo[strlen(nombre_archivo) - 1] = 0;
	JUEGO_ESTADO resultado_cargar = juego_cargar_pokemon(juego,
							     nombre_archivo);
	while (!leido || resultado_cargar != TODO_OK) {
		if (strcmp(nombre_archivo, "q") == 0)
			return false;
		if (resultado_cargar == POKEMON_INSUFICIENTES) {
			printf(ROJO_T"Debe haber 4 pokemones como mínimo"); 
			printf(" para jugar!\n\n"RESET_COLOR);
		}
		else
			printf(ROJO_T"Archivo inválido :(\n\n"RESET_COLOR);
		printf("Ingresa el nombre del archivo con pokemones.");
		printf(TECLA_SALIR"> ");
		leido = fgets(nombre_archivo, 200, stdin);
		if (!leido) {
			printf("Error.\n");
			continue;
		}
		nombre_archivo[strlen(nombre_archivo) - 1] = 0;
		resultado_cargar = juego_cargar_pokemon(juego, nombre_archivo);
	}
	printf(VERDE_T"¡Pokemones cargados correctamente!\n\n"RESET_COLOR);
	return true;
}

/**
 * Muestra por pantalla los comandos disponibles para jugar, junto con sus
 * descripciones.
*/
RESULTADO_MENU mostrar_comandos_disponibles(void *nada)
{
	printf(CYAN_T"\n ----COMANDOS----\n"RESET_COLOR);
	printf(AMARILLO_T" q"RESET_COLOR " - Salir del juego.\n");
	printf(AMARILLO_T" h"RESET_COLOR " - Ver los comandos.\n");
	printf(AMARILLO_T" i"RESET_COLOR " - Ver lista de eficiencia ");
	printf("de ataques.\n");
	printf(AMARILLO_T" l"RESET_COLOR " - Ver pokemones disponibles.\n");
	printf(AMARILLO_T" s"RESET_COLOR " - Seleccionar tu equipo.\n");
	printf(AMARILLO_T" t"RESET_COLOR " - Ver tus pokemones.\n");
	printf(AMARILLO_T" e"RESET_COLOR " - Ver los pokemones de ");
	printf("tu adversario.\n");
	printf(AMARILLO_T" p"RESET_COLOR " - Ver puntajes.\n");
	printf(AMARILLO_T" j"RESET_COLOR " - Jugar ronda.\n\n");
	return MENU_OK;
}

/**
 * Función para salir del juego.
 * 
 * Devuelve MENU_SALIR.
*/
RESULTADO_MENU salir_del_juego(void *nada)
{
	return MENU_SALIR;
}

/**
 * Muestra por pantalla la eficiencia de los distintos tipos de ataque.
*/
RESULTADO_MENU mostrar_eficiencia_ataques(void *nada)
{
	printf(CYAN_T"\n ----EFICIENCIA ATAQUES----\n"RESET_COLOR);

	printf(AMARILLO_T" NORMAL"RESET_COLOR": efectivo contra ");
	printf(VERDE_T"-"RESET_COLOR" inefectivo contra ");
	printf(ROJO_T"-\n"RESET_COLOR);

	printf(AMARILLO_T" FUEGO"RESET_COLOR": efectivo contra ");
	printf(VERDE_T"PLANTA"RESET_COLOR" inefectivo contra ");
	printf(ROJO_T"AGUA\n"RESET_COLOR);

	printf(AMARILLO_T" PLANTA"RESET_COLOR": efectivo contra ");
	printf(VERDE_T"ROCA"RESET_COLOR" inefectivo contra ");
	printf(ROJO_T"FUEGO\n"RESET_COLOR);

	printf(AMARILLO_T" ROCA"RESET_COLOR": efectivo contra ");
	printf(VERDE_T"ELECTRICO"RESET_COLOR" inefectivo contra ");
	printf(ROJO_T"PLANTA\n"RESET_COLOR);

	printf(AMARILLO_T" ELECTRICO"RESET_COLOR": efectivo contra ");
	printf(VERDE_T"AGUA"RESET_COLOR" inefectivo contra ");
	printf(ROJO_T"ROCA\n"RESET_COLOR);

	printf(AMARILLO_T" AGUA"RESET_COLOR": efectivo contra ");
	printf(VERDE_T"FUEGO"RESET_COLOR" inefectivo contra ");
	printf(ROJO_T"ELECTRICO\n\n"RESET_COLOR);

	return MENU_OK;
}

/**
 * Recibe un pokemon o un ataque y devuelve su tipo. Si el indicador
 * es 0 se trata al elemento como un pokemon, y si es otro número como un
 * ataque.
*/
char *nombre_tipo_elemento(void *elemento, int indicador)
{
	enum TIPO tipo = NORMAL;
	if (indicador == 0)
		tipo = pokemon_tipo((pokemon_t *)elemento);
	else
		tipo = ((const struct ataque *)elemento)->tipo;
	if (tipo == ROCA)
		return "Roca";
	if (tipo == FUEGO)
		return "Fuego";
	if (tipo == AGUA)
		return "Agua";
	if (tipo == PLANTA)
		return "Planta";
	if (tipo == ELECTRICO)
		return "Eléctrico";
	return "Normal";
}

/**
 * Muestra los datos de un ataque por pantalla (nombre, poder, tipo).
*/
void mostrar_datos_ataque(const struct ataque *ataque, void *nada)
{
	printf("  *"VERDE_T" %s"RESET_COLOR, ataque->nombre);
	printf(" - Poder: %d - Tipo: %s\n", ataque->poder,
	       nombre_tipo_elemento((void *)ataque, 1));
}

/**
 * Muestra los datos de un pokemon (nombre y tipo) y de todos sus ataques.
 * 
 * Devuelve true.
*/
bool mostrar_pokemon_y_sus_ataques(void *pokemon, void *nada)
{
	printf(AMARILLO_T" %s"RESET_COLOR, pokemon_nombre((pokemon_t *)pokemon));
	printf(" - Tipo: %s\n", nombre_tipo_elemento(pokemon, 0));
	con_cada_ataque((pokemon_t *)pokemon, mostrar_datos_ataque, nada);
	printf("\n");
	return true;
}

/**
 * Muestra los pokemones cargados en el juego, y los ataques de cada uno.
 * 
 * Devuelve MENU_OK.
*/
RESULTADO_MENU mostrar_pokemones_cargados(void *jugadores)
{
	printf(CYAN_T"\n -----POKEMONES CARGADOS-----\n\n"RESET_COLOR);
	lista_t *lista_pokemones = juego_listar_pokemon(
					((jugadores_t *)jugadores)->juego);
	lista_con_cada_elemento(lista_pokemones, mostrar_pokemon_y_sus_ataques,
				NULL);
	return MENU_OK;
}


/**
 * Imprime por pantalla las instrucciones pertinentes para que el usuario
 * seleccione sus pokemones en el juego.
*/
void imprimir_mensaje_seleccionar()
{
	printf("\nIngresa los nombres de los tres pokemones");
	printf(" separados por espacios.");
	printf(TECLA_SALIR);
	printf(AMARILLO_T"Recuerda que el tercer pokemon será para tu");
	printf(" adversario ;)\n"RESET_COLOR"> ");
}

/**
 * Devuelve un mensaje personalizado por cada JUEGO_ESTADO que no es TODO_OK,
 * excepto ERROR_GENERAL.
 * 
 * Devuelve true si el estado no es TODO_OK o ERROR_GENERAL, o false en caso
 * contrario.
*/
bool error_seleccionar(JUEGO_ESTADO estado)
{
	if (estado == POKEMON_REPETIDO) {
		printf(ROJO_T"¡No puedes repetir pokemones!\n\n"RESET_COLOR);
		return true;
	} else if (estado == POKEMON_INEXISTENTE) {
		printf(ROJO_T"¡Has ingresado un pokemon ");
		printf("que no está disponible!\n\n"RESET_COLOR);
		return true;
	}
	return false;
}

/**
 * Selecciona los pokemones del adversario de manera aleatoria.
 * 
 * Devuelve true si se pudieron seleccionar o false en caso de error.
*/
bool seleccionar_pokemones_adversario(jugadores_t *jugadores)
{
	juego_t *juego = jugadores->juego;
	lista_t *pokemones_disponibles = juego_listar_pokemon(juego);
	size_t posicion1 = (size_t)rand() % lista_tamanio(pokemones_disponibles);
	size_t posicion2 = (size_t)rand() % lista_tamanio(pokemones_disponibles);
	while (posicion2 == posicion1)
		posicion2 = (size_t)rand() % lista_tamanio(pokemones_disponibles);
	size_t posicion3 = (size_t)rand() % lista_tamanio(pokemones_disponibles);
	while (posicion3 == posicion1 || posicion3 == posicion2)
		posicion3 = (size_t)rand() % lista_tamanio(pokemones_disponibles);
	pokemon_t *pokemon1 = lista_elemento_en_posicion(pokemones_disponibles,
							 posicion1);
	pokemon_t *pokemon2 = lista_elemento_en_posicion(pokemones_disponibles,
							 posicion2);
	pokemon_t *pokemon3 = lista_elemento_en_posicion(pokemones_disponibles,
							 posicion3);
	juego_seleccionar_pokemon(juego, JUGADOR2,
				 (char *)pokemon_nombre(pokemon1),
				 (char *)pokemon_nombre(pokemon2),
				 (char *)pokemon_nombre(pokemon3));
	return jugador_asignar_pokemones_y_ataques(
					jugadores->jugador2,
					(char *)pokemon_nombre(pokemon1),
					(char *)pokemon_nombre(pokemon2),
				 	(char *)pokemon_nombre(pokemon3)) ==
			TODO_OK;
}

/**
 * Función para seleccionar pokemones en el juego, mostrando por pantalla
 * toda la información necesaria (instrucciones para seleccionar, pokemones
 * disponibles en el juego, etc.). Luego de que el jugador selecciona sus
 * pokemones, automaticamente se realiza la selección del adversario y se
 * intercambia el tercer pokemon de cada uno.
 * 
 * Devuelve MENU_OK si se pudieron seleccionar los pokemones o MENU_ERROR en
 * caso de error (memoria, si ya estaban seleccionados los pokemones, etc.).
*/
RESULTADO_MENU seleccionar_pokemones(void *jugadores)
{
	jugador_t *jugador1 = ((jugadores_t *)jugadores)->jugador1;
	jugador_t *jugador2 = ((jugadores_t *)jugadores)->jugador2;
	if (jugador_pokemones_seleccionados(jugador1)) {
	     	printf(ROJO_T"¡Ya has seleccionado");
		printf(" tus pokemones!\n\n"RESET_COLOR);
		return MENU_ERROR;
	}
	mostrar_pokemones_cargados(jugadores);
	imprimir_mensaje_seleccionar();
	juego_t *juego = ((jugadores_t *)jugadores)->juego;
	char nombre1[20], nombre2[20], nombre3[20], linea[200];
	if (!fgets(linea, 200, stdin)) {
		printf(ROJO_T"Ocurrió un error.");
		printf(" Inténtalo de nuevo.\n\n"RESET_COLOR);
		return MENU_ERROR;
	}
	linea[strlen(linea) - 1] = 0;
	if (strcmp(linea, "q") == 0)
		return MENU_SALIR;
	if (sscanf(linea, "%19s %19s %19s\n", nombre1, nombre2, nombre3) < 3) {
		printf(ROJO_T"¡Debes ingresar tres pokemones!\n\n"RESET_COLOR);
		return MENU_ERROR;
	}
	if (error_seleccionar(juego_seleccionar_pokemon(juego, JUGADOR1,
						        nombre1, nombre2,
				     			nombre3)))
		return MENU_ERROR;
	if (jugador_asignar_pokemones_y_ataques(jugador1, nombre1,
						nombre2, nombre3) != TODO_OK ||
	    !seleccionar_pokemones_adversario(jugadores) ||
	    jugadores_intercambiar_tercer_pokemon(jugador1, jugador2) !=
	    				TODO_OK) {
		printf(ROJO_T"Error fatal. Cerrando el juego"RESET_COLOR);
		return MENU_SALIR;
	}
	printf(VERDE_T"\n¡Pokemones seleccionados");
	printf(" correctamente!\n\n"RESET_COLOR);
	return MENU_OK;
}

typedef struct estructura_auxiliar_mostrar_pokemones_jugador {
	jugador_t *jugador;
	int index_poke_actual;
} aux_mostrar_pokes_t;

/**
 * Muestra por pantalla los datos de un ataque (nombre, poder, tipo).
 * 
 * Devuelve true.
*/
bool mostrar_datos_ataque_aux(void *ataque, void *nada)
{
	mostrar_datos_ataque((const struct ataque *)ataque, nada);
	return true;
}

/**
 * Muestra por pantalla el nombre de un pokemon y sus ataques disponibles.
*/
bool mostrar_pokemon_y_ataques_disponibles(void *pokemon,
					   void *aux_mostrar_pokes)
{
	int index_pokemon =
		((aux_mostrar_pokes_t *)aux_mostrar_pokes)->index_poke_actual;
	jugador_t *jugador =
		((aux_mostrar_pokes_t *)aux_mostrar_pokes)->jugador;
	lista_t *lista_ataques = 
		jugador_listar_ataques_de_pokemon(jugador, (size_t)index_pokemon);
	if (lista_vacia(lista_ataques)) {
		((aux_mostrar_pokes_t *)aux_mostrar_pokes)->index_poke_actual++;
		return true;
	}
	printf(AMARILLO_T" %s"RESET_COLOR, pokemon_nombre((pokemon_t *)pokemon));
	printf(" - Tipo: %s\n", nombre_tipo_elemento(pokemon, 0));
	lista_con_cada_elemento(lista_ataques, mostrar_datos_ataque_aux, NULL);
	((aux_mostrar_pokes_t *)aux_mostrar_pokes)->index_poke_actual++;
	printf("\n");
	return true;
}

/**
 * Muestra por pantalla los pokemones actuales del jugador, con sus ataques.
 * 
 * Devuelve MENU_OK si se pudo mostrar o MENU_ERROR en caso de error.
*/
RESULTADO_MENU mostrar_pokemones_jugador(void *jugadores)
{
	jugador_t *jugador = ((jugadores_t *)jugadores)->jugador1;
	lista_t *lista_pokemones = jugador_listar_pokemones(jugador);
	if (lista_vacia(lista_pokemones)) {
		printf(ROJO_T"¡Primero debes seleccionar"); 
		printf(" tus pokemones!\n\n"RESET_COLOR);
		return MENU_ERROR;
	}
	printf(CYAN_T"\n -----TU EQUIPO-----\n\n"RESET_COLOR);
	aux_mostrar_pokes_t aux_mostrar_pokes = {.jugador = jugador,
						 .index_poke_actual = 0};
	lista_con_cada_elemento(lista_pokemones,
				mostrar_pokemon_y_ataques_disponibles,
				&aux_mostrar_pokes);
	return MENU_OK;
}

/**
 * Muestra por pantalla los pokemones actuales del adversario, con sus ataques.
 * 
 * Devuelve MENU_OK si se pudo mostrar o MENU_ERROR en caso de error.
*/
RESULTADO_MENU mostrar_pokemones_adversario(void *jugadores)
{
	jugador_t *jugador = ((jugadores_t *)jugadores)->jugador2;
	lista_t *lista_pokemones = jugador_listar_pokemones(jugador);
	if (lista_vacia(lista_pokemones)) {
		printf(ROJO_T"¡Primero debes seleccionar"); 
		printf(" tus pokemones!\n\n"RESET_COLOR);
		return MENU_ERROR;
	}
	printf(CYAN_T"\n -----EQUIPO RIVAL-----\n\n"RESET_COLOR);
	aux_mostrar_pokes_t aux_mostrar_pokes = {.jugador = jugador,
						 .index_poke_actual = 0};
	lista_con_cada_elemento(lista_pokemones,
				mostrar_pokemon_y_ataques_disponibles,
				&aux_mostrar_pokes);
	return MENU_OK;
}

/**
 * Muestra los puntajes de los dos jugadores por pantalla.
*/
RESULTADO_MENU mostrar_puntajes(void *jugadores)
{
	juego_t *juego = ((jugadores_t *)jugadores)->juego;
	int puntaje_propio = juego_obtener_puntaje(juego, JUGADOR1);
	int puntaje_rival = juego_obtener_puntaje(juego, JUGADOR2);
	printf(CYAN_T"\n -----PUNTAJES-----\n"RESET_COLOR);
	printf(AMARILLO_T" Tu equipo:"RESET_COLOR" %i\n",
	       puntaje_propio);
	printf(AMARILLO_T" Equipo rival:"RESET_COLOR" %d\n",
	       puntaje_rival);
	if (puntaje_propio > puntaje_rival)
		printf("¡Vas ganando! :)\n\n");
	else if (puntaje_propio < puntaje_rival)
		printf("Vas perdiendo :(, ¡pero todavía hay esperanzas!\n\n");
	else
		printf("Van empatados... ¡Qué tensión!\n\n");
	return MENU_OK;
}

/**
 * Muestra el resultado del ataque del jugador.
*/
void mostrar_resultado_ataque_aliado(char *pokemon_aliado, char *ataque,
				     RESULTADO_ATAQUE resultado,
				     char *pokemon_rival)
{
	printf("\n¡"VERDE_T"%s"RESET_COLOR" ataca a ", pokemon_aliado);
	printf(ROJO_T"%s"RESET_COLOR, pokemon_rival);
	printf(" con "CYAN_T"%s"RESET_COLOR"!\n", ataque);
	if (resultado == ATAQUE_EFECTIVO) {
		printf(VERDE_T"¡Ataque efectivo!"RESET_COLOR);
		printf(" Poder duplicado\n\n");
	} else if (resultado == ATAQUE_REGULAR) {
		printf(AMARILLO_T"Ataque regular\n\n"RESET_COLOR);
	} else {
		printf(ROJO_T"Ataque inefectivo"RESET_COLOR);
		printf(" Poder reducido\n\n");
	}
}

/**
 * Muestra el resultado del ataque del adversario.
*/
void mostrar_resultado_ataque_rival(char *pokemon_rival, char *ataque,
				    RESULTADO_ATAQUE resultado,
				    char *pokemon_aliado)
{
	printf("\n¡"ROJO_T"%s"RESET_COLOR" ataca a ", pokemon_rival);
	printf(VERDE_T"%s"RESET_COLOR, pokemon_aliado);
	printf(" con "CYAN_T"%s"RESET_COLOR"!\n", ataque);
	if (resultado == ATAQUE_EFECTIVO) {
		printf(ROJO_T"Ataque efectivo"RESET_COLOR);
		printf(" Poder duplicado\n\n");
	} else if (resultado == ATAQUE_REGULAR) {
		printf(AMARILLO_T"Ataque regular\n\n"RESET_COLOR);
	} else {
		printf(VERDE_T"¡Ataque inefectivo!"RESET_COLOR);
		printf(" Poder reducido\n\n");
	}
}

/**
 * Muestra por pantalla el resultado del ataque de los dos jugadores en
 * un turno.
*/
void mostrar_resultado_del_ataque(char *pokemon1, char *ataque1,
				  RESULTADO_ATAQUE resultado1, char *pokemon2,
				  char *ataque2, RESULTADO_ATAQUE resultado2)
{
	mostrar_resultado_ataque_aliado(pokemon1, ataque1, resultado1,
					pokemon2);
	mostrar_resultado_ataque_rival(pokemon2, ataque2, resultado2,
				       pokemon1);
}

/**
 * Función para jugar turno mostrando información por pantalla, tal como
 * los equipos, los puntajes, los resultados de los ataques y la información
 * de la eficiencia de los ataques.
 * 
 * Devuelve MENU_OK si se pudo jugar el turno correctamente o MENU_ERROR en
 * caso de error.
*/
RESULTADO_MENU jugar_turno(void *jugadores)
{
	juego_t *juego = ((jugadores_t *)jugadores)->juego;
	jugador_t *adversario = ((jugadores_t *)jugadores)->jugador2;
	if (mostrar_pokemones_jugador(jugadores) == MENU_ERROR)
		return MENU_ERROR;
	mostrar_pokemones_adversario(jugadores);
	mostrar_eficiencia_ataques(NULL);
	printf("Ingresa el nombre del pokemon y el ataque"); 
	printf(" separados por un espacio.\n> ");
	char linea[200], pokemon[20], ataque[20];
	if (!fgets(linea, 200, stdin) ||
	    sscanf(linea, "%19s %19s", pokemon, ataque) < 2) {
		printf(ROJO_T"¡Ataque inválido!\n\n"RESET_COLOR);
		return MENU_ERROR;
	}
	jugada_t j1 = {.ataque = {0}, .pokemon = {0}};
	strcpy(j1.pokemon, pokemon);
	strcpy(j1.ataque, ataque);
	jugada_t j2 = adversario_proxima_jugada((adversario_t *)adversario);
	resultado_jugada_t resultado = juego_jugar_turno(juego, j1, j2);
	if (resultado.jugador1 == ATAQUE_ERROR) {
		printf(ROJO_T"¡Ataque inválido!\n\n"RESET_COLOR);
		return MENU_ERROR;
	}
	jugador_quitar_ataque_de_pokemon(((jugadores_t *)jugadores)->jugador1,
					 pokemon, ataque);
	jugador_quitar_ataque_de_pokemon(((jugadores_t *)jugadores)->jugador2,
					 j2.pokemon, j2.ataque);
	mostrar_resultado_del_ataque(j1.pokemon, ataque, resultado.jugador1,
				     j2.pokemon, j2.ataque,
				     resultado.jugador2);
	mostrar_puntajes(jugadores);
	return MENU_OK;
}

bool agregar_comandos(menu_t *menu)
{
	if (!menu_agregar_comando(menu, "h", mostrar_comandos_disponibles) ||
	    !menu_agregar_comando(menu, "q", salir_del_juego) ||
	    !menu_agregar_comando(menu, "l", mostrar_pokemones_cargados) ||
	    !menu_agregar_comando(menu, "i", mostrar_eficiencia_ataques) || 
	    !menu_agregar_comando(menu, "s", seleccionar_pokemones) ||
	    !menu_agregar_comando(menu, "t", mostrar_pokemones_jugador) ||
	    !menu_agregar_comando(menu, "e", mostrar_pokemones_adversario) ||
	    !menu_agregar_comando(menu, "p", mostrar_puntajes) ||
	    !menu_agregar_comando(menu, "j", jugar_turno))
		return false;
	return true;
}

/**
 * Muestra el ganador del juego.
*/
void mostrar_ganador_juego(juego_t *juego)
{
	if (juego_obtener_puntaje(juego, JUGADOR1) >
	    juego_obtener_puntaje(juego, JUGADOR2))
		printf(VERDE_T"\nFelicitaciones! Has ganado :)\n"RESET_COLOR);
	else if (juego_obtener_puntaje(juego, JUGADOR2) >
		 juego_obtener_puntaje(juego, JUGADOR1))
		printf(ROJO_T"\nHas perdido :(\n"RESET_COLOR);
	else
		printf(AMARILLO_T"\nEmpate :p\n"RESET_COLOR);
}

/**
 * Libera toda la memoria necesaria para jugar.
 * 
 * Puede devolver un mensaje si se cierra el juego por un error de memoria.
*/
int cerrar_juego(juego_t *juego, menu_t *menu, jugadores_t *jugadores,
		 bool mensaje_error)
{
	if (mensaje_error)
		printf(ROJO_T"Error fatal. Cerrando el juego.\n\n"RESET_COLOR);
	else
		printf("Saliendo del juego.\n\n");
	juego_destruir(juego);
	menu_destruir(menu);
	jugadores_destruir(jugadores);
	return 0;
}

/**
 * Este main debe ser modificado para que el usuario pueda jugar el juego. Las
 * instrucciones existentes son solamente a modo ilustrativo del funcionamiento
 * muy alto nivel del juego.
 *
 * Las interacciones deben realizarse por entrada/salida estandar y estar
 * validadas.
 *
 * Se aconseja en todo momento mostrar información relevante para el jugador
 * (por ejemplo, mostrar puntaje actual y movimientos disponibles) para hacer
 * que el juego sea facil de utilizar.
 */

int main(int argc, char *argv[])
{
	printf(VERDE_T"\n¡Bienvenido!\n\n"RESET_COLOR);
	juego_t *juego = juego_crear();
	menu_t *menu = menu_crear();
	jugadores_t *jugadores = jugadores_crear(juego);
	if (!juego || !menu || !agregar_comandos(menu) || !jugadores) {
		return cerrar_juego(juego, menu, jugadores, true);
	}
	if (!cargar_archivo(juego)) {
		return cerrar_juego(juego, menu, jugadores, false);
	}
	while (!juego_finalizado(juego)) {
		printf("Ingresa un comando o presiona h para ver los");
		printf(" comandos disponibles.\n> ");
		char comando[200];
		if (!fgets(comando, 200, stdin))
			continue;	
		comando[strlen(comando) - 1] = 0;
		RESULTADO_MENU ejecucion = menu_ejecutar_comando(menu, comando,
								 jugadores);
		if (ejecucion == MENU_SALIR) {
			return cerrar_juego(juego, menu, jugadores, false);
		} else if (ejecucion == MENU_INEXISTENTE)
			printf(ROJO_T"Ese comando no existe.\n\n"RESET_COLOR);
	}
	mostrar_ganador_juego(juego);
	return cerrar_juego(juego, menu, jugadores, false);
}
