#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "pa2m.h"
#include "src/cajas.h"
#include "src/operaciones.h"

void NombresSeParsean() {
  char nombre[MAX_NOMBRE_ARCHIVO] = "caja1.txt";
  char *nombre_parseado = parsear_nombre_caja(nombre);
  pa2m_afirmar(strcmp(nombre_parseado, "caja1") == 0, "Se parsea el nombre de la caja correctamente");
  free(nombre_parseado);
}

void SeValidanCajasIngresadas() {
  char *nombres_cajas[1];
  pa2m_afirmar(validar_cajas_ingresadas(nombres_cajas, 0) == false, "Ingresar 0 cajas devuelve false");

  char *nombres_cajas2[2] = {"caja1.csv", "caja2.csv"};
  pa2m_afirmar(validar_cajas_ingresadas(nombres_cajas2, 2), "Ingresar cajas validas devuelve true");

  char *nombres_cajas3[2] = {"caja1.csv", "caja 2.csv"};
  pa2m_afirmar(validar_cajas_ingresadas(nombres_cajas3, 2) == false, "Ingresar cajas con espacios devuelve false");

  char *nombres_cajas4[2] = {"caja1.csv", "caja.1.csv"};
  pa2m_afirmar(validar_cajas_ingresadas(nombres_cajas4, 2) == false, "Ingresar cajas con puntos devuelve false");

  char *nombres_cajas5[2] = {"caja1", "caja1"};
  pa2m_afirmar(validar_cajas_ingresadas(nombres_cajas5, 2) == false, "Ingresar cajas repetidas devuelve false");

  pa2m_afirmar(validar_cajas_ingresadas(NULL, 2) == false, "Ingresar cajas NULL devuelve false");
}

void SeCarganCajas() {
  char *nombres_cajas[2] = {"caja1.csv", "caja2.csv"};
  hash_t *contenedor_de_cajas = NULL;
  cargar_cajas(&contenedor_de_cajas, 2, nombres_cajas);
  pa2m_afirmar(hash_cantidad(contenedor_de_cajas) == 2, "Se cargan 2 cajas");
  pa2m_afirmar(hash_contiene(contenedor_de_cajas, "caja1") && hash_contiene(contenedor_de_cajas, "caja2"),
               "Se cargan las cajas con el nombre correcto");
  destruir_contenedor_cajas(contenedor_de_cajas);

  cargar_cajas(&contenedor_de_cajas, 0, nombres_cajas);
  pa2m_afirmar(contenedor_de_cajas == NULL, "No se cargan cajas si la cantidad de nombres es 0");

  cargar_cajas(&contenedor_de_cajas, 2, NULL);
  pa2m_afirmar(contenedor_de_cajas == NULL, "No se cargan cajas si no se ingresan nombres");
}

void SeBuscanCajasConPokemon() {
  char *nombres_cajas[2] = {"caja1.csv", "caja2.csv"};
  hash_t *contenedor_de_cajas = NULL;
  cargar_cajas(&contenedor_de_cajas, 2, nombres_cajas);

  char pokemon1[MAX_NOMBRE_POKEMON] = "pokemon1";
  char pokemon3[MAX_NOMBRE_POKEMON] = "pokemon3";

  lista_t *lista_cajas = buscar_cajas_con_pokemon(contenedor_de_cajas, pokemon1);
  pa2m_afirmar(lista_cajas != NULL, "Se crea la lista de cajas con el pokemon");
  pa2m_afirmar(lista_tamanio(lista_cajas) == 2, "Se agregan las 2 cajas con el pokemon");
  lista_destruir(lista_cajas);

  lista_cajas = buscar_cajas_con_pokemon(contenedor_de_cajas, pokemon3);
  pa2m_afirmar(lista_cajas != NULL, "Se crea la lista de cajas con el pokemon");
  pa2m_afirmar(lista_tamanio(lista_cajas) == 1, "Se agrega una caja con el pokemon");
  lista_destruir(lista_cajas);

  pa2m_afirmar(buscar_cajas_con_pokemon(contenedor_de_cajas, NULL) == NULL,
               "No se crea la lista si el pokemon es NULL");

  destruir_contenedor_cajas(contenedor_de_cajas);

  pa2m_afirmar(buscar_cajas_con_pokemon(NULL, pokemon1) == NULL, "No se crea la lista si contenedor es NULL");
}

