# QuadTree – Simulador de partículas 2D (AED)

Simulador 2D de partículas circulares en movimiento que usa un **QuadTree implementado desde
cero** para detectar colisiones/vecinos cercanos, y lo compara contra la **solución ingenua de
fuerza bruta O(n²)**. Incluye un **visualizador interactivo** (raylib) y un **benchmark de
consola** que mide la diferencia de rendimiento.

El QuadTree (`core/QuadTree.{h,cpp}`) está hecho **sin** `std::map`, `std::set`,
`std::unordered_map` ni estructuras equivalentes. Solo usa `std::vector` para los puntos de cada
nodo y `std::unique_ptr` para los cuatro hijos. raylib se emplea únicamente para la ventana y el
dibujo.

---

## 1. Estructura

```
core/                 núcleo algorítmico (sin dependencias gráficas)
  Vec2.h / AABB.h     vector 2D y caja delimitadora (contains / intersects)
  Particle.h          struct Particle { id; x,y; vx,vy; radius; }
  Metrics.h           contadores: comparaciones, nodos, candidatos, tiempos
  QuadTree.h/.cpp     >>> QuadTree a mano (insert, subdivide, queryRange/Radius)
  BruteForce.h/.cpp   solución ingenua O(n²) (todos los pares)
  Distribution.cpp    generador de datos sintéticos (3 distribuciones)
  Simulation.cpp      física, reconstrucción del árbol y detección con QuadTree
bench/benchmark.cpp   experimento QuadTree vs fuerza bruta (tabla + CSV)
viz/Renderer.cpp/.h   render con raylib (HUD, consultas con el mouse)
app/main.cpp          aplicación visual y bucle principal
CMakeLists.txt        build (raylib se descarga con FetchContent)
```

---

## 2. Requisitos

- Compilador **C++17** (g++ ≥ 9, clang ≥ 10 o MSVC 2019+).
- **Solo para la app visual:** CMake ≥ 3.16 e internet la primera vez (descarga raylib 6.0).
  En Linux se necesitan los paquetes de desarrollo de OpenGL/X11 (`libgl1-mesa-dev`, `libx11-dev`).
- El **benchmark no necesita raylib ni internet**.

---

## 3. Compilación y ejecución

> **Compila en Release** para medir tiempos representativos. Un build *Debug* no optimiza y
> reporta tiempos varias veces más altos (los *speedup* en comparaciones no cambian; los de
> tiempo sí).

### App visual + benchmark (CMake)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

./build/viz_app      # aplicación visual
./build/bench        # benchmark (5 corridas por configuración)
./build/bench 10     # benchmark con 10 corridas
```

En **Windows/Visual Studio**: `cmake --build build --config Release`.

### Solo el benchmark, sin internet (g++)

```bash
g++ -std=c++17 -O2 -Icore \
    core/QuadTree.cpp core/Simulation.cpp core/BruteForce.cpp core/Distribution.cpp \
    bench/benchmark.cpp -o bench
./bench 10                                  # -> benchmark_resultados.csv
./bench 10 docs/benchmark_resultados.csv    # ruta de CSV personalizada
```

---

## 4. Controles de la app visual

Todos los controles usan **teclas de letra o ENTER**, iguales en cualquier distribución de
teclado. Cada tecla **cicla** entre valores fijos; el escenario se regenera al instante.

| Tecla         | Acción                                                        |
|---------------|---------------------------------------------------------------|
| `1` / `2` / `3` | Distribución: **uniforme / clusters / alta densidad**       |
| `N`           | Nº de partículas: 1000 → 5000 → 10000                          |
| `M`           | Tamaño del mundo: 1000 → 1500 → 500                            |
| `K`           | Capacidad por nodo del QuadTree: 4 → 8 → 16 → 32               |
| `T`           | Rango de radios: 2–4 → 3–6 → 6–10                              |
| `V`           | Velocidad máxima: 20 → 50 → 80 → 120                           |
| `ENTER`       | Regenerar con una nueva semilla                               |
| `R` / `C`     | Consulta **rectangular** / **circular** (arrastra el mouse)   |
| `Q`           | Mostrar/ocultar los bordes (celdas) del QuadTree              |
| `SPACE`       | Pausar / reanudar                                             |
| `H`           | Mostrar/ocultar la ayuda de controles                        |
| `ESC`         | Salir                                                         |

**Valores por defecto:** 1000 partículas, alta densidad, mundo 1000×1000, capacidad 8.

---

## 5. ¿Qué se visualiza?

- Partículas en el plano: **azul** = normal, **amarillo** = dentro de la región consultada,
  **rojo** = en colisión.
- Las **subdivisiones** actuales del QuadTree (tecla `Q`). Con datos uniformes la rejilla es
  regular; con clusters o alta densidad las celdas se vuelven pequeñas donde hay aglomeración y
  grandes donde el espacio está vacío (la naturaleza adaptativa del QuadTree).
- La **región consultada** con el mouse (rectángulo AABB o círculo) y los candidatos que devuelve.
- En el **HUD**: comparaciones por frame de QuadTree vs fuerza bruta, nodos visitados, candidatos,
  colisiones, FPS/tiempo por frame y el factor de aceleración.

La fuerza bruta se ejecuta en vivo solo hasta 3000 partículas; por encima de eso el HUD muestra la
estimación teórica O(n²/2), porque ejecutarla cada frame haría inutilizable la simulación: ese es
justamente el problema que el QuadTree resuelve.

---

## 6. Comparación experimental

**Solución ingenua:** `brute::detectCollisions` compara todos los pares i<j → O(n²), exactamente
n·(n−1)/2 comparaciones. **QuadTree:** para cada partícula consulta solo un radio acotado
(`radio + radio_máximo`) y poda las ramas que no intersectan la región. Ambos métodos producen
**el mismo conjunto de colisiones** (el benchmark lo verifica comparando los conteos).

El benchmark recorre **3 tamaños (1000, 5000, 10000) × 3 distribuciones**, promediando varias
corridas con semillas distintas. Mide tiempo de construcción y de consulta (QT y BF),
comparaciones, nodos visitados, candidatos por objeto y colisiones, y guarda todo en CSV.

La ventaja del QuadTree **depende de la distribución**: con datos uniformes el speedup en
comparaciones llega a cientos de veces, mientras que en alta densidad (casi todo amontonado) baja
notablemente, porque la poda espacial pierde eficacia. Es el comportamiento esperado y muestra
tanto la fortaleza como el peor caso de la estructura.

Reproducir:

```bash
./bench 10 docs/benchmark_resultados.csv > docs/benchmark_tabla.txt
```

---

## 7. Datos

Todos los datos son **sintéticos**, generados con `std::mt19937` (semilla configurable) en
`core/Distribution.cpp`. **No se usa ningún dataset externo.**

- **Uniforme:** partículas repartidas de forma homogénea.
- **Clusters:** 5 cúmulos gaussianos en posiciones aleatorias.
- **Alta densidad:** ~80 % de las partículas concentradas en una zona central pequeña.