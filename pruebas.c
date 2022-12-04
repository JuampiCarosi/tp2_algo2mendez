#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "pa2m.h"
#include "src/cajas.h"
#include "src/operaciones.h"

void NombresSeParsean() {
  char nombre_archivo[MAX_NOMBRE_ARCHIVO] = "caja1.txt";
  char *nombre = nombre_archivo_a_caja(nombre_archivo);
  pa2m_afirmar(strcmp(nombre, "caja1") == 0, "Se parsea el nombre de la caja correctamente");
  free(nombre);
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
  hash_t *contenedor_de_cajas = cargar_cajas(2, nombres_cajas, NULL);
  pa2m_afirmar(hash_cantidad(contenedor_de_cajas) == 2, "Se cargan 2 cajas");
  pa2m_afirmar(hash_contiene(contenedor_de_cajas, "caja1") && hash_contiene(contenedor_de_cajas, "caja2"),
               "Se cargan las cajas con el nombre correcto");
  destruir_contenedor_cajas(contenedor_de_cajas);

  pa2m_afirmar(cargar_cajas(0, nombres_cajas, NULL) == NULL, "No se cargan cajas si la cantidad de nombres es 0");

  pa2m_afirmar(cargar_cajas(2, NULL, NULL) == NULL, "No se cargan cajas si no se ingresan nombres");
}

void SeBuscanCajasConPokemon() {
  char *nombres_cajas[2] = {"caja1.csv", "caja2.csv"};
  hash_t *contenedor_de_cajas = cargar_cajas(2, nombres_cajas, NULL);
  lista_t *lista_cajas_indexadas = indexar_cajas(contenedor_de_cajas);

  char pokemon1[MAX_NOMBRE_POKEMON] = "pokemon1";
  char pokemon3[MAX_NOMBRE_POKEMON] = "pokemon3";

  lista_t *lista_cajas = buscar_cajas_con_pokemon(lista_cajas_indexadas, pokemon1);
  pa2m_afirmar(lista_cajas != NULL, "Se crea la lista de cajas con el pokemon");
  pa2m_afirmar(lista_tamanio(lista_cajas) == 2, "Se agregan las 2 cajas con el pokemon");
  lista_destruir(lista_cajas);

  lista_cajas = buscar_cajas_con_pokemon(lista_cajas_indexadas, pokemon3);
  pa2m_afirmar(lista_cajas != NULL, "Se crea la lista de cajas con el pokemon");
  pa2m_afirmar(lista_tamanio(lista_cajas) == 1, "Se agrega una caja con el pokemon");
  lista_destruir(lista_cajas);

  pa2m_afirmar(buscar_cajas_con_pokemon(lista_cajas_indexadas, NULL) == NULL,
               "No se crea la lista si el pokemon es NULL");

  destruir_contenedor_cajas(contenedor_de_cajas);
  destruir_lista_pokemones_indexados(lista_cajas_indexadas);

  pa2m_afirmar(buscar_cajas_con_pokemon(NULL, pokemon1) == NULL, "No se crea la lista si contenedor es NULL");
}

