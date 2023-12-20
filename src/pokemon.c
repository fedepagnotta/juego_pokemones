#include "pokemon.h"
#include "ataque.h"
#include "tipo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ATAQUES 3
#define ERROR -1
#define EXITO 0
#define PARTES_LINEA_CREACION 2
#define PARTES_LINEA_ATAQUE 3
#define FORMATO_CREACION "%19[^;];%c\n"
#define FORMATO_ATAQUE "%19[^;];%[^;];%u\n"
#define SIZE_LINEA 150
#define SIZE_NOMBRE 20

struct pokemon {
	char nombre[SIZE_NOMBRE];
	enum TIPO tipo;
	struct ataque ataque[MAX_ATAQUES];
};

struct info_pokemon {
	struct pokemon *pokemones;
	int cantidad;
};

int caracter_a_tipo(char aux_tipo, enum TIPO *tipo)
{
	switch (aux_tipo) {
	case 'N':
		*tipo = NORMAL;
		break;
	case 'F':
		*tipo = FUEGO;
		break;
	case 'A':
		*tipo = AGUA;
		break;
	case 'P':
		*tipo = PLANTA;
		break;
	case 'E':
		*tipo = ELECTRICO;
		break;
	case 'R':
		*tipo = ROCA;
		break;
	default:
		return ERROR;
	}
	return EXITO;
}

void cerrar_archivo_null(FILE *archivo)
{
	if (archivo != NULL)
		fclose(archivo);
}

size_t contar_digitos(int n)
{
	if (n == 0)
		return 1;
	size_t res = 0;
	while (n > 0) {
		n = n / 10;
		res++;
	}
	return res;
}

int error_creacion(pokemon_t *pokemon)
{
	free(pokemon);
	return ERROR;
}

int crear_pokemon(char linea[SIZE_LINEA], pokemon_t *poke_aux)
{
	char aux_nombre[SIZE_NOMBRE], aux_tipo;
	int validacion = sscanf(linea, FORMATO_CREACION, aux_nombre, &aux_tipo);
	if (validacion != PARTES_LINEA_CREACION ||
	    strlen(linea) != strlen(aux_nombre) + 3 || aux_nombre[0] == '\0') {
		return error_creacion(poke_aux);
	}
	if (caracter_a_tipo(aux_tipo, &(poke_aux->tipo)) == ERROR) {
		return error_creacion(poke_aux);
	}
	strcpy(poke_aux->nombre, aux_nombre);
	return EXITO;
}

int agregar_ataque(char linea[SIZE_LINEA], int i, pokemon_t *poke_aux)
{
	char aux_nombre[SIZE_NOMBRE], aux_tipo;
	unsigned aux_poder;
	enum TIPO *tipo = &(poke_aux->ataque[i - 1].tipo);
	int validacion = sscanf(linea, FORMATO_ATAQUE, aux_nombre, &aux_tipo,
				&aux_poder);
	if (validacion != PARTES_LINEA_ATAQUE ||
	    strlen(linea) !=
		    (strlen(aux_nombre) + 4 + contar_digitos((int)aux_poder)) ||
	    aux_nombre[0] == '\0') {
		return error_creacion(poke_aux);
	}
	if (caracter_a_tipo(aux_tipo, tipo) == ERROR) {
		return error_creacion(poke_aux);
	}
	strcpy(poke_aux->ataque[i - 1].nombre, aux_nombre);
	poke_aux->ataque[i - 1].poder = aux_poder;
	return EXITO;
}

struct info_pokemon *manejar_error(informacion_pokemon_t *info, FILE *archivo)
{
	fclose(archivo);
	if (info->cantidad == 0) {
		free(info);
		return NULL;
	}
	return info;
}

pokemon_t *crear_poke_aux(informacion_pokemon_t *info)
{
	pokemon_t *aux =
		realloc(info->pokemones,
			sizeof(pokemon_t) * (unsigned)(info->cantidad + 1));
	return aux;
}

void agregar_pokemon(pokemon_t *aux, informacion_pokemon_t *info,
		     pokemon_t *poke_aux)
{
	info->pokemones = aux;
	info->pokemones[info->cantidad] = *poke_aux;
	info->cantidad++;
}

