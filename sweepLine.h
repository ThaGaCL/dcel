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

enum EventType { LEFT_ENDPOINT, RIGHT_ENDPOINT };
typedef struct {
    HalfEdge* halfEdge;                     // Ponteiro para a semi-aresta
    Vertice* vertex;                        // Ponteiro para o vértice
    EventType type;
} Event;

typedef struct {
    bool operator()(const Event* e1, const Event* e2) const {
        if (e1->vertex->y != e2->vertex->y) {
            return e1->vertex->y > e2->vertex->y; 
        }
        if (e1->vertex->x != e2->vertex->x) {
            return e1->vertex->x > e2->vertex->x; 
        }
        
        return e1->type < e2->type;
    }
} EventComparator;

typedef struct {
    int sweepY;

    double getXAtY(HalfEdge* he, int y) const {
        Vertice* p1 = he->origin;
        Vertice* p2 = he->twin->origin;

        if (p1->y == p2->y) return p1->x;

        return p1->x + (double)(p2->x - p1->x) * (sweepY - p1->y) / (double)(p2->y - p1->y);
    }

    bool operator()(const HalfEdge* h1, const HalfEdge* h2) const {
        double x1 = getXAtY(const_cast<HalfEdge*>(h1), sweepY);
        double x2 = getXAtY(const_cast<HalfEdge*>(h2), sweepY);
    
        if (x1 != x2) return x1 < x2;

        return h1 < h2;
    }
} SetComparator;


class SweepLine {
public:
    SweepLine() {};
    ~SweepLine() {};
    int sweepY;
    SetComparator comparator;

    bool findIntersection(HALF_EDGES halfEdges);

private:
    set<HalfEdge*, SetComparator> status;                               // Árvore de busca balanceada
    priority_queue<Event*, vector<Event*>, EventComparator> eventQueue; // Lista prioritária de eventos 

    void insertHalfEdgesHeap(HalfEdge* h);
    void handleLeftEndPoint(Event* e);
    void processEvent(Vertice* point);
    bool addEvent(Event* e);
    bool removeEvent(Event* e);
};

#endif