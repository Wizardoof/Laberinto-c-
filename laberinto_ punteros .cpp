#include <iostream>   // para imprimir en pantalla y leer datos (cout, cin)
#include <stack>      // para implementar DFS (pila)
#include <queue>      // para implementar BFS (cola)
#include <cstdlib>    // rand() y srand()
#include <ctime>      // time() para semilla aleatoria
#include <algorithm>  // reverse() para invertir el camino
using namespace std;  // para evitar std:: repetidamente   

// ===== CARACTERES DEL LABERINTO =====
const char MURO = '#';      // representa una pared 
const char CAMINO = '*';    // representa un camino libre
const char SOLUCION = 'o';  // camino encontrado por BFS
const char ENTRADA = 'E';   // celda de inicio
const char SALIDA = 'S';    // celda de salida

// ===== ESTRUCTURA POSICIÓN =====
// Guarda coordenadas dentro del laberinto
struct Pos {
    int x, y;                     // columna y fila
    Pos(int _x=-1,int _y=-1):x(_x),y(_y){} // constructor con valores por defecto -1
};

// ===== CLASE LABERINTO =====
class Laberinto {
    int n;               // tamaño del laberinto (n x n)
    char** grid;         // puntero doble para matriz dinámica (cada fila es un array)
    Pos entrada, salida; // coordenadas de inicio y fin
    vector<Pos> solucion;// camino encontrado por BFS

public:
    // Constructor: reserva memoria para matriz y llena de muros
    Laberinto(int tam=11):n(tam){
        grid = new char*[n];             // crear puntero a punteros
        for(int i=0;i<n;i++){
            grid[i] = new char[n];       // cada fila
            for(int j=0;j<n;j++) grid[i][j]=MURO; // llenar de muros
        }
    }

    // Destructor: liberar memoria para evitar fugas
    ~Laberinto(){
        for(int i=0;i<n;i++) delete[] grid[i]; // liberar cada fila
        delete[] grid;                          // liberar puntero de filas
    }

    // ===== Generar laberinto usando DFS (backtracking no recursivo) =====
    void generar(){
        stack<Pos> pila;                     // pila para DFS
        Pos inicio(1,1);                     // celda inicial (evita bordes)
        grid[inicio.y][inicio.x]=CAMINO;     // marcar inicio como camino
        pila.push(inicio);                    // agregar a la pila

        // movimientos posibles: saltando 2 celdas para mantener muros
        int dx[4]={0,2,0,-2};                // derecha, abajo, izquierda, arriba (x)
        int dy[4]={-2,0,2,0};                // derecha, abajo, izquierda, arriba (y)

        // mientras haya celdas para procesar
        while(!pila.empty()){
            Pos a = pila.top(); // celda actual
            vector<int> dirs;   // direcciones válidas para moverse

            // revisar vecinos a 2 pasos que sean muros
            for(int i=0;i<4;i++){
                int nx = a.x + dx[i], ny = a.y + dy[i];
                if(nx>0 && ny>0 && nx<n-1 && ny<n-1 && grid[ny][nx]==MURO)
                    dirs.push_back(i); // guardar dirección válida
            }

            if(!dirs.empty()){
                // elegir dirección aleatoria
                int i = dirs[rand()%dirs.size()];
                int nx = a.x + dx[i], ny = a.y + dy[i];

                // abrir celda destino
                grid[ny][nx]=CAMINO;
                // abrir muro intermedio
                grid[a.y+dy[i]/2][a.x+dx[i]/2]=CAMINO;

                // avanzar
                pila.push(Pos(nx,ny));
            } else {
                // si no hay vecinos válidos → retroceder
                pila.pop();
            }
        }

        // ===== Marcar entrada y salida =====
        grid[0][0]=ENTRADA; grid[n-1][n-1]=SALIDA;
        // abrir celdas vecinas para conectarlas
        grid[0][1]=CAMINO; grid[1][0]=CAMINO;
        grid[n-1][n-2]=CAMINO; grid[n-2][n-1]=CAMINO;

        entrada = Pos(0,0);
        salida = Pos(n-1,n-1);
    }

    // ===== Resolver laberinto usando BFS (camino más corto) =====
    bool resolver(){
        queue<Pos> q;                     // cola BFS
        bool** vis = new bool*[n];        // matriz de visitados
        Pos** padre = new Pos*[n];        // matriz para reconstruir camino
        for(int i=0;i<n;i++){
            vis[i] = new bool[n]{false};  // inicializar todo en false
            padre[i] = new Pos[n];        // por defecto (-1,-1)
        }

        q.push(entrada);
        vis[entrada.y][entrada.x]=true;

        int dx[4]={0,1,0,-1}; // movimientos 4 direcciones
        int dy[4]={-1,0,1,0};
        bool encontrado=false;

        while(!q.empty() && !encontrado){
            Pos a = q.front(); q.pop();
            if(a.x==salida.x && a.y==salida.y){
                encontrado=true;
                break;
            }
            // explorar vecinos
            for(int i=0;i<4;i++){
                int nx=a.x+dx[i], ny=a.y+dy[i];
                if(nx>=0 && ny>=0 && nx<n && ny<n && !vis[ny][nx] &&
                   (grid[ny][nx]==CAMINO || grid[ny][nx]==SALIDA)){
                    vis[ny][nx]=true;
                    padre[ny][nx]=a;
                    q.push(Pos(nx,ny));
                }
            }
        }

        if(!encontrado){
            // liberar memoria
            for(int i=0;i<n;i++){ delete[] vis[i]; delete[] padre[i]; }
            delete[] vis; delete[] padre;
            return false;
        }

        // ===== Reconstruir camino desde salida hasta entrada =====
        solucion.clear();
        Pos a = salida;
        while(!(a.x==entrada.x && a.y==entrada.y)){
            solucion.push_back(a);
            a=padre[a.y][a.x];
        }
        solucion.push_back(entrada);
        reverse(solucion.begin(),solucion.end()); // ahora de inicio a fin

        // liberar memoria
        for(int i=0;i<n;i++){ delete[] vis[i]; delete[] padre[i]; }
        delete[] vis; delete[] padre;

        return true;
    }

    // ===== Mostrar laberinto =====
    void mostrar(bool conSol=false){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                char c = grid[i][j];
                // si pedimos mostrar la solución, marcar el camino
                if(conSol){
                    for(auto&p:solucion)
                        if(p.x==j && p.y==i && c==CAMINO) c=SOLUCION;
                }
                cout<<c<<' ';
            }
            cout<<"\n";
        }
    }
};

// ===== MAIN =====
int main(){
    srand(time(0)); // inicializar aleatoriedad con hora actual
    int tam;
    cout<<"Ingresa tamaño del laberinto (impar >=5): ";
    cin>>tam;

    // validar tamaño
    if(tam<5) tam=5;
    if(tam%2==0) tam++; // forzar impar

    Laberinto lab(tam);  // crear objeto

    lab.generar(); // generar laberinto
    cout<<"\nLaberinto generado ("<<tam<<"x"<<tam<<"):\n";
    lab.mostrar();

    // resolver y mostrar solución
    if(lab.resolver()){
        cout<<"\nSolución encontrada:\n";
        lab.mostrar(true);
    } else {
        cout<<"\nNo hay solución.\n";
    }
}
