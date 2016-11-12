#include "stdafx.h"
#include "Octree.h"

// Constructor del octree.
//@param	c1	Esquina inferior.
//@param	c2	Esquina superior.
//@param	d 	La profundidad del nodo.
COctree::COctree(VECTOR4D c1, VECTOR4D c2, int d, CDXBasicPainter* pPainter) {
	m_pPainter = pPainter;
	corner1 = c1;
	corner2 = c2;
	center = (c1 + c2) / 2.0f;
	depth = d;
	numPoints = 0;
	hasChildren = false;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				children[i][j][k] = NULL;
			}
		}
	}
}

// Destructor
COctree::~COctree() {
	if (hasChildren) {
		destroyChildren();
	}
}

// fileBall() se encarga de agregar o eliminar pelotas al nodo adecuado en el octree (se
// basa en la posicion de la pelota). Basicamente va a encontra a cual hijo esa pelota
// en particular pertenece y la va a remover o agregar dependiendo si @addBall es
// verdadera o falsa.
//@param    Point		    Pelotas.
//@param	pos				La posicion.
//@param	addBall			verdadero para agregar la pelota.

void COctree::filePoint(Point* point, VECTOR4D pos, bool addBall) {
	// Determina en cual de sus hijos la pelota pertenece.
	for (int x = 0; x < 2; x++) {
		if (x == 0) {
			if (pos.v[0]  > center.v[0]) {
				continue;
			}
		}
		else if (pos.v[0]  < center.v[0]) {
			continue;
		}


		for (int y = 0; y < 2; y++) {
			if (y == 0) {
				if (pos.v[1] > center.v[1]) {
					continue;
				}
			}
			else if (pos.v[1] < center.v[1]) {
				continue;
			}

			for (int z = 0; z < 2; z++) {
				if (z == 0) {
					if (pos.v[2]  > center.v[2]) {
						continue;
					}
				}
				else if (pos.v[2] < center.v[2]) {
					continue;
				}

				// Agrega o elimina la pelota.
				if (addBall) {
					children[x][y][z]->add(point);
				}
				else {
					children[x][y][z]->remove(point, pos);
				}
			}
		}
	}
}

// Divide un cubo en particular a cubos mas pequenos. Es decir de un nodo crea a otros
// nodos mas. Toma todos los hijos del nodo en cuestion y los agrega a los nodos hijo de
// ese nodo.

void COctree::haveChildren() {
	for (int x = 0; x < 2; x++) {
		float minX;
		float maxX;
		if (x == 0) {
			minX = corner1.v[0];
			maxX = center.v[0];
		}
		else {
			minX = center.v[0];
			maxX = corner2.v[0];
		}

		for (int y = 0; y < 2; y++) {
			float minY;
			float maxY;
			if (y == 0) {
				minY = corner1.v[1];
				maxY = center.v[1];
			}
			else {
				minY = center.v[1];
				maxY = corner2.v[1];
			}

			for (int z = 0; z < 2; z++) {
				float minZ;
				float maxZ;
				if (z == 0) {
					minZ = corner1.v[2];
					maxZ = center.v[2];
				}
				else {
					minZ = center.v[2];
					maxZ = corner2.v[2];
				}
				//Crea un nuevo octree y la profundidad se incrementa en 1.
				children[x][y][z] = new COctree({ minX, minY, minZ ,1},
												 { maxX, maxY, maxZ },
												  depth + 1,
												m_pPainter);
			}
		}
	}

	// Itera sobre todas las bolas del antiguo nodo y las va colocando en los nuevos hijos con fileBall().
	for (set<Point*>::iterator it = Points.begin(); it != Points.end();
		it++) {
		Point* point = *it;
		filePoint(point, (VECTOR4D)*point, true);
	}
	// Elimina todas las bolas de ese nodo en particula ya que fueron colocadas en sus hijos.
	Points.clear();
	// Elimina todas las bolas de ese nodo en particula ya que fueron colocadas en sus hijos.
	hasChildren = true;
}

// Se encarga de saber cuales bolas estan contenidas en un nodo particular incluyendo
// sus hijos y los va a colocar en el set @bs. Si este nodo en cuestion
// tiene hijos va a realizar una llamada recursiva hasta sus hijos que no tienen mas
// hijos y va a colocar las bolas de cada nodo hijo en el set bs. Este
// metodo se va a usar particularmente cuando queremos juntar los nodos.
void COctree::collectPoints(set<Point*> &bs) {
	if (hasChildren) {
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					children[x][y][z]->collectPoints(bs);
				}
			}
		}
	}
	else {
		for (set<Point*>::iterator it = Points.begin(); it != Points.end();
			it++) {
			Point* point = *it;
			bs.insert(point);
		}
	}
}

