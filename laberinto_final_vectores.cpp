// ======================= INCLUDES =======================
// Librerías necesarias para distintas funcionalidades
#include <iostream>   // para imprimir en pantalla (cout) y leer (cin)
#include <vector>     // para manejar matrices dinámicas (vector de vectores)
#include <stack>      // para el algoritmo DFS (generación del laberinto)
#include <queue>      // para el algoritmo BFS (resolución del laberinto)
#include <cstdlib>    // para rand(), srand()
#include <ctime>      // para time() (semilla aleatoria)
#include <algorithm>  // para reverse() (invertir camino)
using namespace std;  // para usar std repetidamente 

// ======================= CARACTERES DEL LABERINTO =======================
const char MURO = '#';      // representa una pared
const char CAMINO = '*';    // representa un espacio libre
const char SOLUCION = 'o';  // marca el camino encontrado por BFS
const char ENTRADA = 'E';   // celda de inicio (arriba a la izquierda)
const char SALIDA = 'S';    // celda de salida (abajo a la derecha)

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
public:
    // Constructor: crea un laberinto lleno de muros
    Laberinto(int tam=11):n(tam),grid(tam,vector<char>(tam,MURO)) {}

    // ================= GENERAR LABERINTO CON DFS =================
    void generar() {
        stack<Pos> pila;                  // pila para backtracking (DFS no recursivo)
        Pos inicio(1,1);                  // celda inicial dentro de la malla (evita bordes)
        grid[inicio.y][inicio.x]=CAMINO;  // marcar inicio como camino
        pila.push(inicio);                 // colocar inicio en la pila

        // Movimientos posibles: 4 direcciones (saltando 2 celdas para dejar muros intermedios)
        int dx[4]={0,2,0,-2}; // derecha, abajo, izquierda, arriba (x)
        int dy[4]={-2,0,2,0}; // arriba, derecha, abajo, izquierda (y)

        // Bucle principal DFS
        while(!pila.empty()){
            Pos a=pila.top(); // celda actual
            vector<int> dirs; // direcciones válidas a explorar

            // Buscar vecinos a 2 pasos que sean muros (no visitados)
            for(int i=0;i<4;i++){
                int nx=a.x+dx[i], ny=a.y+dy[i];
                if(nx>0 && ny>0 && nx<n-1 && ny<n-1 && grid[ny][nx]==MURO)
                    dirs.push_back(i); // guardar dirección válida
            }

            if(!dirs.empty()){
                // Elegir dirección aleatoria entre las válidas
                int i=dirs[rand()%dirs.size()];
                int nx=a.x+dx[i], ny=a.y+dy[i];

                // Abrir celda destino
                grid[ny][nx]=CAMINO;
                // Abrir muro intermedio entre actual y destino
                grid[a.y+dy[i]/2][a.x+dx[i]/2]=CAMINO;

                // Avanzar a la nueva celda
                pila.push(Pos(nx,ny));
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
        queue<Pos> q;                               // cola para BFS
        vector<vector<bool>> vis(n,vector<bool>(n,false));   // matriz de visitados
        vector<vector<Pos>> padre(n,vector<Pos>(n));         // matriz de padres para reconstruir camino

        q.push(entrada); 
        vis[entrada.y][entrada.x]=true;

        // Movimientos posibles: 4 direcciones
        int dx[4]={0,1,0,-1}; // derecha, abajo, izquierda, arriba (x)
        int dy[4]={-1,0,1,0}; // arriba, derecha, abajo, izquierda (y)

        while(!q.empty()){
            Pos a=q.front(); q.pop();

            // Si llegamos a la salida → reconstruir camino
            if(a.x==salida.x && a.y==salida.y){
                solucion.clear();
                while(!(a.x==entrada.x && a.y==entrada.y)){
                    solucion.push_back(a);         // agregar posición actual al camino
                    a=padre[a.y][a.x];            // retroceder usando "padre"
                }
                solucion.push_back(entrada);       // agregar inicio al final
                reverse(solucion.begin(),solucion.end()); // invertir camino para inicio→fin
                return true;
            }

            // Explorar vecinos
            for(int i=0;i<4;i++){
                int nx=a.x+dx[i], ny=a.y+dy[i];
                // Si está dentro del laberinto, no visitado, y es CAMINO o SALIDA
                if(nx>=0 && ny>=0 && nx<n && ny<n && !vis[ny][nx] &&
                   (grid[ny][nx]==CAMINO || grid[ny][nx]==SALIDA)){
                    vis[ny][nx]=true;         // marcar como visitado
                    padre[ny][nx]=a;          // guardar padre
                    q.push(Pos(nx,ny));       // añadir a la cola
                }
            }
        }
        return false; // no se encontró camino
    }

    // ================= MOSTRAR LABERINTO =================
    // conSol = true → mostrar camino solución
    void mostrar(bool conSol=false){
        vector<vector<char>> temp=grid; // copia para no modificar el original

        if(conSol){
            for(auto&p:solucion)
                if(temp[p.y][p.x]==CAMINO) temp[p.y][p.x]=SOLUCION; // marcar solución
        }

        // imprimir laberinto
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++) cout<<temp[i][j]<<' ';
            cout<<"\n";
        }
    }
};

// ======================= MAIN =======================
int main(){
    srand(time(0)); // inicializar aleatoriedad con la hora actual

    // ==== PEDIR TAMAÑO AL USUARIO ====
 // Si el laberinto es menor a 5x5:
 //No hay suficiente espacio para colocar entrada, salida, caminos y muros intermedios.
 //Por ejemplo, un 3x3 tendría la entrada y salida tan cerca que el DFS no tendría sentido, y podrías no generar caminos correctamente.
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
    // \n es un carácter de salto de línea. Cada vez que lo usas, la consola pasa a la siguiente línea.
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

