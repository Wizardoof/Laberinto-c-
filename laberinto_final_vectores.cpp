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
const char MURO     = '#';   // representa una pared
const char CAMINO   = '*';   // representa un espacio libre
const char SOLUCION = 'o';   // marca el camino encontrado por BFS
const char ENTRADA  = 'E';   // celda de inicio (arriba a la izquierda)
const char SALIDA   = 'S';   // celda de salida (abajo a la derecha)

// ======================= ESTRUCTURA POSICIÓN =======================
// Guarda una coordenada (x,y) dentro del laberinto
struct Pos {
    int x, y;
    Pos(int _x=-1,int _y=-1):x(_x),y(_y){} // constructor con valores por defecto -1
};

// ======================= CLASE LABERINTO =======================
class Laberinto {
    int n;                           // tamaño del laberinto (n x n)
    vector<vector<char>> grid;       // matriz de caracteres que lo representa
    Pos entrada, salida;             // coordenadas de inicio y fin
    vector<Pos> solucion;            // camino encontrado por BFS

public: // se puede usar en cualquier parte del codigo 
    // Constructor: crea un laberinto lleno de muros
    Laberinto(int tam=11):n(tam),grid(tam,vector<char>(tam,MURO)) {}

    // ================= GENERAR LABERINTO CON DFS =================
    void generar() {
        stack<Pos> pila;                  // pila para backtracking (DFS no recursivo)
        Pos inicio(1,1);                  // celda inicial dentro de la malla (evita bordes)
        grid[inicio.y][inicio.x]=CAMINO;  // marcar inicio como camino
        pila.push(inicio);                // colocar inicio en la pila

        // Movimientos posibles: 4 direcciones (saltando 2 celdas para dejar muros intermedios)
        int movX[4]={0,2,0,-2}; // desplazamientos en X
        int movY[4]={-2,0,2,0}; // desplazamientos en Y

        // Bucle principal DFS
        while(!pila.empty()){
            Pos actual=pila.top();     // celda actual
            vector<int> direcciones;   // direcciones válidas a explorar

            // Buscar vecinos a 2 pasos que sean muros (no visitados)
            for(int i=0;i<4;i++){
                int nuevaX=actual.x+movX[i];
                int nuevaY=actual.y+movY[i];

                if(nuevaX>0 && nuevaY>0 && nuevaX<n-1 && nuevaY<n-1 &&
                   grid[nuevaY][nuevaX]==MURO)
                {
                    direcciones.push_back(i); // guardar dirección válida
                }
            }

            if(!direcciones.empty()){
                // Elegir dirección aleatoria entre las válidas
                int dirElegida=direcciones[rand()%direcciones.size()];
                int nuevaX=actual.x+movX[dirElegida];
                int nuevaY=actual.y+movY[dirElegida];

                // Abrir celda destino
                grid[nuevaY][nuevaX]=CAMINO;
                // Abrir muro intermedio entre actual y destino
                grid[actual.y+movY[dirElegida]/2][actual.x+movX[dirElegida]/2]=CAMINO;

                // Avanzar a la nueva celda
                pila.push(Pos(nuevaX,nuevaY));
            } else {
                // Si no hay vecinos válidos → retroceder
                pila.pop();
            }
        }

        // ================= MARCAR ENTRADA Y SALIDA =================
        grid[0][0]=ENTRADA; grid[n-1][n-1]=SALIDA;
        // Abrir celdas vecinas para conectar esquinas al interior
        grid[0][1]=CAMINO; grid[1][0]=CAMINO;
        grid[n-1][n-2]=CAMINO; grid[n-2][n-1]=CAMINO;

        // Guardar coordenadas
        entrada=Pos(0,0);
        salida=Pos(n-1,n-1);
    }

    // ================= RESOLVER LABERINTO CON BFS =================
    bool resolver() {
        queue<Pos> cola;                           // cola para BFS
        vector<vector<bool>> visitado(n,vector<bool>(n,false));   // matriz de visitados
        vector<vector<Pos>> padre(n,vector<Pos>(n));              // matriz de padres para reconstruir camino

        cola.push(entrada); 
        visitado[entrada.y][entrada.x]=true;

        // Movimientos posibles: 4 direcciones (arriba, derecha, abajo, izquierda)
        int movX[4]={0,1,0,-1};
        int movY[4]={-1,0,1,0};

        while(!cola.empty()){
            Pos actual=cola.front(); cola.pop();

            // Si llegamos a la salida → reconstruir camino
            if(actual.x==salida.x && actual.y==salida.y){
                solucion.clear();
                while(!(actual.x==entrada.x && actual.y==entrada.y)){
                    solucion.push_back(actual);     // agregar posición actual al camino
                    actual=padre[actual.y][actual.x]; // retroceder usando "padre"
                }
                solucion.push_back(entrada);       // agregar inicio al final
                reverse(solucion.begin(),solucion.end()); // invertir camino inicio→fin
                return true;
            }

            // Explorar vecinos
            for(int i=0;i<4;i++){
                int nuevaX=actual.x+movX[i];
                int nuevaY=actual.y+movY[i];

                // Si está dentro del laberinto, no visitado, y es CAMINO o SALIDA
                if(nuevaX>=0 && nuevaY>=0 && nuevaX<n && nuevaY<n &&
                   !visitado[nuevaY][nuevaX] &&
                   (grid[nuevaY][nuevaX]==CAMINO || grid[nuevaY][nuevaX]==SALIDA))
                {
                    visitado[nuevaY][nuevaX]=true;   // marcar como visitado
                    padre[nuevaY][nuevaX]=actual;   // guardar padre
                    cola.push(Pos(nuevaX,nuevaY));  // añadir a la cola
                }
            }
        }
        return false; // no se encontró camino
    }

    // ================= MOSTRAR LABERINTO =================
    // conSol = true → mostrar camino solución
    void mostrar(bool conSol=false){
        vector<vector<char>> copia=grid; // copia para no modificar el original

        if(conSol){
            for(auto&p:solucion)
                if(copia[p.y][p.x]==CAMINO) copia[p.y][p.x]=SOLUCION; // marcar solución
        }

        // imprimir laberinto
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++) cout<<copia[i][j]<<' ';
            cout<<"\n";
        }
    }
};

// ======================= MAIN =======================
int main(){
    srand(time(0)); // inicializar aleatoriedad con la hora actual

    // ==== PEDIR TAMAÑO AL USUARIO ====
    int tam;
    cout << "Introduce el tamaño del laberinto (impar >= 5): ";
    cin >> tam;

    // Validaciones
    if(tam < 5){
        cout << "Tamaño demasiado pequeño. Se usará 5.\n";
        tam = 5;
    }
    if(tam % 2 == 0){
        cout << "Tamaño par detectado. Se incrementará en 1 para ser impar.\n";
        tam += 1;
    }

    Laberinto lab(tam); // crear laberinto con tamaño elegido

    // Generar laberinto
    lab.generar();
    cout << "\n============================\n";
    cout << "LABERINTO GENERADO (" << tam << "x" << tam << "):\n\n"; 
    lab.mostrar(); // mostrar laberinto generado
    cout << "============================\n";

    // Resolver y mostrar solución
    if(lab.resolver()){
        cout << "\n============================\n";
        cout << "SOLUCIÓN ENCONTRADA:\n\n";
        lab.mostrar(true); // mostrar laberinto con camino solución
        cout << "============================\n";
    } else {
        cout << "\nNo hay solución.\n";
    }

    return 0;
}
