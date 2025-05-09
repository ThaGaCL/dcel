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