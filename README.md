# QuadTree – Simulador de partículas 2D (AED)

Simulador 2D de partículas circulares en movimiento que usa un **QuadTree implementado desde cero**
para detectar colisiones/vecinos cercanos, y lo compara contra una **solución ingenua de fuerza
bruta (O(n²))**. Incluye un visualizador interactivo (raylib) y un benchmark de consola.

---

## 1. Estructura del proyecto
  
```
core/                 núcleo algorítmico (sin dependencias gráficas)
  Vec2.h              vector 2D
  AABB.h              caja delimitadora (contains / intersects / intersectsCircle)
  Particle.h          struct Particle { int id; double x,y,vx,vy,radius; }
  Metrics.h           contadores: comparaciones, nodos visitados, candidatos, tiempos
  QuadTree.h/.cpp     >>> QuadTree implementado a mano (insert, subdivide, queries)
  BruteForce.h/.cpp   solución ingenua O(n²) (todos contra todos)
  Distribution.h/.cpp generador de datos sintéticos (3 distribuciones)
  Simulation.h/.cpp   física, reconstrucción del árbol y detección con QuadTree
bench/benchmark.cpp   experimentos QuadTree vs fuerza bruta (salida CSV + tabla)
viz/Renderer.h/.cpp   render con raylib (HUD, consultas con mouse)
app/main.cpp          aplicación visual + configuración (CLI y teclado)
CMakeLists.txt        build (raylib es opcional: ver BUILD_VIZ)
docs/                 resultados de ejemplo del benchmark
```

La estructura asignada (QuadTree) está implementada en `core/QuadTree.{h,cpp}` **sin** usar
`std::map`, `std::set`, `std::unordered_map` ni ninguna estructura equivalente. Solo se usan
auxiliares básicos permitidos (`std::vector` para los puntos de cada nodo y `std::unique_ptr`
para los cuatro hijos). raylib se usa únicamente para la visualización/ventana.

---

## 2. Requisitos

- Compilador C++17 (g++ ≥ 9, clang ≥ 10 o MSVC 2019+).
- **Solo para la app visual:** CMake ≥ 3.16 y conexión a internet la primera vez
  (CMake descarga raylib 6.0 con `FetchContent`). En Linux, raylib necesita los paquetes de
  desarrollo de OpenGL/X11 (`libgl1-mesa-dev`, `libx11-dev`, etc.).
- El **benchmark no necesita raylib ni internet**.

---

## 3. Compilación y ejecución

### 3.1. App visual + benchmark (CMake)

> **Importante:** compila en **Release** para medir tiempos representativos. Un build *Debug*
> (p. ej. la carpeta `cmake-build-debug` de CLion) no optimiza y reporta tiempos varias veces
> más altos. Los *speedup* en comparaciones no cambian, pero los de tiempo sí.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

./build/viz_app        # aplicación visual
./build/bench          # benchmark (promedio de 5 corridas)
./build/bench 10       # benchmark con 10 corridas por configuración
```

En **CLion**: selecciona el perfil/objetivo en *Release* (no *Debug*) antes de ejecutar `bench`.
En Windows con Visual Studio: `cmake --build build --config Release`.

### 3.2. Solo el benchmark, sin internet

Si no se quiere (o no se puede) descargar raylib, se desactiva la parte visual:

```bash
cmake -B build -DBUILD_VIZ=OFF
cmake --build build --target bench -j
./build/bench 10
```

O directamente con g++, sin CMake (siempre con optimización `-O2`):

```bash
g++ -std=c++17 -O2 -Icore \
    core/QuadTree.cpp core/Simulation.cpp core/BruteForce.cpp core/Distribution.cpp \
    bench/benchmark.cpp -o bench
./bench 10
```

El benchmark muestra una **tabla legible en la consola** y guarda el **CSV** (apto para graficar)
en un archivo. Uso: `bench [repeticiones] [archivo_csv]`.

```bash
./bench                                  # 5 corridas -> benchmark_resultados.csv
./bench 10                               # 10 corridas -> benchmark_resultados.csv
./bench 10 docs/benchmark_resultados.csv # 10 corridas, ruta del CSV personalizada
```

---

## 4. Controles de la aplicación visual

| Tecla        | Acción                                            |
|--------------|---------------------------------------------------|
| `R`          | Modo consulta **rectangular** (arrastra el mouse) |
| `C`          | Modo consulta **circular** (arrastra desde el centro) |
| `Q`          | Mostrar/ocultar los bordes (subdivisiones) del QuadTree |
| `1 / 2 / 3`  | Distribución **uniforme / clusters / alta densidad** |
| `G`          | Nueva semilla (regenera el mismo patrón)          |
| `+ / -`      | Aumentar / disminuir el número de partículas (±100) |
| `[ / ]`      | Disminuir / aumentar la capacidad por nodo del QuadTree |
| `SPACE`      | Pausar / reanudar la simulación                   |
| `H`          | Mostrar/ocultar la ayuda de controles             |
| `ESC`        | Salir                                             |

---

## 5. Configuración de los datos de entrada

Todos los parámetros del enunciado son configurables. Por **línea de comandos** (valor inicial):

```bash
./build/viz_app --n 2000 --world 1200 --cap 4 --rmin 2 --rmax 5 \
                --speed 100 --seed 7 --dist clusters
