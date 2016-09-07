#pragma once
#include "Matrix4D.h"
#include <set>
#include <vector>
#include "DXBasicPainter.h"
#include "Mesh.h"
using namespace std;

// MAX_OCTREE_DEPTH es el numero maximo de niveles en el arbol.Puede perjudicar si
// existen muchos niveles en el arbol ya que la caja puede llegar a ser tan pequena que
// el radio de las pelotas esten contenidas en muchas cajas a la vez.
const int MAX_OCTREE_DEPTH = 6;
// Si removemos pelotas de los nodos y queda por debajo de este numero entonces lo que
// vamos hacer es juntarlo ese nodo.
const int MIN_BALLS_PER_OCTREE = 2;	 
// Queremos tener un maximo de pelotas por nodo en el octree.Si existen mas pelotas
// entonces lo que vamos hacer es dividirlo mas.
const int MAX_BALLS_PER_OCTREE = 10;

const float BOX_SIZE = 8.0f;
#define DEPTH 4


// Guarda informacion referente a una pelota.
typedef VECTOR4D Point;

// Guarda todos los tipos de paredes.
enum Wall { WALL_LEFT, WALL_RIGHT, WALL_FAR, WALL_NEAR, WALL_TOP, WALL_BOTTOM };

// Guarda un par de apuntadores a las pelotas. Esto es usado para despues indicar
// colisiones potenciales entre dos pelotas.
struct PointPair {
	Point* Point1;
	Point* Point2;
};

// Guarda un par de apuntadores de una pelota y una pared. Esto es usado para despues
// indicar colisiones potenciales entre una pelota y una pared.
struct PointWallPair {
	Point* Point;
	Wall wall;
};


class COctree {
private:
	// Estos 3 vectores representan las esquinas del cubo en el octree y tambien el centro.
	VECTOR4D corner1; //(minX, minY, minZ)
	VECTOR4D corner2; //(maxX, maxY, maxZ)
	VECTOR4D center;//((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2)

	// Guarda los hijos si es que hay alguno.Basicamente usamos un arreglo para guardar los
	// 8 hijos del octree.children[0][*][*] son los hijos con cordenada x que van desde
	// minX hasta centerX.children[1][*][*] son los hijos con cordenada x que van desde
	// centerX hasta maxX.Esto aplica para las otras dos dimensiones.
	COctree *children[2][2][2];
	// Nos indica si un nodo en particular tiene algun hijo.
	bool hasChildren;
	// Guarda todas las pelotas en un nodo en particular si este nodo no tiene ningun hijo.
	set<Point*> Points;
	// El nivel por el que nos encontramos en el Octree.
	int depth;

	CDXBasicPainter *m_pPainter;

public:
	// El numero total de pelotas que contiene el nodo.Tambien incluye aquellas guardadas
	// en sus hijos.Necesitamos tener muy encuenta este numero ya que nos va a indicar
	// cuando dividir o juntar los nodos.
	int numPoints;
private:
	// filePoint() se encarga de agregar o eliminar pelotas al nodo adecuado en el octree (se
	// basa en la posicion de la pelota). Basicamente va a encontra a cual hijo esa pelota
	// en particular pertenece y la va a remover o agregar dependiendo si @addPoint es
	// verdadera o falsa.
	//@param    Point		    Pelotas.
	//@param	pos				La posicion.
	//@param	addPoint			verdadero para agregar la pelota.
	void filePoint(Point* Point, VECTOR4D pos, bool addPoint);

	// Divide un cubo en particular a cubos mas pequenos. Es decir de un nodo crea a otros
	// nodos mas. Toma todos los hijos del nodo en cuestion y los agrega a los nodos hijo de
	// ese nodo.
	void haveChildren() ;

	// Se encarga de saber cuales bolas estan contenidas en un nodo particular incluyendo
	// sus hijos y los va a colocar en el set @bs. Si este nodo en cuestion
	// tiene hijos va a realizar una llamada recursiva hasta sus hijos que no tienen mas
	// hijos y va a colocar las bolas de cada nodo hijo en el set bs. Este
	// metodo se va a usar particularmente cuando queremos juntar los nodos.
	void collectPoints(set<Point*> &bs) ;

	// Destruye todos los hijos del nodo y mueve todas las pelotas al set de pelotas.
	void destroyChildren() ;

	// Remueve la pelota del octree de la posicion particular @pos.
	//@param    Point	Las pelotas.
	//@param	pos		La posicion.
	void remove(Point* Point, VECTOR4D pos) ;