void SeCombinanCajas() {
  char *nombres_cajas[2] = {"caja1.csv", "caja2.csv"};
  hash_t *contenedor_de_cajas = cargar_cajas(2, nombres_cajas, NULL);
  lista_t *lista_cajas_indexadas = indexar_cajas(contenedor_de_cajas);

  char caja1[MAX_NOMBRE_ARCHIVO] = "caja1";
  char caja2[MAX_NOMBRE_ARCHIVO] = "caja2";
  char caja3[MAX_NOMBRE_ARCHIVO] = "caja3.csv";
  char caja4[MAX_NOMBRE_ARCHIVO] = "caja4.csv";

  contenedor_de_cajas = combinar_cajas(caja1, caja2, caja3, contenedor_de_cajas, lista_cajas_indexadas);

  pa2m_afirmar(hash_cantidad(contenedor_de_cajas) == 3, "La cantidad de elementos es 3");
  pa2m_afirmar(hash_contiene(contenedor_de_cajas, "caja3"), "Se crea la caja combinada con el nombre corecto");

  caja_t *caja_combinada = hash_obtener(contenedor_de_cajas, "caja3");

  pa2m_afirmar(caja_cantidad(caja_combinada) == 8, "La cantidad de pokemones dentro de la caja combinada es 8");

  contenedor_de_cajas = combinar_cajas(caja1, caja1, caja4, contenedor_de_cajas, lista_cajas_indexadas);

  pa2m_afirmar(hash_cantidad(contenedor_de_cajas) == 4 && hash_contiene(contenedor_de_cajas, "caja4"),
               "Se puede mezclar la misma caja");

  caja_combinada = hash_obtener(contenedor_de_cajas, "caja4");

  pa2m_afirmar(caja_cantidad(caja_combinada) == 6, "La cantidad de pokemones dentro de la caja combinada es 3");

  char caja_inexistente[MAX_NOMBRE_ARCHIVO] = "caja_inexistente";

  pa2m_afirmar(combinar_cajas(caja1, caja_inexistente, caja3, contenedor_de_cajas, lista_cajas_indexadas) == NULL,
               "No se crea la caja combinada si una de las cajas no existe");

  pa2m_afirmar(combinar_cajas(caja1, caja2, caja1, contenedor_de_cajas, lista_cajas_indexadas) == NULL,
               "No se crea la caja combinada si se repite el nombre de la caja combinada");

  pa2m_afirmar(combinar_cajas(caja1, caja2, caja3, contenedor_de_cajas, lista_cajas_indexadas) == NULL,
               "No puede sobreescribir un archivo ya escrito");

  pa2m_afirmar(combinar_cajas(NULL, caja2, caja3, contenedor_de_cajas, lista_cajas_indexadas) == NULL,
               "No se crea la caja combinada si el nombre de la primera caja es NULL");

  pa2m_afirmar(combinar_cajas(caja1, NULL, caja3, contenedor_de_cajas, lista_cajas_indexadas) == NULL,
               "No se crea la caja combinada si el nombre de la segunda caja es NULL");

  pa2m_afirmar(combinar_cajas(caja1, caja2, NULL, contenedor_de_cajas, lista_cajas_indexadas) == NULL,
               "No se crea la caja combinada si el nombre de la caja combinada es NULL");

  pa2m_afirmar(combinar_cajas(caja1, caja2, caja3, NULL, lista_cajas_indexadas) == NULL,
               "No se crea la caja combinada si el contenedor de cajas es NULL");

  pa2m_afirmar(combinar_cajas(caja1, caja2, caja3, contenedor_de_cajas, NULL) == NULL,
               "No se crea la caja combinada si la lista de cajas indexadas es NULL");

  destruir_contenedor_cajas(contenedor_de_cajas);
  destruir_lista_pokemones_indexados(lista_cajas_indexadas);
}

bool comparacion_pokemones(pokemon_t *pokemon1, pokemon_t *pokemon2) {
  return (strcmp(pokemon_nombre(pokemon1), pokemon_nombre(pokemon2)) == 0) &&
         pokemon_nivel(pokemon1) == pokemon_nivel(pokemon2) && pokemon_ataque(pokemon1) == pokemon_ataque(pokemon2) &&
         pokemon_defensa(pokemon1) == pokemon_defensa(pokemon2) && pokemon_ataque(pokemon1) == pokemon_ataque(pokemon2);
}

void CajaSeCargaArchivoYObtieneCantidad() {
  caja_t *caja = caja_cargar_archivo("caja1.csv");
  pa2m_afirmar(caja != NULL, "Se crea la caja");
  pa2m_afirmar(caja_cantidad(caja) == 3, "La cantidad de pokemones dentro de la caja es 3");

  pa2m_afirmar(caja_cargar_archivo("caja_inexistente.csv") == NULL, "No se crea la caja si el archivo no existe");
  pa2m_afirmar(caja_cargar_archivo(NULL) == NULL, "No se crea la caja si el archivo es NULL");
  pa2m_afirmar(caja_cantidad(NULL) == 0, "La cantidad de una caja NULL es 0");

  caja_destruir(caja);
}

