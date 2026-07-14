# VIASEGURA

Herramienta de consola desarrollada en **C11** para consultar y analizar un inventario de cámaras de videovigilancia. La aplicación carga registros desde un fichero separado por `;`, permite calcular distancias geográficas, consultar cámaras por zona y localizar ubicaciones que contienen más de un dispositivo.

El proyecto fue desarrollado como trabajo académico y posteriormente reorganizado como un pequeño laboratorio de programación en C, tratamiento de ficheros, validación de entradas y compilación con CMake.

## Funcionalidades

- Carga de registros desde un fichero de datos externo.
- Búsqueda de cámaras mediante su identificador.
- Cálculo de la distancia entre dos cámaras mediante la fórmula de Haversine.
- Listado de cámaras pertenecientes a una zona concreta.
- Recuento de cámaras por zona.
- Detección de ubicaciones compartidas por varias cámaras.
- Generación del fichero `camaras.txt` con los dispositivos situados en ubicaciones repetidas.
- Posibilidad de indicar un fichero de entrada diferente mediante un argumento de línea de comandos.

## Tecnologías

- C11
- CMake 3.16+
- Entrada y salida estándar de C
- Gestión de ficheros
- Estructuras y arrays
- Cálculos geográficos

## Enfoque de seguridad y calidad

Esta versión aplica prácticas básicas de programación segura y organización del código:

- Lectura de entradas con `fgets` en lugar de funciones inseguras como `gets`.
- Conversión validada de opciones numéricas mediante `strtol`.
- Copia limitada de cadenas mediante `snprintf`.
- Control del número máximo de registros cargados.
- Comprobación de errores al abrir, crear y cerrar ficheros.
- Separación entre la lógica geográfica (`geo.c`) y la interfaz de consola (`main.c`).
- Compilación separada de los módulos mediante CMake.
- Separación entre los datos de entrada y los artefactos de compilación.

## Estructura del repositorio

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

- `main.c`: carga de datos, validación de entradas, menú y operaciones sobre el inventario.
- `geo.c` y `geo.h`: cálculo de distancias geográficas.
- `data/videovigilancia.txt`: conjunto de datos utilizado por defecto.
- `leeme.txt`: autoría original del proyecto.

## Compilación y ejecución

### Requisitos

- Compilador compatible con C11.
- CMake 3.16 o superior.

### Compilar

```bash
cmake -S . -B build
cmake --build build
```

### Ejecutar con el fichero incluido

```bash
./build/VIASEGURA
```

CMake copia automáticamente la carpeta `data/` dentro del directorio de compilación.

### Ejecutar con otro fichero

```bash
./build/VIASEGURA ruta/al/fichero.txt
```

El fichero debe incluir una cabecera y utilizar `;` como separador entre campos.

## Menú de la aplicación

```text
1 - Determinar la distancia entre dos cámaras de videovigilancia
2 - Mostrar estadísticas de cámaras por zonas
3 - Generar un fichero con cámaras en ubicaciones repetidas
0 - Terminar
```

## Decisiones técnicas

La distancia entre dos cámaras se calcula a partir de sus coordenadas mediante la fórmula de Haversine. En sistemas Unix, el ejecutable se enlaza con la biblioteca matemática `m`.

Los registros se almacenan en estructuras de C y la aplicación mantiene un límite máximo de 200 entradas para evitar escrituras fuera del array reservado.

## Autoría

Proyecto académico original desarrollado por:

- David Arévalo Rey
- Alberto Martín Gómez
- Daniel Vela Quimbay