```

| Opción          | Descripción                                   | Def    |
|-----------------|-----------------------------------------------|--------|
| `--n`           | número de partículas                          | 600    |
| `--world`       | tamaño del espacio 2D (cuadrado)              | 1000   |
| `--cap`         | capacidad máxima por nodo del QuadTree        | 8      |
| `--rmin/--rmax` | radio mínimo / máximo                         | 3 / 6  |
| `--speed`       | velocidad máxima                              | 80     |
| `--seed`        | semilla del generador                         | 42     |
| `--dist`        | `uniforme` · `clusters` · `alta_densidad`     | uniforme |
| `--win w h`     | tamaño de la ventana                          | 1280 900 |
| `-h, --help`    | ayuda                                         |        |

Y en **tiempo de ejecución** con las teclas `1/2/3`, `G`, `+/-`, `[ / ]` (ver tabla de controles).
La configuración activa se muestra en el panel lateral (HUD).

### Distribuciones (mínimo 3 requeridas)

- **Uniforme:** partículas repartidas de forma homogénea en todo el espacio.
- **Clusters:** 5 cúmulos gaussianos en posiciones aleatorias.
- **Alta densidad:** ~80 % de las partículas concentradas en una zona central pequeña.

---

## 6. ¿Qué se visualiza?

- Las partículas en el plano (azul = normal, **amarillo** = dentro de la región consultada,
  **rojo** = en colisión).
- Las **subdivisiones actuales** del QuadTree (tecla `Q`).
- La **región consultada**: rectángulo (AABB) o círculo (radio de vecindad).
- Los **candidatos** retornados por el QuadTree y las **colisiones** detectadas.
- En el HUD: comparaciones por frame de **QuadTree vs fuerza bruta**, nodos visitados,
  candidatos, número de colisiones, **FPS y tiempo por frame**, y el factor de aceleración.

La fuerza bruta se ejecuta en vivo solo hasta 3000 partículas (por encima de eso domina el HUD la
estimación teórica O(n²/2), porque ejecutarla cada frame haría inutilizable la simulación: ese es
justamente el problema que el QuadTree resuelve).

---

## 7. Comparación experimental

### Solución ingenua

`brute::detectCollisions` (`core/BruteForce.cpp`) compara **todos los pares** i<j: O(n²).
El QuadTree, en cambio, consulta para cada partícula solo un radio acotado
(`radio + radio_máximo`) y poda las ramas que no intersectan la región. Ambos métodos producen
**exactamente el mismo conjunto de colisiones** (el benchmark lo verifica imprimiendo y
comparando ambos conteos).

### Tamaños de entrada y justificación

Se reportan tres tamaños: **1 000 (pequeño)**, **5 000 (mediano)** y **10 000 (grande)**.
La justificación es el costo de la solución ingenua: la fuerza bruta hace exactamente
n·(n−1)/2 comparaciones, es decir ~0.5 M, ~12.5 M y ~50 M respectivamente. A 10 000 objetos la
fuerza bruta ya ronda los ~100 ms por consulta en una laptop estándar, lo que la vuelve
impracticable en tiempo real (≤16.6 ms/frame para 60 FPS); por eso 10 000 es un "grande"
representativo y a la vez el punto donde la ventaja del QuadTree se vuelve indispensable.

### Cómo reproducir

```bash
./bench 10 docs/benchmark_resultados.csv > docs/benchmark_tabla.txt
```

El benchmark recorre los 3 tamaños × las 3 distribuciones, promediando 10 corridas con semillas
distintas. Mide: tiempo de construcción, tiempo de consulta (QT y BF), comparaciones, nodos
visitados, **candidatos por objeto** y colisiones.


## 8. Datos / dataset

Todos los datos son **sintéticos**, generados con `std::mt19937` (semilla configurable) en
`core/Distribution.cpp`. **No se usa ningún dataset externo**.
