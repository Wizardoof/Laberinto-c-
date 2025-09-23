// ======================= INCLUDES =======================
// Librerías necesarias para distintas funcionalidades
#include <iostream>   // para imprimir en pantalla (cout) y leer (cin)
#include <vector>     // para manejar matrices dinámicas (vector de vectores)
#include <stack>      // para el algoritmo DFS (generación del laberinto)
#include <queue>      // para el algoritmo BFS (resolución del laberinto)
#include <cstdlib>    // para rand(), srand()
#include <ctime>      // para time() (semilla aleatoria)
#include <algorithm>  // para reverse() (invertir camino)
using namespace std;  // evita escribir "std::" en cada instrucción

// ======================= CARACTERES DEL LABERINTO =======================
// Caracteres que representan diferentes elementos dentro del laberinto
const char MURO        = '#';   // representa una pared
const char CAMINO      = '*';   // representa un espacio libre (camino transitable)
const char SOLUCION    = 'o';   // marca el camino encontrado por BFS
const char ENTRADA     = 'E';   // celda de inicio (arriba a la izquierda)
const char SALIDA      = 'S';   // celda de salida (abajo a la derecha)

// ======================= ESTRUCTURA POSICIÓN =======================
// Guarda una coordenada (x,y) dentro del laberinto
struct Posicion {
    int x, y;
    Posicion(int coordenadaX=-1,int coordenadaY=-1):x(coordenadaX),y(coordenadaY){} 
};

// ======================= CLASE LABERINTO =======================
class Laberinto {
    int dimension;                                // tamaño del laberinto (dimension x dimension)
    vector<vector<char>> matrizLaberinto;         // matriz de caracteres que lo representa
    Posicion coordenadaEntrada, coordenadaSalida; // coordenadas de inicio y fin
    vector<Posicion> caminoSolucion;              // camino encontrado por BFS

public: // se puede usar en cualquier parte del código 
    // Constructor: crea un laberinto lleno de muros
    Laberinto(int dimensionInicial=11)
        : dimension(dimensionInicial),
          matrizLaberinto(dimensionInicial, vector<char>(dimensionInicial, MURO)) {}

    // ================= GENERAR LABERINTO CON DFS =================
    void generarLaberinto() {  
        // DFS: explorar → marcar camino → ir profundo → si no hay más → retroceder
        stack<Posicion> pilaCeldas;                    
        Posicion celdaInicial(1,1);                    
        matrizLaberinto[celdaInicial.y][celdaInicial.x] = CAMINO;  
        pilaCeldas.push(celdaInicial);                 

        // Movimientos posibles: 4 direcciones (saltando 2 celdas para dejar muros intermedios)
        int desplazamientosX[4] = {0, 2, 0, -2}; 
        int desplazamientosY[4] = {-2, 0, 2, 0}; 

        // Bucle principal DFS
        while(!pilaCeldas.empty()){
            Posicion celdaActual = pilaCeldas.top();     
            vector<int> direccionesValidas;             

            // Buscar vecinos a 2 pasos que sean muros (no visitados aún)
            for(int direccion=0; direccion<4; direccion++){
                int nuevaX = celdaActual.x + desplazamientosX[direccion];
                int nuevaY = celdaActual.y + desplazamientosY[direccion];

                if(nuevaX>0 && nuevaY>0 && nuevaX<dimension-1 && nuevaY<dimension-1 &&
                   matrizLaberinto[nuevaY][nuevaX]==MURO)
                {
                    direccionesValidas.push_back(direccion); 
                }
            }

            if(!direccionesValidas.empty()){
                // Elegir dirección aleatoria entre las válidas
                int direccionElegida = direccionesValidas[rand() % direccionesValidas.size()];
                int nuevaX = celdaActual.x + desplazamientosX[direccionElegida];
                int nuevaY = celdaActual.y + desplazamientosY[direccionElegida];

                // Abrir celda destino
                matrizLaberinto[nuevaY][nuevaX] = CAMINO;
                // Abrir muro intermedio entre actual y destino
                matrizLaberinto[celdaActual.y + desplazamientosY[direccionElegida]/2]
                               [celdaActual.x + desplazamientosX[direccionElegida]/2] = CAMINO;

                // Avanzar a la nueva celda
                pilaCeldas.push(Posicion(nuevaX,nuevaY));
            } else {
                // Si no hay vecinos válidos → retroceder
                pilaCeldas.pop();
            }
        }

        // ================= MARCAR ENTRADA Y SALIDA =================
        matrizLaberinto[0][0] = ENTRADA; 
        matrizLaberinto[dimension-1][dimension-1] = SALIDA;
        // Abrir celdas vecinas para conectar esquinas al interior
        matrizLaberinto[0][1] = CAMINO; matrizLaberinto[1][0] = CAMINO;
        matrizLaberinto[dimension-1][dimension-2] = CAMINO; 
        matrizLaberinto[dimension-2][dimension-1] = CAMINO;

        // Guardar coordenadas de entrada y salida
        coordenadaEntrada = Posicion(0,0);
        coordenadaSalida  = Posicion(dimension-1,dimension-1);
    }

