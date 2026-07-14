// Eric R. Jefferson Armada
/**
 * @file maze_bt.cc
 * @brief Resolución del problema del laberinto empleando Vuelta Atrás (Backtracking).
 * 
 * Descripción:
 * Este programa aplica el método de backtracking para encontrar la longitud del camino 
 * más corto y la ruta exacta que conduce a la salida de una cuadrícula n x m.
 * El punto de partida es la casilla (0,0) y el destino es (n-1,m-1).
 * 
 * Reglas de movimiento:
 * Se permite visitar cualquier casilla colindante (8 direcciones posibles) siempre 
 * que sea accesible (valor 1).
 * 
 * Salida:
 * La función principal (maze_bt) calcula y muestra por consola:
 * - La longitud del camino más corto.
 * - Estadísticas del árbol de búsqueda: nodos visitados, explorados, hojas, 
 *   descartados por no ser factibles y descartados por no ser prometedores.
 * - Tiempo de CPU consumido en milisegundos.
 * - Representación visual (--p2D) o codificada (-p) del camino de salida.
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <limits>
#include <chrono>
#include <algorithm>

using namespace std;

constexpr char PATH_CHAR = '*';
constexpr char WALL_CHAR = '0';
constexpr long INFINITY_COST = numeric_limits<long>::max();
constexpr unsigned NOT_VISITED = 0;

struct MazeProblem {
    vector<vector<char>> grid;
    int rows;
    int cols;
};

struct Stats {
    unsigned long visited = 0;
    unsigned long explored = 0;
    unsigned long leaves = 0;
    unsigned long non_feasible = 0;
    unsigned long non_promising = 0;
};

void print_stats(const Stats& st) {
    cout << st.visited << " "
         << st.explored << " "
         << st.leaves << " "
         << st.non_feasible << " "
         << st.non_promising << "\n";
}

bool parse_arguments(int argc, char* argv[], string& filename, bool& p2D, bool& p) {
    bool valid = false;
    p = false;
    p2D = false;
    filename = "";
    
    if (argc > 1) {
        valid = true;
        for (int i = 1; i < argc && valid; i++) {
            if (strcmp(argv[i], "-f") == 0) {
                if (i + 1 < argc) {
                    filename = argv[i + 1];
                    i++;
                } else {
                    cerr << "ERROR: missing filename." << endl;
                    valid = false;
                }
            } else if (strcmp(argv[i], "--p2D") == 0) {
                p2D = true;
            } else if (strcmp(argv[i], "-p") == 0) {
                p = true;
            } else {
                valid = false;
                cerr << "ERROR: unknown option" << endl;
            }
        }
    } else {
        cerr << "ERROR: missing filename." << endl;
        valid = false;
    }
    
    if (!valid) {
        cerr << "Usage:" << endl;
        cerr << "maze_bt [-p] [--p2D] -f fichero_entrada" << endl;
    }
    return valid;
}

bool load_maze(const string& filename, MazeProblem& problem) {
    ifstream file(filename);
    if (file.is_open()) {
        file >> problem.rows >> problem.cols;
        problem.grid.assign(problem.rows, vector<char>(problem.cols));
        for (int i = 0; i < problem.rows; i++) {
            for (int j = 0; j < problem.cols; j++) {
                file >> problem.grid[i][j];
            }
        }
        file.close();
        return true;
    } else {
        cerr << "ERROR: can't open file: " << filename << endl;
        cerr << "Usage:" << endl;
        cerr << "maze_bt [-p] [--p2D] -f fichero_entrada" << endl;
        return false;
    }
}

// Movimientos: 1-N, 2-NE, 3-E, 4-SE, 5-S, 6-SW, 7-W, 8-NW
const int move_offsets[9][2] = {
    {0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}
};

const vector<unsigned> exploration_order = {4, 5, 3, 6, 2, 7, 1, 8};

long optimistic_bound(const MazeProblem& problem, int current_r, int current_c) {
    return max(problem.rows - current_r - 1, problem.cols - current_c - 1);
}

long greedy_initialization(const MazeProblem& problem, vector<vector<unsigned>>& move_history) {
    long cost = 0;
    int r = 0, c = 0;
    
    if (problem.grid[0][0] == WALL_CHAR) return INFINITY_COST;
    
    cost = 1;
    while ((r != problem.rows - 1 || c != problem.cols - 1) && cost != INFINITY_COST) {
        if (r + 1 < problem.rows && c + 1 < problem.cols && problem.grid[r+1][c+1] != WALL_CHAR) {
            cost++; r++; c++;
            move_history[r][c] = 4;
        } else if (r + 1 < problem.rows && problem.grid[r+1][c] != WALL_CHAR) {
            cost++; r++;
            move_history[r][c] = 5;
        } else if (c + 1 < problem.cols && problem.grid[r][c+1] != WALL_CHAR) {
            cost++; c++;
            move_history[r][c] = 3;
        } else {
            cost = INFINITY_COST;
        }
    }
    return cost;
}

// OBLIGATORIO: Función nombrada explícitamente como "maze_bt"
void maze_bt(const MazeProblem& problem, int r, int c, int current_cost,
             vector<vector<long>>& min_costs, vector<vector<unsigned>>& move_history,
             long& current_best, Stats& st) {
    
    if (r == problem.rows - 1 && c == problem.cols - 1) {
        st.leaves++;
        if (current_cost < current_best) {
            current_best = current_cost;
        }
        return;
    }
    
    for (unsigned move : exploration_order) {
        st.visited++;
        int next_r = r + move_offsets[move][0];
        int next_c = c + move_offsets[move][1];
        
        if (next_r >= 0 && next_r < problem.rows && 
            next_c >= 0 && next_c < problem.cols && 
            problem.grid[next_r][next_c] == '1') {
            
            if (current_cost + 1 < min_costs[next_r][next_c]) {
                if (current_cost + 1 + optimistic_bound(problem, next_r, next_c) < current_best) {
                    st.explored++;
                    min_costs[next_r][next_c] = current_cost + 1;
                    move_history[next_r][next_c] = move;
                    
                    maze_bt(problem, next_r, next_c, current_cost + 1, 
                            min_costs, move_history, current_best, st);
                } else {
                    st.non_promising++;
                }
            } else {
                st.non_promising++;
            }
        } else {
            st.non_feasible++;
        }
    }
}

void build_path_visuals(const vector<vector<unsigned>>& move_history, 
                        vector<vector<char>>& path_grid, 
                        vector<unsigned>& move_sequence) {
    int r = move_history.size() - 1;
    int c = move_history[0].size() - 1;
    
    while (r != 0 || c != 0) {
        path_grid[r][c] = PATH_CHAR;
        unsigned previous_move = move_history[r][c];
        move_sequence.insert(move_sequence.begin(), previous_move);
        
        r -= move_offsets[previous_move][0];
        c -= move_offsets[previous_move][1];
    }
    path_grid[0][0] = PATH_CHAR;
}

void print_grid(const vector<vector<char>>& grid) {
    for (const auto& row : grid) {
        for (char cell : row) {
            cout << cell;
        }
        cout << '\n';
    }
}

void print_sequence(const vector<unsigned>& sequence) {
    cout << "<";
    for (unsigned move : sequence) {
        cout << move;
    }
    cout << ">" << endl;
}

int main(int argc, char* argv[]) {
    string filename;
    bool p2D, p;
    MazeProblem problem;
    
    if (parse_arguments(argc, argv, filename, p2D, p)) {
        if (load_maze(filename, problem)) {
            Stats st;
            
            auto start_time = chrono::high_resolution_clock::now();
            
            if (problem.grid[0][0] == WALL_CHAR) {
                auto end_time = chrono::high_resolution_clock::now();
                double cpu_time_ms = chrono::duration<double, milli>(end_time - start_time).count();
                
                cout << 0 << "\n";
                print_stats(st);
                cout << cpu_time_ms << "\n";
                if (p2D) cout << "0\n";
                if (p) cout << "<0>\n";
                return 0;
            }
            
            vector<vector<long>> min_costs(problem.rows, vector<long>(problem.cols, INFINITY_COST));
            vector<vector<unsigned>> move_history(problem.rows, vector<unsigned>(problem.cols, NOT_VISITED));
            vector<vector<unsigned>> greedy_history = move_history;
            
            long best_solution = greedy_initialization(problem, greedy_history);
            if(best_solution != INFINITY_COST) {
                move_history = greedy_history;
            }
            
            min_costs[0][0] = 1;
            maze_bt(problem, 0, 0, 1, min_costs, move_history, best_solution, st);
            
            auto end_time = chrono::high_resolution_clock::now();
            double cpu_time_ms = chrono::duration<double, milli>(end_time - start_time).count();
            
            if (best_solution == INFINITY_COST) {
                cout << 0 << "\n";
                print_stats(st);
                cout << cpu_time_ms << "\n";
                if (p2D) cout << "0\n";
                if (p) cout << "<0>\n";
            } else {
                cout << best_solution << "\n";
                print_stats(st);
                cout << cpu_time_ms << "\n";
                
                vector<vector<char>> path_grid = problem.grid;
                vector<unsigned> move_sequence;
                build_path_visuals(move_history, path_grid, move_sequence);
                
                if (p2D) {
                    print_grid(path_grid);
                }
                if (p) {
                    print_sequence(move_sequence);
                }
            }
        }
    }
    return 0;
}