void SeCombinanCajas() {
  char *nombres_cajas[2] = {"caja1.csv", "caja2.csv"};
  hash_t *contenedor_de_cajas = NULL;
  cargar_cajas(&contenedor_de_cajas, 2, nombres_cajas);

  char caja1[MAX_NOMBRE_ARCHIVO] = "caja1";
  char caja2[MAX_NOMBRE_ARCHIVO] = "caja2";
  char caja3[MAX_NOMBRE_ARCHIVO] = "caja3.csv";
  char caja4[MAX_NOMBRE_ARCHIVO] = "caja4.csv";

  contenedor_de_cajas = combinar_cajas(caja1, caja2, caja3, contenedor_de_cajas);

  pa2m_afirmar(hash_cantidad(contenedor_de_cajas) == 3, "La cantidad de elementos es 3");
  pa2m_afirmar(hash_contiene(contenedor_de_cajas, "caja3"), "Se crea la caja combinada con el nombre corecto");

  caja_t *caja_combinada = hash_obtener(contenedor_de_cajas, "caja3");

  pa2m_afirmar(caja_cantidad(caja_combinada) == 7, "La cantidad de pokemones dentro de la caja combinada es 7");

  contenedor_de_cajas = combinar_cajas(caja1, caja1, caja4, contenedor_de_cajas);

  pa2m_afirmar(hash_cantidad(contenedor_de_cajas) == 4 && hash_contiene(contenedor_de_cajas, "caja4"),
               "Se puede mezclar la misma caja");

  caja_combinada = hash_obtener(contenedor_de_cajas, "caja4");

  pa2m_afirmar(caja_cantidad(caja_combinada) == 3, "La cantidad de pokemones dentro de la caja combinada es 3");

  char caja_inexistente[MAX_NOMBRE_ARCHIVO] = "caja_inexistente";

  pa2m_afirmar(combinar_cajas(caja1, caja_inexistente, caja3, contenedor_de_cajas) == NULL,
               "No se crea la caja combinada si una de las cajas no existe");

  pa2m_afirmar(combinar_cajas(caja1, caja2, caja1, contenedor_de_cajas) == NULL,
               "No se crea la caja combinada si se repite el nombre de la caja combinada");

  pa2m_afirmar(combinar_cajas(caja1, caja2, caja3, contenedor_de_cajas) == NULL,
               "No puede sobreescribir un archivo ya escrito");

  pa2m_afirmar(combinar_cajas(NULL, caja2, caja3, contenedor_de_cajas) == NULL,
               "No se crea la caja combinada si el nombre de la primera caja es NULL");

  pa2m_afirmar(combinar_cajas(caja1, NULL, caja3, contenedor_de_cajas) == NULL,
               "No se crea la caja combinada si el nombre de la segunda caja es NULL");

  pa2m_afirmar(combinar_cajas(caja1, caja2, NULL, contenedor_de_cajas) == NULL,
               "No se crea la caja combinada si el nombre de la caja combinada es NULL");

  pa2m_afirmar(combinar_cajas(caja1, caja2, caja3, NULL) == NULL,
               "No se crea la caja combinada si el contenedor de cajas es NULL");

  destruir_contenedor_cajas(contenedor_de_cajas);
}

int main() {
  pa2m_nuevo_grupo("Pruebas validacion de nombres");
  NombresSeParsean();
  SeValidanCajasIngresadas();

  pa2m_nuevo_grupo("Pruebas carga de cajas");
  SeCarganCajas();

  pa2m_nuevo_grupo("Pruebas busqueda de cajas con pokemon");
  SeBuscanCajasConPokemon();

  pa2m_nuevo_grupo("Pruebas combinacion de cajas");
  SeCombinanCajas();

  return pa2m_mostrar_reporte();
}