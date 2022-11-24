#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/cajas.h"
#include "src/pokemon.h"

bool mostrar_pokemon(pokemon_t *pokemones) {
  printf("elementos : %p\n", (void *)pokemones);

  printf("  Nombre: %s\n    Nivel: %i\n    Ataque: %i\n    Defensa: %i\n", pokemon_nombre(pokemones),
         pokemon_nivel(pokemones), pokemon_ataque(pokemones), pokemon_defensa(pokemones));
  return true;
}

int main(int argc, char *argv[]) {
  caja_t *caja1 = caja_cargar_archivo(argv[1]);

  // printf("Pokemones de la caja 1\n");
  // int cantidad_pokemones_recorridos = caja_recorrer(caja1, mostrar_pokemon);
  // printf("Cantidad de pokemones contados en caja 1: %i\n", cantidad_pokemones_recorridos);

  // caja_t *caja2 = caja_cargar_archivo(argv[2]);
  // printf("Hay %i pokemones en las cajas 1 y 2\n",
  //        caja_cantidad(caja1) + caja_cantidad(caja2));

  // caja_t *caja_combinada = caja_combinar(caja1, caja2);
  // printf("Pokemones de las cajas 1 y 2\n ");
  // caja_recorrer(caja_combinada, mostrar_pokemon);

  // caja_guardar_archivo(caja_combinada, argv[3]);
  caja_destruir(caja1);
  // caja_destruir(caja2);
  // caja_destruir(caja_combinada);
  return 0;
}
