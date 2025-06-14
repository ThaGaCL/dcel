#ifndef SWEEP_H
#define SWEEP_H

/*
    sweepLine.h
    
    Este arquivo contém a implementação do algoritmo de varredura (sweep line) para detectar interseções em uma malha.
    O algoritmo utiliza uma árvore de busca balanceada para armazenar as semi-arestas ativas e uma fila de eventos
    para processar os eventos de interseção. A classe SweepLine é responsável por gerenciar esses dados e executar
    o algoritmo de varredura. A classe Event representa um evento na fila de eventos, enquanto a classe SetComparator
    é usada para comparar as semi-arestas ativas na árvore de busca. 

*/

#include "io.h"
#include <queue>
#include <set>


typedef struct Point {
    int x;
    int y;
    int z;
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
    bool operator>(const Point& other) const {
        return other < *this;
    }
} Point;

using POINTS = vector<Point*>;

typedef struct sweepLineEdge {
    int idx;
    Point* origin;
    Point* destination;
} sweepLineEdge;

using EDGES = std::vector<sweepLineEdge*>;

enum EventType { LEFT_ENDPOINT, RIGHT_ENDPOINT };
typedef struct {
    sweepLineEdge* edge;
    EventType type;
} Event;

typedef struct {
    bool operator()(const Event* e1, const Event* e2) const {
        if (e1->edge->origin != e2->edge->origin) {
            return e1->edge->origin > e2->edge->origin;
        }
        if (e1->edge->destination != e2->edge->destination) {
            return e1->edge->destination > e2->edge->destination;
        }

        return e1->type < e2->type;
    }
} EventComparator;

typedef struct {
    int sweepY;

    double getXAtY(sweepLineEdge* edge, int y) const {
        Point* p1 = edge->origin;
        Point* p2 = edge->destination;

        if (p1->y == p2->y) return p1->x;

        return p1->x + (double)(p2->x - p1->x) * (sweepY - p1->y) / (double)(p2->y - p1->y);
    }

    bool operator()(const sweepLineEdge* h1, const sweepLineEdge* h2) const {
        double x1 = getXAtY(const_cast<sweepLineEdge*>(h1), sweepY);
        double x2 = getXAtY(const_cast<sweepLineEdge*>(h2), sweepY);

        if (x1 != x2) return x1 < x2;

        return h1 < h2;
    }
} SetComparator;

int orientation(Vertice *a, Vertice *b, Vertice *c);
bool onSegment(Vertice* p, Vertice* q, Vertice* r);

class SweepLine {
public:
    SweepLine() {};
    ~SweepLine() {};
    int sweepY;
    int nIndex = 0;
    POINTS points;
    SetComparator comparator;
    Event* createNewEvent(Point* origin, Point* destination, EventType t);
    Point* createNewPoint(int x, int y, int z);
    sweepLineEdge* createNewsweepLineEdge(Point* origin, Point* destination);

    bool findIntersection(EDGES edges);


private:
    set<sweepLineEdge*, SetComparator> status;                               // Árvore de busca balanceada
    priority_queue<Event*, vector<Event*>, EventComparator> eventQueue; // Lista prioritária de eventos 

    // void insertHalfsweepLineEdgesHeap(sweepLineEdge* h);
    void insertsweepLineEdgesHeap(sweepLineEdge* sweepLineEdge);
    bool addEvent(Event* e);
    bool removeEvent(Event* e);
    Point* searchPoint(int x, int y, int z);

    void freeEvent(Event* e);
    void freeEventQueue();
    void freePoints();

    void destroyEvent(Event* e) {
        delete e;
    }
    void destroyPoint(Point* p) {
        delete p;
    }
};

#endif