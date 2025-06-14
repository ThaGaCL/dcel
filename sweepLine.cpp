#include "sweepLine.h"

/*
    Função que cria um novo evento
    Retorna o evento criado
*/
Event* SweepLine::createNewEvent(Point* origin, Point* destination, EventType t){
    Event* e = new Event();
    e->edge = new sweepLineEdge();
    e->edge->origin = origin;
    e->edge->destination = destination;
    e->type = t;

    return e;
}

Point* SweepLine::searchPoint(int x, int y, int z){
    for (Point* p : points){
        if (p->x == x && p->y == y && p->z == z){
            return p;
        }
    }
    return nullptr;
}

/*
    Função que cria um novo ponto
    Retorna o ponto criado
*/
Point* SweepLine::createNewPoint(int x, int y, int z){
    Point* p = searchPoint(x, y, z);
    if (p) return p;

    p = new Point();
    p->x = x;
    p->y = y;
    p->z = z;

    points.push_back(p);
    return p;
}


/*
    Função que cria uma nova aresta
*/
sweepLineEdge* SweepLine::createNewsweepLineEdge(Point* origin, Point* destination) {
    sweepLineEdge* e = new sweepLineEdge();
    e->origin = origin;
    e->destination = destination;

    return e;
}


/*
    Função que insere os eventos na fila de eventos
    A fila de eventos é uma min-heap, onde o menor elemento é o primeiro a ser removido
*/
void SweepLine::insertsweepLineEdgesHeap(sweepLineEdge* edge) {
    if (!edge) return;

    Point* left = edge->origin;
    Point* right = edge->destination;

    if ((left->x < right->x) || ((left->x == right->x) && (left->y < right->y))){
        eventQueue.push(createNewEvent(left, right, LEFT_ENDPOINT));
        eventQueue.push(createNewEvent(right, left, RIGHT_ENDPOINT));
    } else {
        eventQueue.push(createNewEvent(left, right, RIGHT_ENDPOINT));
        eventQueue.push(createNewEvent(right, left, LEFT_ENDPOINT));
    }

}

/*
    Função que verifica a orientação dos pontos
    Retorna 0 se colinear, 1 se horário e 2 se anti-horário
*/
int orientation(Point* a, Point* b, Point* c) {
    int val = (b->y - a->y) * (c->x - b->x) - (b->x - a->x) * (c->y - b->y);
    if (val == 0) return 0; // Casos colineares
    return (val > 0) ? 1 : 2; // Horário ou anti-horário
}

bool onSegment(Point* p, Point* q, Point* r){
    return (q->x <= std::max(p->x, r->x) && q->x >= std::min(p->x, r->x) &&
            q->y <= std::max(p->y, r->y) && q->y >= std::min(p->y, r->y));
}

bool intersection(sweepLineEdge& prev, sweepLineEdge& current) {
    // Pular caso as arestas são irmãs (dividem um endpoint)
    // if (prev.origin == current.twin->origin || current.origin == prev.twin->origin) {
    //     return false;
    // }

    Point* p1 = prev.origin;
    Point* q1 = prev.destination;
    Point* p2 = current.origin;
    Point* q2 = current.destination;

    if (p1 == p2 || p1 == q2 || q1 == p2 || q1 == q2) {
        return false;
    }

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4) {
        return true;
    }

    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;
};

void printStatus(set<sweepLineEdge*, SetComparator>& status){
    printf("status: ");
    for (auto it = status.begin(); it != status.end(); ++it) {
        printf("(%d, %d)-(%d, %d) ", (*it)->origin->x, (*it)->origin->y, (*it)->destination->x, (*it)->destination->y);
    }
    printf("\n");
}

/*
    Função que adiciona um evento na lista de eventos
    Retorna true se houver interseção
*/
bool SweepLine::addEvent(Event* e){
    auto first = status.insert(e->edge).first;
    // printStatus(status);
    
    // caso não seja o primeiro nodo, é necessário verificar intersecção 
    if (first != status.begin()){
        auto prev = first; prev--;
        if (intersection(**prev, *e->edge)){
            return true;
        }
        
        auto next = first; next++;
        if (next != status.end()){
            if (intersection(**next, *e->edge)){
                return true;
            }
        }
    }
    return false;
}

/*
    Função que remove um evento da lista de eventos
    Retorna true se houver interseção
*/
bool SweepLine::removeEvent(Event* e){
    auto node = status.find(e->edge);
    if (node == status.end()) return false;

    auto prev = node;
    auto next = node;
    bool has_prev = (node != status.begin());
    bool has_next = (++next != status.end());

    if (has_prev) --prev;
    status.erase(node);

    if (has_prev && has_next) {
        if (intersection(**prev, **next)) {
            return true;
        }
    }

    return false;
}

/*
    Libera a memória de todos os eventos restantes na fila de eventos
*/
void SweepLine::freeEventQueue() {
    while (!eventQueue.empty()) {
        Event* e = eventQueue.top();
        eventQueue.pop();
        freeEvent(e);
    }
}

/*
    Função que libera a memória de um evento
*/
void SweepLine::freeEvent(Event* e) {
    if (e) destroyEvent(e);
    
}

/*
    Libera a memória do vetor de pontos
*/
void SweepLine::freePoints(){
    for (Point* p : points){
        destroyPoint(p);
    }
}

/*
    Função que verifica se inicia o sweep line 
    Retorna true se houver interseção
*/
bool SweepLine::findIntersection(EDGES edges){
    for (sweepLineEdge* h : edges){
        insertsweepLineEdgesHeap(h);
    }

    while (!eventQueue.empty()){
        Event* e = eventQueue.top();
        sweepY = e->edge->origin->y;
        comparator.sweepY = sweepY;

        eventQueue.pop();

        switch (e->type){
            case LEFT_ENDPOINT:
                if (addEvent(e)) {
                    freeEvent(e);
                    freeEventQueue();
                    return true;
                }
                break;
            case RIGHT_ENDPOINT:
                if (removeEvent(e)) {
                    freeEvent(e);
                    freeEventQueue();
                    return true;
                }
                break;
            default:
                break;
        }
        freeEvent(e);
    }

    return false;
}