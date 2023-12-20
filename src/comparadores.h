#ifndef _COMPARADORES_H_
#define _COMPARADORES_H_

#include <string.h>
#include <stdbool.h>

/**
 * Compara el nombre de un pokemon con otro nombre y devuelve 0 si son iguales
 * o distinto de 0 si no lo son.
*/
int comparador_nombres_pokemon(void *pokemon, void *nombre);

/**
 * Recibe un ataque y compara su nombre con otro nombre recibido por parámetro.
 * 
 * Devuelve 0 si son iguales o distinto de 0 si no lo son.
*/
int comparador_nombres_ataque(void *ataque, void *nombre);

/**
 * Recibe un ataque y un nombre.
 * 
 * Devuelve true si el nombre del ataque es distinto al recibido por
 * parámetro o false si son iguales.
*/
bool nombres_ataque_son_diferentes(void *ataque, void *nombre);

/**
 * Recibe un pokemon y un nombre.
 * 
 * Devuelve true si el nombre del pokemon es distinto al recibido por
 * parámetro o false si son iguales.
*/
bool nombres_pokemon_son_diferentes(void *pokemon, void *nombre);

#endif // COMPARADORES_H_