// Destruye todos los hijos del nodo y mueve todas las pelotas al set de pelotas.
void COctree::destroyChildren() {
	// Destruye todos los hijos del nodo y mueve todas las pelotas al set de pelotas.
	collectPoints(Points);
	// Una vez que tenemos guardadas las pelotas procedemos a eliminar sus hijos.
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				delete children[x][y][z];
				children[x][y][z] = NULL;
			}
		}
	}
	// Una vez que tenemos guardadas las pelotas procedemos a eliminar sus hijos.
	hasChildren = false;
}

// Remueve la pelota del octree de la posicion particular @pos.
//@param    Point	Las pelotas.
//@param	pos		La posicion.
void COctree::remove(Point* Point, VECTOR4D pos) {
	// Decrementamos el numero de @numBalls.
	numPoints--;
	// Si el numero de @numBalls es menor al que establecimos en un principio que debe contener
	// cada nodo procedemos a destruir ese nodo.
	if (hasChildren && numPoints < MIN_BALLS_PER_OCTREE) {
		destroyChildren();
	}
	// De lo contrario si todavia es mayor al numero menor de pelotas por caja, procedemos a
	// eliminar esa pelota con esa posicion del nodo. Por eso ponemos false en fileBall()
	if (hasChildren) {
		filePoint(Point, pos, false);
	}
	// Si no tiene hijos entonces simplemente la borramos de este nodo en particular.
	else {
		Points.erase(Point);
	}
}

// Agrega una pelota al nodo.
//@param Point Las pelotas.
void COctree::add(Point* point) {
	numPoints++;
	// Si el numero de @numBalls es mayor que el numero maximo permitido por MAX_BALLS_PER_OCTREE y
	// la profundidad es menor que el permitido por MAX_OCTREE_DEPTH y ese nodo no tiene hijos
	// entonces se divide el nodo con haveChildren()
	if (!hasChildren && depth < MAX_OCTREE_DEPTH &&
		numPoints > MAX_BALLS_PER_OCTREE) {
		haveChildren();
	}
	// Si el nodo tiene hijos simplemente se agrega la pelota y su posicion. De lo contrario
	// solamente se agrega la pelota al set.
	if (hasChildren) {
		filePoint(point, (VECTOR4D)*point, true);
	}
	else {
		Points.insert(point);
	}
}

// Remueve la pelota del nodo.Hace una llamada al otro metodo remove.
//@param	Point	La pelota a remover.
void COctree::remove(Point* Point) {
	remove(Point, (VECTOR4D)*Point);
}

// Se manda a llamar cada vez que la pelota cambia de posicion.Esto sucede en
// GLScene.cpp.Para hacer esto solamente eliminamos la pelota de la posicion antigua y
// la agregamos en su nueva posicion.
//@param	Point	Pelota para actualizar.
//@param	oldPos			Posicion Antigua.
void COctree::PointMoved(Point* Point, VECTOR4D oldPos) {
	remove(Point, oldPos);
	add(Point);
}
//Agrega colisiones potenciales entre pelotas al vector @collisions Si
//el nodo tiene hijos entonces se navega a sus nodos hijos y se manda a llamar
//recursivamente la funcion hasta que no tenga mas hijos y en ese caso iteramos
//sobre el set de pelotas para agregarlas al vector de @collisions.
void COctree::potentialPointPointCollisions(vector<PointPair> &collisions) {
	if (hasChildren) {
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					children[x][y][z]->
						potentialPointPointCollisions(collisions);
				}
			}
		}
	}
	else {
		// Agregamos todos los pares de pelotas.
		for (set<Point*>::iterator it = Points.begin(); it != Points.end();it++) {
			Point* ball1 = *it;
			for (set<Point*>::iterator it2 = Points.begin();it2 != Points.end(); it2++) {
				Point* ball2 = *it2;
				// Nos aseguramos que agregamos cada par solamente una vez.
				if (ball1 < ball2) {
					PointPair bp;
					bp.Point1 = ball1;
					bp.Point2 = ball2;
					collisions.push_back(bp);
				}
			}
		}
	}
}


// Nos ayuda a debuguear la creacion del arbol
void COctree::printCHildren(int tab, COctree* raiz)
{

	if (raiz == NULL)
	{
		return;
	}
	for (int i = 0; i < tab; i++)printf(" ");


	printf("%i, E1: [%.2f][%.2f][%.2f], E2: [%.2f][%.2f][%.2f]\n", raiz->numPoints, raiz->corner1.v[0], raiz->corner1.v[1], raiz->corner1.v[2], raiz->corner2.v[0], raiz->corner2.v[1], raiz->corner2.v[2]);
	for (auto point:raiz->Points)
	{
		for (int i = 0; i < tab; i++)printf(" ");
		printf("PosBall: [%.2f][%.2f][%.2f]\n", point[0], point[1], point[2]);
	}
	if (hasChildren)
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					printCHildren(tab + 2, raiz->children[i][j][k]);
				}
			}
		}

}

