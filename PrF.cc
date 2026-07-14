// Eric R. Jefferson Armada 
/**
 * @file maze_bb.cc
 * @brief Resolución del problema del laberinto empleando Ramificación y Poda (Branch & Bound).
 *
 * Descripción:
 * Este programa obtiene la longitud del camino más corto y la ruta óptima
 * hacia la salida de un laberinto utilizando técnicas de ramificación y poda.
 * Implementa cotas optimistas y pesimistas para descartar nodos no prometedores
 * y minimizar el tiempo de ejecución de la CPU.
 *
 * Reglas de movimiento:
 * Se permite visitar cualquier casilla colindante (8 direcciones posibles) siempre que sea accesible.
 *
 * Salida:
 * La función principal (maze_bb) muestra:
 * - La longitud del camino más corto.
 * - Estadísticas detalladas (8 valores) sobre los nodos: visitados, explorados, hojas,
 *   no factibles, no prometedores, prometedores descartados y actualizaciones de la mejor solución.
 * - Tiempo de CPU consumido en milisegundos.
 * - Representación visual (--p2D) o codificada (-p) del camino óptimo.
 */
    #include <iostream>
    #include <fstream>
    #include <vector>
    #include <queue>
    #include <unordered_map>
    #include <unordered_set>
    #include <tuple>
    #include <string>
    #include <chrono>
    #include <climits>
    #include <sstream>
    #include <algorithm>
    using namespace std;

    // Estructura que representa el laberinto como un array lineal y sus dimensiones
    struct MazeMap {
        int rows, cols;
        vector<char> cells; // 0 = muro, 1 = libre

        // Comprueba si una celda (x, y) esta dentro de los limites
        inline bool inside(int x, int y) const { return x >= 0 && y >= 0 && x < rows && y < cols; }
        // Comprueba si una celda (x, y) es libre
        inline bool isFree(int x, int y) const { return inside(x, y) && cells[x*cols + y]; }
    };

    // Estructura para guardar informacion de cada estado en la busqueda
    struct State {
        int x, y;        // posicion
        int steps;       // pasos acumulados hasta aqui
        int h;           // heuristica estimada hasta el final
        int prev;        // indice al estado padre en el vector de estados
        uint8_t move;    // movimiento realizado para llegar aqui
        State(int x_, int y_, int s_, int h_, int prev_, uint8_t mv_) : x(x_), y(y_), steps(s_), h(h_), prev(prev_), move(mv_) {}
    };
    // Estructura para recopilar estadisticas de la ejecucion
    struct RunStats {
        int seen = 0, expanded = 0, leaves = 0, infeas = 0, notprom = 0, disc_prom = 0, leaf_upd = 0, pess_upd = 0;
    };

    // Arrays para los movimientos en las 8 direcciones
    const int movex[8] = {-1,-1,0,1,1,1,0,-1};
    const int movey[8] = {0,1,1,1,0,-1,-1,-1};

    // Calcula la heuristica (distancia Chebyshev) para estimar el coste restante
    inline int heuristic(int x, int y, int tx, int ty) {
        return max(abs(x-tx), abs(y-ty));
        // return 0; // Heuristica anulada para busqueda de coste uniforme
    }

    // Lee el laberinto desde un archivo y lo almacena en la estructura MazeMap
    MazeMap readMaze(const string& path) {
        ifstream f(path);
        int n, m;
        f >> n >> m;
        MazeMap mm;
        mm.rows = n; mm.cols = m; mm.cells.resize(n*m);
        for(int i = 0; i < n; ++i)
            for(int j = 0; j < m; ++j) {
                int v; f >> v; mm.cells[i*m + j] = v;
            }
        return mm;
    }

    // Algoritmo principal: resuelve el laberinto usando A* y recopila estadisticas
    // Devuelve la longitud del camino optimo y la secuencia de movimientos optima
    pair<int, vector<uint8_t>> maze_bb(const MazeMap& maze, RunStats& rs) {
        int start = 0, goal = maze.rows*maze.cols - 1;
        // Si inicio o fin es muro, no hay solucion
        if(!maze.cells[start] || !maze.cells[goal]) return {0, {}};

        using QElem = tuple<int,int,int,int,int,uint8_t>; // f, x, y, steps, prev_idx, move
        priority_queue<QElem, vector<QElem>, greater<>> open; // Cola de prioridad para A*

        unordered_map<int, int> bestStep; // Para cada celda, guarda el mejor numero de pasos encontrado
        vector<State> pathStates;         // Vector de estados para reconstruccion de camino

        // Nodo inicial en la cola
        open.emplace(heuristic(0,0,maze.rows-1,maze.cols-1), 0, 0, 1, -1, 0);

        int finalIdx = -1, bestLength = INT_MAX; // Cota pesimista inicial muy alta

        // Bucle principal del algoritmo A*
        while(!open.empty()) {
            auto [f, x, y, steps, prev, move] = open.top(); open.pop();
            int pos = x*maze.cols + y;
            ++rs.seen;
            // Si ya llegamos a esta celda con menos pasos, descartamos este estado
            if(bestStep.count(pos) && bestStep[pos] <= steps) { ++rs.disc_prom; continue; }
            bestStep[pos] = steps;
            pathStates.emplace_back(x, y, steps, f, prev, move);

            // Si llegamos a la meta, actualizamos el mejor camino
            if(x == maze.rows-1 && y == maze.cols-1) {
                ++rs.leaves;
                if(steps < bestLength) {
                    bestLength = steps;    // Actualizamos la cota pesimista
                    finalIdx = pathStates.size()-1;
                    ++rs.leaf_upd;
                }
                continue;
            }
            ++rs.expanded;

            // Expande los 8 movimientos posibles desde la celda actual
            for(int mv=0; mv<8; ++mv) {
                int nx = x + movex[mv], ny = y + movey[mv];
                // Si la nueva celda no es válida o es muro, la descartamos
                if(!maze.isFree(nx, ny)) { ++rs.infeas; continue; }
                int newSteps = steps+1;
                int h = heuristic(nx, ny, maze.rows-1, maze.cols-1);
                int fscore = newSteps + h;
                // Podamos nodos que no pueden mejorar la mejor solución encontrada
                if(fscore >= bestLength) { ++rs.notprom; continue; }
                open.emplace(fscore, nx, ny, newSteps, pathStates.size()-1, mv+1);
            }
        }

        // Si no se ha encontrado solución, devolvemos 0
        if(finalIdx==-1) return {0,{}};

        // Reconstruimos la secuencia de movimientos desde el final hasta el inicio
        vector<uint8_t> moves;
        int idx = finalIdx;
        while(idx > 0) {
            moves.push_back(pathStates[idx].move);
            idx = pathStates[idx].prev;
        }
        reverse(moves.begin(), moves.end());
        return {bestLength, moves};
    }

    // Imprime el laberinto marcando el camino óptimo con '*' (formato 2D)
    void showPath2D(const MazeMap& maze, const vector<uint8_t>& moves) {
        vector<string> out(maze.rows, string(maze.cols, '0'));
        for (int i = 0; i < maze.rows; ++i)
            for (int j = 0; j < maze.cols; ++j)
                out[i][j] = maze.cells[i * maze.cols + j] ? '1' : '0'; // Respeta muros/libres originales

        int x=0,y=0; out[x][y]='*';
        for(auto d : moves) {
            x += movex[d-1];
            y += movey[d-1];
            out[x][y]='*';
        }
        for(const auto& s : out) cout << s << '\n';
    }

    // Imprime el camino óptimo como secuencia de direcciones (formato 1D)
    void showPath1D(const vector<uint8_t>& moves) {
        cout << '<'; for(auto d:moves) cout<<int(d); cout << ">\n";
    }

    // Imprime las estadísticas de la ejecución y el tiempo empleado
    void showStats(const RunStats& rs, double ms) {
        cout << rs.seen << ' ' << rs.expanded << ' ' << rs.leaves << ' ' << rs.infeas << ' ' << rs.notprom << ' '
            << rs.disc_prom << ' ' << rs.leaf_upd << ' ' << rs.pess_upd << '\n';
        cout.setf(ios::fixed); cout.precision(3);
        cout << ms << endl;
    }

    // Función principal: gestiona los argumentos, la ejecución y la salida
    int main(int argc, char* argv[]) {
    string inputfile;
    bool flagP = false, flagP2D = false;

    // Parseo de argumentos: -f (fichero), -p, --p2D
    for(int i=1; i<argc; ++i) {
        string s(argv[i]);
        if(s == "-f" && i+1 < argc) inputfile = argv[++i];
        else if(s == "-p") flagP = true;
        else if(s == "--p2D") flagP2D = true;
        else {
            cerr << "Sintaxis: maze_bb [-p] [--p2D] -f fichero_entrada\n";
            return 1;
        }
    }
    if(inputfile.empty()) {
        cerr << "Falta -f fichero\n";
        return 1;
    }

    // Comprobación de apertura del fichero antes de leer el laberinto
    ifstream test(inputfile);
    if (!test) {
        cerr << "Error: no se pudo abrir el fichero: " << inputfile << endl;
        return 1;
    }
    test.close();

    MazeMap maze = readMaze(inputfile);
    RunStats stats;

    auto t0 = chrono::high_resolution_clock::now();
    auto [len, moves] = maze_bb(maze, stats);
    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t1-t0).count();

    cout << len << endl;
    showStats(stats, ms);

    if((flagP2D || flagP) && len==0) {
        if(flagP2D) cout << "0\n";
        if(flagP) cout << "<0>\n";
        return 0;
    }
    if(flagP2D && len>0) showPath2D(maze, moves);
    if(flagP && len>0) showPath1D(moves);
    return 0;
}