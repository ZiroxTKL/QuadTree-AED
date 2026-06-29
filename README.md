# QuadTree Simulator

Simulador 2D de partículas en movimiento que usa un **QuadTree** para detectar
colisiones y vecinos cercanos de forma eficiente, comparándolo contra una
solución de **fuerza bruta** `O(n²)`.

Cada partícula tiene posición, velocidad y radio. En cada frame se actualizan
las posiciones, se reconstruye el QuadTree y se detectan colisiones. La
aplicación genera datos sintéticos configurables (no requiere dataset externo).

## Características

- Inserción en el QuadTree con subdivisión automática al superar la capacidad.
- Consultas por **región rectangular** y por **radio de vecindad**.
- Detección de colisiones acelerada por el QuadTree.
- Comparación directa contra fuerza bruta, con **validación de correctitud**
  (verifica que ambos métodos detectan exactamente las mismas colisiones).
- Tres distribuciones iniciales: **uniforme**, **clusters** y **zona densa**.
- Conteo de comparaciones y candidatos, y medición de tiempo por frame.
- Visualizador opcional con SDL2 (partículas, subdivisiones, query, colisiones).

## Estructura

```
include/   headers (.hpp) organizados por modulo
src/       implementaciones (.cpp)
  core/        Particle, Rectangle, Circle, Config
  quadtree/    QuadNode, QuadTree
  simulation/  Physics, ParticleGenerator, Collision, Simulation
  benchmark/   BruteForce, Benchmark, Statistics
  renderer/    visualizador SDL2 (opcional)
  utils/       Random, Timer, Logger
data/      config.json y presets (uniform/clusters/dense)
results/   CSV generados por el benchmark
docs/      experimento.md, resultados, informe
tests/     pruebas unitarias
```

## Compilar

Requiere un compilador C++17 y CMake. SDL2 es **opcional** (solo para el
visualizador); sin él, todo el modo benchmark/headless funciona igual.

```bash
mkdir build && cd build
cmake ..
make -j
```

CMake detecta SDL2 automáticamente. Para forzar build sin visualizador:

```bash
cmake -DWITH_RENDERER=OFF ..
```

### Sin CMake (g++ directo, solo headless)

```bash
g++ -std=c++17 -O2 -Iinclude \
    src/core/*.cpp src/quadtree/*.cpp src/simulation/*.cpp \
    src/benchmark/*.cpp src/utils/*.cpp src/main.cpp \
    -o simulator
```

## Uso

```bash
# Visualizacion interactiva (requiere SDL2)
./simulator --simulate --preset data/presets/clusters.json

# Benchmark: compara QuadTree vs fuerza bruta y escribe CSV
./simulator --benchmark --preset data/presets/uniform.json \
            --sizes 1000,5000,10000 --frames 50 --out results
```

Opciones principales: `--preset <json>`, `--n <int>`, `--frames <int>`,
`--capacity <int>`, `--dist <uniform|clusters|dense>`, `--sizes <a,b,c>`,
`--out <dir>`. Ver `./simulator --help`.

### Controles del visualizador

| Tecla / acción        | Efecto                                |
|-----------------------|---------------------------------------|
| Espacio               | pausar / reanudar                     |
| `T`                   | mostrar / ocultar el QuadTree         |
| Click izquierdo       | fijar el punto de consulta de vecindad|
| Rueda del ratón       | zoom                                  |
| Flechas               | desplazar la cámara                   |
| ESC                   | salir                                 |

## Tests

```bash
cd build && ctest --output-on-failure
```

Cubren la correctitud del QuadTree (queries contra fuerza bruta), la detección
de colisiones y el generador de distribuciones.

## Resultados

Ver [`docs/experimento.md`](docs/experimento.md) para el análisis completo. En
resumen, con distribución uniforme el QuadTree alcanza ~7.4× de speedup sobre
fuerza bruta a 10 000 objetos, y el rendimiento depende fuertemente de la
distribución espacial de las partículas.