void *error_al_iniciar(FILE *archivo, informacion_pokemon_t *info,
		       pokemon_t *poke_aux)
{
	free(poke_aux);
	free(info);
	cerrar_archivo_null(archivo);
	return NULL;
}

informacion_pokemon_t *pokemon_cargar_archivo(const char *path)
{
	if (path == NULL) {
		return NULL;
	}
	informacion_pokemon_t *info = calloc(1, sizeof(informacion_pokemon_t));
	pokemon_t *poke_aux = calloc(1, sizeof(pokemon_t));
	FILE *archivo = fopen(path, "r");
	if (archivo == NULL || info == NULL || poke_aux == NULL) {
		return error_al_iniciar(archivo, info, poke_aux);
	}
	char linea[SIZE_LINEA];
	int i = 0;
	while (fgets(linea, SIZE_LINEA, archivo)) {
		if (i == 0) {
			if (crear_pokemon(linea, poke_aux) == ERROR) {
				return manejar_error(info, archivo);
			}
			i++;
		} else {
			if (agregar_ataque(linea, i, poke_aux) == ERROR) {
				return manejar_error(info, archivo);
			}
			if (i == 3) {
				pokemon_t *aux = crear_poke_aux(info);
				if (!aux) {
					free(poke_aux);
					return manejar_error(info, archivo);
				}
				agregar_pokemon(aux, info, poke_aux);
				i = -1;
			}
			i++;
		}
	}
	free(poke_aux);
	fclose(archivo);
	return info;
}

pokemon_t *pokemon_buscar(informacion_pokemon_t *ip, const char *nombre)
{
	int i;
	if (ip != NULL && nombre != NULL) {
		for (i = 0; i < ip->cantidad; i++) {
			if (strcmp(ip->pokemones[i].nombre, nombre) == 0) {
				return &(ip->pokemones[i]);
			}
		}
	}
	return NULL;
}

int pokemon_cantidad(informacion_pokemon_t *ip)
{
	if (ip != NULL)
		return ip->cantidad;
	return 0;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (pokemon != NULL)
		return pokemon->nombre;
	return NULL;
}

enum TIPO pokemon_tipo(pokemon_t *pokemon)
{
	if (pokemon != NULL)
		return pokemon->tipo;
	return NORMAL;
}

const struct ataque *pokemon_buscar_ataque(pokemon_t *pokemon,
					   const char *nombre)
{
	int i;
	for (i = 0; i < MAX_ATAQUES; i++) {
		if (strcmp(pokemon->ataque[i].nombre, nombre) == 0)
			return &(pokemon->ataque[i]);
	}
	return NULL;
}

void ordenar_info(informacion_pokemon_t *ip)
{
	int i, a;
	for (i = 1; i < ip->cantidad; i++) {
		pokemon_t poke_actual = ip->pokemones[i];
		a = i - 1;
		while (a >= 0 && strcmp(ip->pokemones[a].nombre,
					poke_actual.nombre) > 0) {
			ip->pokemones[a + 1] = ip->pokemones[a];
			a--;
		}
		ip->pokemones[a + 1] = poke_actual;
	}
}

int con_cada_pokemon(informacion_pokemon_t *ip, void (*f)(pokemon_t *, void *),
		     void *aux)
{
	int i, res = 0;
	if (ip != NULL && f != NULL) {
		ordenar_info(ip);
		for (i = 0; i < ip->cantidad; i++) {
			f(&(ip->pokemones[i]), aux);
			res++;
		}
	}
	return res;
}

int con_cada_ataque(pokemon_t *pokemon,
		    void (*f)(const struct ataque *, void *), void *aux)
{
	int i, res = 0;
	if (pokemon != NULL && f != NULL)
		for (i = 0; i < MAX_ATAQUES; i++) {
			f(&(pokemon->ataque[i]), aux);
			res++;
		}
	return res;
}

void pokemon_destruir_todo(informacion_pokemon_t *ip)
{
	if (ip != NULL) {
		free(ip->pokemones);
		free(ip);
	}
}