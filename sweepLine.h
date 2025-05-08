#ifndef SWEEP_H
#define SWEEP_H

#include "io.h"
#include <queue>
#include <set>

enum EventType { LEFT_ENDPOINT, RIGHT_ENDPOINT, INTERSECTION };
typedef struct {
    HalfEdge* halfEdge;                     // Ponteiro para a semi-aresta
    Vertice* vertex;                        // Ponteiro para o vértice
    EventType type;
    bool isStart;                           // Indica se o evento é um ponto de início (true) ou um ponto de fim (false)
    bool isIntersection;                    // Indica se o evento é uma interseção (true) ou não (false)
    bool isVertex;                          // Indica se o evento é um vértice (true) ou não (false)
} Event;


typedef struct {
    bool operator()(const Event* e1, const Event* e2) const {
        if (e1->vertex->y != e2->vertex->y) {
            return e1->vertex->y > e2->vertex->y; // Ordena por coordenada y
        } else {
            return e1->vertex->x > e2->vertex->x; // Ordena por coordenada x
        }
    }
} EventComparator;

typedef struct {
    bool operator()(const HalfEdge* h1, const HalfEdge* h2) const {
        if (h1->origin->y != h2->origin->y) {
            return h1->origin->y > h2->origin->y; // Ordena por coordenada y
        } else {
            return h1->origin->x > h2->origin->x; // Ordena por coordenada x
        }
    }
} SetComparator;


class SweepLine {
public:
    SweepLine() {};
    ~SweepLine() {};

    bool findIntersection(HALF_EDGES halfEdges);
    bool findIntersection(HalfEdge* h1, HalfEdge* h2);

private:
    set<HalfEdge*, SetComparator> status;                           // Árvore de busca balanceada
    priority_queue<Event*, vector<Event*>, EventComparator> eventQueue; // Lista prioritária de eventos 

    void insertHalfEdgesHeap(HalfEdge* h);
    void handleLeftEndPoint(Event* e);
    void addEvent(Event* e);
    void removeEvent(Event* e);
    void processEvent(Vertice* point);
};

#endif