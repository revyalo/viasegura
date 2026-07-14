# VIASEGURA

VIASEGURA es una herramienta de consola escrita en C para consultar un inventario de camaras de videovigilancia. Carga datos desde un fichero separado por `;` y permite calcular distancias geograficas, listar camaras por zona y detectar ubicaciones con mas de una camara.

## Enfoque blue team

El proyecto esta orientado a inventario y analisis basico de activos fisicos. En un contexto defensivo, este tipo de herramienta ayuda a revisar cobertura, duplicidades y distribucion de dispositivos de vigilancia por zona.

Buenas practicas aplicadas en esta version:

- Separacion entre logica geografica (`geo.c`) y menu de consola (`main.c`).
- Compilacion limpia con CMake.
- Lectura de entradas con `fgets` en lugar de funciones inseguras.
- Control del numero real de registros cargados.
- Datos de entrada separados de los artefactos de compilacion.

## Estructura

```text
.
├── CMakeLists.txt
├── data/
│   └── videovigilancia.txt
├── geo.c
├── geo.h
├── main.c
└── leeme.txt
```

## Compilacion y ejecucion

```bash
cmake -S . -B build
cmake --build build
./build/VIASEGURA
```

Tambien se puede indicar otro fichero de datos:

```bash
./build/VIASEGURA data/videovigilancia.txt
```

## Funcionalidades

- Calculo de distancia entre dos camaras a partir de sus coordenadas.
- Listado de camaras por zona.
- Recuento de camaras por zona.
- Generacion de `camaras.txt` con camaras situadas en ubicaciones repetidas.

## Notas para entrevista

Puntos que conviene poder explicar:

- Por que `gets` es insegura y se sustituyo por `fgets`.
- Por que no se debe incluir un `.c` desde otro `.c`; CMake compila `main.c` y `geo.c` por separado.
- Como se calcula la distancia con la formula de Haversine.
- Como se evita depender de ficheros generados dentro de la carpeta de build.

## Autoria

Proyecto academico original realizado por las personas indicadas en `leeme.txt`. Esta version esta preparada como repositorio tecnico para portfolio.