	// Este metodo es llamado individualmente para cada una de las paredes de nuestra caja.
	// Guarda las colisiones potenciales en un vector @cs. Recursivamente se manda a llamar
	// en la mitad correcta del hijo. Una vez que ya no hay mas hijos se guarda en el vector
	// @cs las colisiones potenciales entre la pared y la pelota.
	//@param [in,out]	cs	Vector en donde guardamos la posibles colisiones.
	//@param	w		  	Pared a procesar.
	//@param	coord	  	Las coordenadas.
	//@param	dir		  	The direccion.
	void potentialPointWallCollisions(vector<PointWallPair> &cs,
		Wall w, char coord, int dir);
public:
	// Constructor
	COctree(VECTOR4D c1, VECTOR4D c2, int d, CDXBasicPainter* pPainter );

	// Destructor
	~COctree();

	// Agrega una pelota al nodo.
	//@param Point Las pelotas.
	void add(Point* Point);

	// Remueve la pelota del nodo.
	//@param	Point	La pelota a remover.
	void remove(Point* Point);

	// Se manda a llamar cada vez que la pelota cambia de posicion.Esto sucede en
	// GLScene.cpp.Para hacer esto solamente eliminamos la pelota de la posicion antigua y
	// la agregamos en su nueva posicion.
	//@param	Point	Pelota para actualizar.
	//@param	oldPos			Posicion Antigua.
	void PointMoved(Point* Point, VECTOR4D oldPos);

	// Agrega colisiones potenciales entre pelotas al vector @collisions Si
	// el nodo tiene hijos entonces se navega a sus nodos hijos y se manda a llamar
	// recursivamente la funcion hasta que no tenga mas hijos y en ese caso iteramos
	// sobre el set de pelotas para agregarlas al vector de @collisions.
	void potentialPointPointCollisions(vector<PointPair> &collisions) ;

	// Agrega las colisiones potenciales entre cada pared y las pelotas. 
	void potentialPointWallCollisions(vector<PointWallPair> &collisions);

	// Nos ayuda a debuguear la creacion del arbol
	void printCHildren(int tab, COctree* raiz);

	void DrawOctree();
	
};

//Va a encontrar todas las colisiones posibles entre pares de pelotas. Va a
//guardar los resultados en el vector @potentialCollisions. Realiza esto al 
//llamar uno de los metodos del Octree potentialPointPointCollisions()
void potentialPointPointCollisions(vector<PointPair> &potentialCollisions,
	vector<Point*> &Points, COctree* octree);

//Busca todas las posibles colisiones entre una pelota y una pared. Va a guardar
//los resultados en el vector @potentialCollisions. Realiza esto al llamar uno
//de los metodos del Octree potentialPointWallCollsions()
void potentialPointWallCollisions(vector<PointWallPair> &potentialCollisions,
	vector<Point*> &Points, COctree* octree);

// Prueba si dos pelotas estan colisionando una contra la otra. Hace esto dependiendo
// si la distancia de cualquiera de las dos pelotas es menor que la suma de los radios.
// Si estan muy cerca queremos hacer una revisada extra de que si estan colisionando.
// Si las pelotas se estan moviendo en sentidos opuestos una de otra entonces decimos que
// no estan colisinando, porque significa que probablemente acaban de rebotar.
bool testPointPointCollision(Point* b1, Point* b2);

// Va a encontrar todas las colisiones potenciales entre pelotas usando 
// potentialPointPointCollsions(). Luego va a recorrer todo el vector @bsp que 
// contiene las colisiones potenciales y revisar si si hay colisiones con
// testPointPointCollision(). En el caso que si exista va a realizar las 
// matematicas para cambiar la direccion de ambas pelotas y que reboten sin que
// pierdan su velocidad.
void handlePointPointCollisions(vector<Point*> &Points, COctree* octree);


// Regresa la direccion del centro del cubo a una pared en particular.
VECTOR4D wallDirection(Wall wall);

// Prueba si una pelota y una pared estan realmente colisionando. Revisa esto al
// sumar la posicion actual de la pelota mas su radio es mayor que el centro de
// de la caja. Tambien hacemos una revision etra al ver si la pelota acaba o no
// de rebotar sobre la pared.
bool testPointWallCollision(Point* Point, Wall wall);

//Va a encontrar todas las colisiones potenciales entre pelotas y paredes usando
//potentialPointWallCollisions(). Despues va a iterar sobre ese vector llamando
//testPointWallCollision() para saber si es en verdad una colision. De ser asi,
//refleja la pelota fuera de la pared sin perder su velocidad.
void handlePointWallCollisions(vector<Point*> &Points, COctree* octree);

// Prueba todos los escenarios posibles. Todas las colisiones entre pelotas. 
void iterativeMethodPointPointCollision(vector<PointPair> &potentialCollisions, vector<Point*> &Points);

// Prueba todos los escenarios posibles. Todas las colisiones entre pelotas y paredes. 
void iterativeMethodPointWallCollision(vector<PointWallPair> &potentialCollisions, vector<Point*> &Points);
