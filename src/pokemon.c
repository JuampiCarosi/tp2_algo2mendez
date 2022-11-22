#include "pokemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOMBRE_POKEMON 30
#define FORMATO_LECTURA "%[^;];%i;%i;%i\n"
#define PROPIEDADES_POR_LINEA 4

struct _pokemon_t {
	char nombre[MAX_NOMBRE_POKEMON];
	int nivel;
	int ataque;
	int defensa;
};

pokemon_t *pokemon_crear_desde_string(char *string)
{
	if (!string)
		return NULL;
	pokemon_t *pokemon = malloc(sizeof(pokemon_t));
	if (!pokemon)
		return NULL;
	int leidos = sscanf(string, FORMATO_LECTURA, pokemon->nombre,
			    &(pokemon->nivel), &(pokemon->ataque),
			    &(pokemon->defensa));
	if (leidos != PROPIEDADES_POR_LINEA) {
		free(pokemon);
		return NULL;
	}
	return pokemon;
}

int pokemon_nivel(pokemon_t *pokemon)
{
	if (!pokemon)
		return 0;

	return pokemon->nivel;
}

int pokemon_ataque(pokemon_t *pokemon)
{
	if (!pokemon)
		return 0;

	return pokemon->ataque;
}

int pokemon_defensa(pokemon_t *pokemon)
{
	if (!pokemon)
		return 0;

	return pokemon->defensa;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (!pokemon)
		return NULL;

	return pokemon->nombre;
}

void pokemon_destruir(pokemon_t *pokemon)
{
	if (!pokemon)
		return;

	free(pokemon);
}