void CajaSeObtienenPokemones() {
  caja_t *caja = caja_cargar_archivo("caja1.csv");
  pokemon_t *pokemon1 = pokemon_crear_desde_string("pokemon1;10;15;10");
  pokemon_t *pokemon2 = pokemon_crear_desde_string("pokemon2;3145;1345;1435");
  pokemon_t *pokemon3 = pokemon_crear_desde_string("pokemon3;1;1;1");

  pa2m_afirmar(comparacion_pokemones(pokemon1, caja_obtener_pokemon(caja, 0)), "El pokemon 1 es correcto");
  pa2m_afirmar(comparacion_pokemones(pokemon2, caja_obtener_pokemon(caja, 1)), "El pokemon 2 es correcto");
  pa2m_afirmar(comparacion_pokemones(pokemon3, caja_obtener_pokemon(caja, 2)), "El pokemon 3 es correcto");

  pa2m_afirmar(caja_obtener_pokemon(caja, 3) == NULL, "No se puede obtener un pokemon que no existe");
  pa2m_afirmar(caja_obtener_pokemon(NULL, 1) == NULL, "Obtener pokemon de caja NULL devuelve NULL");

  free(pokemon1);
  free(pokemon2);
  free(pokemon3);

  caja_destruir(caja);
}

void CajaSeCombina() {
  caja_t *caja1 = caja_cargar_archivo("caja1.csv");
  caja_t *caja2 = caja_cargar_archivo("caja2.csv");

  caja_t *caja_combinada = caja_combinar(caja1, caja2);

  pa2m_afirmar(caja_cantidad(caja_combinada) == 8, "La cantidad de pokemones dentro de la caja combinada es 8");

  pa2m_afirmar(caja_combinar(NULL, caja1) == NULL, "Si la caja 1 es NULL se devuelve NULL");
  pa2m_afirmar(caja_combinar(caja1, NULL) == NULL, "Si la caja 2 es NULL se devuelve NULL");

  caja_destruir(caja_combinada);
  caja_destruir(caja1);
  caja_destruir(caja2);
}

void nada() {}

void CajaSeRecorre() {
  caja_t *caja = caja_cargar_archivo("caja1.csv");

  pa2m_afirmar(caja_recorrer(caja, nada) == 3, "Se recorren 3 pokemones");
  pa2m_afirmar(caja_recorrer(caja, NULL) == 0, "Si la funcion es NULL se recorren 0 pokemones");
  pa2m_afirmar(caja_recorrer(NULL, nada) == 0, "Si la caja es NULL se recorren 0 pokemones");

  caja_destruir(caja);
}

void CajaSeGuardaArchivo() {
  caja_t *caja = caja_cargar_archivo("caja1.csv");

  pa2m_afirmar(caja_guardar_archivo(caja, "caja1_guardada.csv") == 3, "Se guarda la caja en un archivo");

  caja_t *caja_cargada = caja_cargar_archivo("caja1_guardada.csv");

  pokemon_t *pokemon1 = pokemon_crear_desde_string("pokemon1;10;15;10");
  pokemon_t *pokemon2 = pokemon_crear_desde_string("pokemon2;3145;1345;1435");
  pokemon_t *pokemon3 = pokemon_crear_desde_string("pokemon3;1;1;1");

  pa2m_afirmar(comparacion_pokemones(pokemon1, caja_obtener_pokemon(caja, 0)),
               "El pokemon 1 de la caja cargada es correcto");
  pa2m_afirmar(comparacion_pokemones(pokemon2, caja_obtener_pokemon(caja, 1)),
               "El pokemon 2 de la caja cargada es correcto");
  pa2m_afirmar(comparacion_pokemones(pokemon3, caja_obtener_pokemon(caja, 2)),
               "El pokemon 3 de la caja cargada es correcto");
  free(pokemon1);
  free(pokemon2);
  free(pokemon3);

  pa2m_afirmar(caja_guardar_archivo(NULL, "caja1_guardada.csv") == 0, "No se guarda la caja si es NULL");
  pa2m_afirmar(caja_guardar_archivo(caja, NULL) == 0, "No se guarda la caja si el archivo es NULL");

  caja_destruir(caja_cargada);
  caja_destruir(caja);
}

int main() {
  pa2m_nuevo_grupo("Pruebas del TDA Caja");
  CajaSeCargaArchivoYObtieneCantidad();
  CajaSeObtienenPokemones();
  CajaSeCombina();
  CajaSeRecorre();
  CajaSeGuardaArchivo();

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