void COctree::DrawOctree()
{

	VECTOR4D c1 = corner1;
	VECTOR4D c2 = corner2;

	CDXBasicPainter::VERTEX cube[8];
	unsigned long   m_lIndicesFrame[16];
	cube[0].Position = { c1.x,c1.y,c1.z,1 };
	cube[1].Position = { c1.x,c2.y,c1.z,1 };
	cube[2].Position = { c2.x,c1.y,c1.z,1 };
	cube[3].Position = { c2.x,c2.y,c1.z,1 };
	cube[4].Position = { c1.x,c1.y,c2.z,1 };
	cube[5].Position = { c1.x,c2.y,c2.z,1 };
	cube[6].Position = { c2.x,c1.y,c2.z,1 };
	cube[7].Position = { c2.x,c2.y,c2.z,1 };

	for (int i = 0; i < 8; i++)
		cube[i].Color = m_Color;

	m_lIndicesFrame[0] = 0;
	m_lIndicesFrame[1] = 1;
	m_lIndicesFrame[2] = 3;
	m_lIndicesFrame[3] = 2;
	m_lIndicesFrame[4] = 0;
	m_lIndicesFrame[5] = 4;
	m_lIndicesFrame[6] = 5;
	m_lIndicesFrame[7] = 1;
	m_lIndicesFrame[8] = 5;
	m_lIndicesFrame[9] = 7;
	m_lIndicesFrame[10] = 3;
	m_lIndicesFrame[11] = 2;
	m_lIndicesFrame[12] = 6;
	m_lIndicesFrame[13] = 7;
	m_lIndicesFrame[14] = 6;
	m_lIndicesFrame[15] = 4;

	m_pPainter->DrawIndexed(cube, 8, m_lIndicesFrame, 16, PAINTER_WITH_LINESTRIP);

	if (hasChildren)
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					children[i][j][k]-> DrawOctree();
				}
			}
		}

}

//Va a encontrar todas las colisiones posibles entre pares de pelotas. Va a
//guardar los resultados en el vector @potentialCollisions. Realiza esto al
//llamar uno de los metodos del Octree potentialBallBallCollisions()
void potentialPointPointCollisions(vector<PointPair> &potentialCollisions,
	vector<Point*> &balls, COctree* octree) {
	//Metodo Rapido
	octree->potentialPointPointCollisions(potentialCollisions);

	//Metodo Lento
	//iterativeMethodBallBallCollision(potentialCollisions, balls);
}

// Prueba si dos pelotas estan colisionando una contra la otra. Hace esto dependiendo
// si la distancia de cualquiera de las dos pelotas es menor que la suma de los radios.
// Si estan muy cerca queremos hacer una revisada extra de que si estan colisionando.
// Si las pelotas se estan moviendo en sentidos opuestos una de otra entonces decimos que
// no estan colisinando, porque significa que probablemente acaban de rebotar.
bool testBallBallCollision(Point* b1, Point* b2) {

	/*
	//Revisa si las pelotas estan lo suficientemente cerca.
	float r = b1->getRadius() + b2->getRadius();
	if ((b1->getPositionV() - b2->getPositionV()).magnitudeSquared() < r * r) {
		//Revisa si las pelotas se estan acercando hacia ellas.
		VECTOR4D netVelocity = b1->getVelocity() - b2->getVelocity();
		VECTOR4D displacement = b1->getPositionV() - b2->getPositionV();
		return Dot(netVelocity, displacement) < 0;
	}
	else
		return false;
		*/
	return false;
}

// Va a encontrar todas las colisiones potenciales entre pelotas usando
// potentialBallBallCollsions(). Luego va a recorrer todo el vector @bsp que
// contiene las colisiones potenciales y revisar si si hay colisiones con
// testBallBallCollision(). En el caso que si exista va a realizar las
// matematicas para cambiar la direccion de ambas pelotas y que reboten sin que
// pierdan su velocidad.
void handleBallBallCollisions(vector<Point*> &balls, COctree* octree) {
	vector<PointPair> bps;
	potentialPointPointCollisions(bps, balls, octree);

	for (unsigned int i = 0; i < bps.size(); i++) {
		PointPair bp = bps[i];

		Point* b1 = bp.Point1;
		Point* b2 = bp.Point2;
		if (testBallBallCollision(b1, b2)) {
			// Hacemos que las pelotas parezcan que rebotan.
			VECTOR4D displacement = Normalize(*b1 - *b2) ;
			//b1->setVelocity(b1->getVelocity() - ((displacement * 2) * dot(b1->getVelocity(), displacement)) );
			//b2->setVelocity(b2->getVelocity() - ((displacement * 2) * dot(b2->getVelocity(), displacement)) );
		}
	}
}



