#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const char Muro = '#';
const char CAMINO = '*';
const char ENTRADA = 'E';
const char SALIDA = 'S';
const char SOLUCION = 'O';

struct Pos {
    int x, y;
    Pos(int _x=-1,int _y=-1):x(_x),y(_y){} /* data */
};

class laberinto {
    int n;
    vector<vector<char>> grid;
    Pos ENTRADA,salida;
    vector<Pos> SOLUCION;

    public:
    laberinto(int tam=11):n(tam),grid(tam),vector<char>(tam,Muro){}
    void generar(){
        stack<Pos> pila;
        Pos inicio(1,1);
        int dx[4]={0,2,0,-2}, dy[4]={-2,0,2,0};
        while (!pila.empty()){
            Pos a=pila.top(); vector<int> dirs;
            for (int i = 0; i < 4; i++){
                int nx=a.c+dx[i], ny=a.y+dy[i];
                if(nx>0 && ny>0 && nx<n-1 && ny<n-1 && grid[ny][nx]==Muro)
            }
            if 