    // ================= RESOLVER LABERINTO CON BFS =================
    bool resolverLaberinto() {   
        // BFS = busca el camino más corto de entrada a salida
        queue<Posicion> colaCeldasPendientes;                          
        vector<vector<bool>> matrizVisitados(dimension, vector<bool>(dimension,false));   
        vector<vector<Posicion>> matrizPadres(dimension, vector<Posicion>(dimension));   

        colaCeldasPendientes.push(coordenadaEntrada); 
        matrizVisitados[coordenadaEntrada.y][coordenadaEntrada.x] = true;

        // Movimientos posibles: 4 direcciones (arriba, derecha, abajo, izquierda)
        int desplazamientosX[4] = {0,1,0,-1};
        int desplazamientosY[4] = {-1,0,1,0};

        while(!colaCeldasPendientes.empty()){
            Posicion celdaActual = colaCeldasPendientes.front(); 
            colaCeldasPendientes.pop();

            // Si llegamos a la salida → reconstruir camino
            if(celdaActual.x == coordenadaSalida.x && celdaActual.y == coordenadaSalida.y){
                caminoSolucion.clear();
                while(!(celdaActual.x==coordenadaEntrada.x && celdaActual.y==coordenadaEntrada.y)){
                    caminoSolucion.push_back(celdaActual);     
                    celdaActual = matrizPadres[celdaActual.y][celdaActual.x]; 
                }
                caminoSolucion.push_back(coordenadaEntrada);       
                reverse(caminoSolucion.begin(),caminoSolucion.end()); 
                return true;
            }

            // Explorar vecinos
            for(int direccion=0; direccion<4; direccion++){
                int nuevaX = celdaActual.x + desplazamientosX[direccion];
                int nuevaY = celdaActual.y + desplazamientosY[direccion];

                // Si está dentro del laberinto, no visitado, y es CAMINO o SALIDA
                if(nuevaX>=0 && nuevaY>=0 && nuevaX<dimension && nuevaY<dimension &&
                   !matrizVisitados[nuevaY][nuevaX] &&
                   (matrizLaberinto[nuevaY][nuevaX]==CAMINO || matrizLaberinto[nuevaY][nuevaX]==SALIDA))
                {
                    matrizVisitados[nuevaY][nuevaX] = true;   
                    matrizPadres[nuevaY][nuevaX] = celdaActual;   
                    colaCeldasPendientes.push(Posicion(nuevaX,nuevaY));  
                }
            }
        }
        return false; // no se encontró camino
    }

    // ================= MOSTRAR LABERINTO =================
    void mostrarLaberinto(bool mostrarConSolucion=false){
        vector<vector<char>> copiaLaberinto = matrizLaberinto; 

        if(mostrarConSolucion){
            for(auto& posicion : caminoSolucion)
                if(copiaLaberinto[posicion.y][posicion.x] == CAMINO) 
                    copiaLaberinto[posicion.y][posicion.x] = SOLUCION; 
        }

        // imprimir laberinto
        for(int fila=0; fila<dimension; fila++){
            for(int columna=0; columna<dimension; columna++) 
                cout << copiaLaberinto[fila][columna] << ' ';
            cout << "\n";
        }
    }
};

// ======================= MAIN =======================
int main(){
    srand(time(0)); // inicializar aleatoriedad con la hora actual

    // ==== PEDIR TAMAÑO AL USUARIO ====
    int dimensionUsuario;
    cout << "Introduce el tamaño del laberinto (impar >= 5): ";
    cin >> dimensionUsuario;

    // Validaciones
    if(dimensionUsuario < 5){
        cout << "Tamaño demasiado pequeño. Se usará 5.\n";
        dimensionUsuario = 5;
    }
    if(dimensionUsuario % 2 == 0){
        cout << "Tamaño par detectado. Se incrementará en 1 para ser impar.\n";
        dimensionUsuario += 1;
    }

    Laberinto miLaberinto(dimensionUsuario); 

    // Generar laberinto
    miLaberinto.generarLaberinto();
    cout << "\n============================\n";
    cout << "LABERINTO GENERADO (" << dimensionUsuario << "x" << dimensionUsuario << "):\n\n"; 
    miLaberinto.mostrarLaberinto(); 
    cout << "============================\n";

    // Resolver y mostrar solución
    if(miLaberinto.resolverLaberinto()){
        cout << "\n============================\n";
        cout << "SOLUCIÓN ENCONTRADA:\n\n";
        miLaberinto.mostrarLaberinto(true); 
        cout << "============================\n";
    } else {
        cout << "\nNo hay solución.\n";
    }

    return 0;
}
