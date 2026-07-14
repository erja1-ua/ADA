🇪🇸 Español | [🇬🇧 English](./README.en.md)

# 🧩 ADA — El Problema del Laberinto: Voraz, Vuelta Atrás y Ramificación y Poda

> Análisis y Diseño de Algoritmos. Tres estrategias distintas para resolver el mismo problema — encontrar el camino más corto en una cuadrícula n × m — comparando su comportamiento, coste computacional y calidad de la solución obtenida.

![Language](https://img.shields.io/badge/language-C%2B%2B-blue)
![Status](https://img.shields.io/badge/status-completo-brightgreen)

---

## 📌 Descripción

Este repositorio contiene las tres entregas de laboratorio de ADA centradas en el **problema del laberinto**: dado un tablero de celdas accesibles (`1`) e inaccesibles (`0`), encontrar el camino más corto entre la esquina superior izquierda `(0,0)` y la inferior derecha `(n-1,m-1)`. Cada práctica aborda el problema con una técnica algorítmica distinta, de menor a mayor sofisticación. Este README resume el enfoque, las reglas de movimiento y el uso de cada programa.

---

## 📂 Índice

- [Práctica 7 — Método Voraz](#-práctica-7--método-voraz-greedy)
- [Práctica 8 — Vuelta Atrás](#-práctica-8--vuelta-atrás-backtracking)
- [Práctica Final — Ramificación y Poda](#-práctica-final--ramificación-y-poda-branch--bound)
- [Comparativa de estrategias](#-comparativa-de-estrategias)

---

## 🟡 Práctica 7 — Método Voraz (Greedy)

Estrategia voraz que toma decisiones locales sin posibilidad de rectificar. **No garantiza** encontrar la solución óptima, ni siquiera un camino de salida aunque exista.

- **Movimientos permitidos (3):** derecha, abajo, diagonal (abajo-derecha).
- **Función principal:** `maze_greedy`
- **Sintaxis:** `maze_greedy [--p2D] -f fichero_entrada`
- **Salida:** longitud del camino encontrado (`0` si no llega a la salida) y, opcionalmente, el camino superpuesto al laberinto.

💻 **Código:** [`Pr7.cc`](./Pr7.cc)

---

## 🟠 Práctica 8 — Vuelta Atrás (Backtracking)

Exploración sistemática y exhaustiva del espacio de soluciones con posibilidad de retroceder, garantizando encontrar el camino óptimo si existe.

- **Movimientos permitidos (8):** cualquier celda colindante accesible.
- **Función principal:** `maze_bt`
- **Sintaxis:** `maze_bt [-p] [--p2D] -f fichero_entrada`
- **Salida:** longitud óptima, estadísticas del árbol de búsqueda (nodos visitados, explorados, hoja, no factibles y no prometedores), tiempo de CPU y camino (`-p` codificado / `--p2D` visual).
- **Optimización incluida:** inicialización con una solución voraz de partida para acotar antes de explorar, y poda por cota optimista basada en distancia Chebyshev.

💻 **Código:** [`Pr8.cc`](./Pr8.cc)

---

## 🔴 Práctica Final — Ramificación y Poda (Branch & Bound)

Mismo problema que la Práctica 8, resuelto con ramificación y poda: los nodos se exploran por prioridad (best-first) usando una cota optimista (heurística Chebyshev), lo que reduce drásticamente el número de nodos visitados frente al backtracking.

- **Movimientos permitidos (8):** cualquier celda colindante accesible.
- **Función principal:** `maze_bb`
- **Sintaxis:** `maze_bb [-p] [--p2D] -f fichero_entrada`
- **Salida:** longitud óptima, 8 estadísticas del proceso de búsqueda (incluye actualizaciones de la mejor solución desde hoja y desde cota pesimista), tiempo de CPU y camino (`-p` / `--p2D`).
- **Estructura de datos clave:** cola de prioridad (`priority_queue`) ordenada por `f = coste + heurística`, siguiendo el esquema de A*.

💻 **Código:** [`PrF.cc`](./PrF.cc)

---

## 📊 Comparativa de estrategias

| | Voraz (Pr. 7) | Vuelta Atrás (Pr. 8) | Ramificación y Poda (Pr. Final) |
|---|---|---|---|
| **Movimientos** | 3 (derecha, abajo, diagonal) | 8 (todas las direcciones) | 8 (todas las direcciones) |
| **Óptimo garantizado** | ❌ No | ✅ Sí | ✅ Sí |
| **Estrategia de exploración** | Decisión local, sin retroceso | DFS con poda | Best-first con cota optimista |
| **Nodos explorados** | Mínimos (un único camino) | Altos | Reducidos gracias a la poda |
| **Complejidad** | Baja | Media-alta | Media (mejor tiempo de respuesta) |

