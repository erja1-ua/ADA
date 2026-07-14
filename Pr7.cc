// Eric R. Jefferson Armada 
/**
 * @file maze_greedy.cc
 * @brief Resolución del problema del laberinto empleando el Método Voraz (Greedy).
 *
 * Descripción:
 * Este programa aplica una estrategia voraz para tratar de encontrar el camino 
 * más corto desde el origen (0,0) hasta el destino (n-1, m-1). 
 * Cabe destacar que este método no garantiza la solución óptima ni encontrar 
 * una salida aunque exista.
 *
 * Reglas de movimiento:
 * Solo se permiten 3 desplazamientos: derecha, abajo y en diagonal (abajo-derecha).
 *
 * Salida:
 * La función principal (maze_greedy) calcula y muestra:
 * - La longitud del mejor camino encontrado (o 0 si no hay salida).
 * - Opcionalmente (--p2D), el camino recorrido superpuesto al laberinto.
 */
#include <iostream>
#include <limits>
#include <cstring>
#include <vector>
#include <fstream>

using namespace std;

// Imprime la matriz con el camino recorrido, fila por fila
void imprimir(const vector<vector<char>> &pathMap) {
    for (const vector<char> &row : pathMap) {
        for (const char &cell : row) {
            cout << cell;
        }
        cout << endl;  // Salto de línea al final de cada fila
    }
}

// Aplica un algoritmo voraz para intentar encontrar un camino desde (0,0) hasta (n-1,m-1).
// Solo se permiten movimientos hacia la derecha, abajo o en diagonal (abajo-derecha).
// Marca el camino recorrido en pathMap con '*' y guarda la longitud en pathLength.
void maze_greedy(const vector<vector<char>> &grid, vector<vector<char>> &pathMap, long &pathLength) {
    int i = 0, j = 0;
    int rows = grid.size();
    int cols = grid[0].size();
    bool foundPath = true;

    // Copiar el laberinto original en pathMap para marcar el camino
    pathMap = grid;

    // Si la posición inicial es inaccesible, no hay solución
    if (grid[0][0] == '0') {
        pathLength = 0;
        return;
    }

    // Marcar la casilla inicial
    pathMap[0][0] = '*';
    pathLength = 1;

    // Mientras no hayamos llegado al final ni nos bloqueemos
    while ((i != rows - 1 || j != cols - 1) && foundPath) {
        // Intentar mover en diagonal (abajo-derecha)
        if (i + 1 < rows && j + 1 < cols && grid[i + 1][j + 1] == '1') {
            i++;
            j++;
        }
        // Si no, intentar mover abajo
        else if (i + 1 < rows && grid[i + 1][j] == '1') {
            i++;
        }
        // Si no, intentar mover a la derecha
        else if (j + 1 < cols && grid[i][j + 1] == '1') {
            j++;
        }
        // Si no se puede mover, se corta el camino
        else {
            foundPath = false;
        }

        // Marcar el camino si se pudo avanzar
        if (foundPath) {
            pathMap[i][j] = '*';
            pathLength++;
        }
    }

    // Si no se llegó al final, se considera que no hay solución
    if (!foundPath || (i != rows - 1 || j != cols - 1)) {
        pathLength = 0;
    }
}

bool leerFichero(string fichero, vector<vector<char>> &maze){
	bool leido;
	ifstream fich;
	int n, m;
	fich.open(fichero);
	if(fich.is_open()){
		fich >> n >> m;
		maze = vector<vector<char>>(n, vector<char>(m, 0));
		for(int i = 0; i < n; i++){
			for(int j = 0; j < m; j++){
				fich >> maze[i][j];
			}
		}
		fich.close();
		leido = true;
	}
	else{
		leido = false;
	}
	return leido;
}

bool procesarArgumentos(int argc, char *argv[], string &filename, bool &p2D){
	bool ok = false;
	if(argc != 1){
		ok = true;	
		filename = "";
		p2D = false;
		for(int i = 1; i < argc && ok; i++){
			if(strcmp(argv[i], "-f") == 0){
				if(i + 1 < argc){
					filename = argv[i + 1];
					i++;
				}
				else{
					cerr << "ERROR: missing filename." << endl;
					ok = false;
				}
			}
			else{
				if(strcmp(argv[i], "--p2D") == 0){
					p2D = true;
				}
				else{
					cerr << "ERROR: unknown option " << argv[i] << "." << endl;
					ok = false;
				}
			}
		}
	}
	if(!ok || filename == ""){
		cerr << "Usage:" << endl;
        cerr << "maze_greedy [--p2D] -f file" << endl;
		ok = false;
	}
	return ok;
}

int main(int argc, char *argv[]) {
    string filename;
    bool printPath;

    // Procesamos los argumentos
    if (!procesarArgumentos(argc, argv, filename, printPath)) {
        return 1;  // Ya imprime errores y usage en caso necesario
    }

    // Leemos el archivo con el laberinto
    vector<vector<char>> grid;
    if (!leerFichero(filename, grid)) {
        cerr << "ERROR: can't open file: " << filename << "." << endl;
        cerr << "Usage:" << endl;
        cerr << "maze_greedy  [--p2D] -f file" << endl;
        return 1;
    }

    // Aplicamos el algoritmo voraz
    vector<vector<char>> pathMap;
    long pathLength;
    maze_greedy(grid, pathMap, pathLength);

    // Mostramos el resultado
    cout << pathLength << endl;
    if (printPath) {
        imprimir(pathMap);
    }

    return 0;
}






