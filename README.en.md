[🇪🇸 Español](./README.md) | 🇬🇧 English

# 🧩 ADA — The Maze Problem: Greedy, Backtracking, and Branch & Bound

> Algorithm Analysis and Design. Three different strategies to solve the same problem — finding the shortest path in an n × m grid — comparing their behavior, computational cost, and solution quality.

![Language](https://img.shields.io/badge/language-C%2B%2B-blue)
![Status](https://img.shields.io/badge/status-complete-brightgreen)

---

## 📌 Overview

This repository contains the three ADA lab assignments centered on the **maze problem**: given a grid of accessible (`1`) and blocked (`0`) cells, find the shortest path from the top-left corner `(0,0)` to the bottom-right `(n-1,m-1)`. Each lab tackles the problem with a different algorithmic technique, increasing in sophistication. This README summarizes the approach, movement rules, and usage of each program.

---

## 📂 Table of Contents

- [Lab 7 — Greedy Method](#-lab-7--greedy-method)
- [Lab 8 — Backtracking](#-lab-8--backtracking)
- [Final Lab — Branch & Bound](#-final-lab--branch--bound)
- [Strategy Comparison](#-strategy-comparison)

---

## 🟡 Lab 7 — Greedy Method

A greedy strategy that makes local decisions with no ability to backtrack. It **does not guarantee** finding the optimal solution, nor even reaching the exit when one exists.

- **Allowed moves (3):** right, down, diagonal (down-right).
- **Main function:** `maze_greedy`
- **Syntax:** `maze_greedy [--p2D] -f input_file`
- **Output:** length of the path found (`0` if the exit is not reached) and, optionally, the path overlaid on the maze.

💻 **Code:** [`Pr7.cc`](./Pr7.cc)

---

## 🟠 Lab 8 — Backtracking

A systematic, exhaustive exploration of the solution space with the ability to backtrack, guaranteeing the optimal path is found if one exists.

- **Allowed moves (8):** any accessible adjacent cell.
- **Main function:** `maze_bt`
- **Syntax:** `maze_bt [-p] [--p2D] -f input_file`
- **Output:** optimal length, search-tree statistics (visited, explored, leaf, non-feasible, and non-promising nodes), CPU time, and the path (`-p` encoded / `--p2D` visual).
- **Included optimization:** initialization with a greedy starting solution to bound the search early, plus pruning via an optimistic Chebyshev-distance bound.

💻 **Code:** [`Pr8.cc`](./Pr8.cc)

---

## 🔴 Final Lab — Branch & Bound

Same problem as Lab 8, solved with branch & bound: nodes are explored by priority (best-first) using an optimistic bound (Chebyshev heuristic), which drastically reduces the number of nodes visited compared to backtracking.

- **Allowed moves (8):** any accessible adjacent cell.
- **Main function:** `maze_bb`
- **Syntax:** `maze_bb [-p] [--p2D] -f input_file`
- **Output:** optimal length, 8 search-process statistics (including best-solution updates from leaves and from the pessimistic bound), CPU time, and the path (`-p` / `--p2D`).
- **Key data structure:** a `priority_queue` ordered by `f = cost + heuristic`, following the A* scheme.

💻 **Code:** [`PrF.cc`](./PrF.cc)

---

## 📊 Strategy Comparison

| | Greedy (Lab 7) | Backtracking (Lab 8) | Branch & Bound (Final Lab) |
|---|---|---|---|
| **Moves** | 3 (right, down, diagonal) | 8 (all directions) | 8 (all directions) |
| **Optimal guaranteed** | ❌ No | ✅ Yes | ✅ Yes |
| **Exploration strategy** | Local decision, no backtrack | DFS with pruning | Best-first with optimistic bound |
| **Nodes explored** | Minimal (single path) | High | Reduced thanks to pruning |
| **Complexity** | Low | Medium-high | Medium (better response time) |

