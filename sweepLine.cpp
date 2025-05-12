#include "sweepLine.h"

Event* createNewEvent(Vertice* v, EventType t, HalfEdge* h){
    Event* e = new Event();
    e->vertex = v;
    e->type = t;
    e->halfEdge = h;

    return e;
}

void SweepLine::insertHalfEdgesHeap(HalfEdge* h) {
    if (!h) return;

    Vertice* left = h->origin;
    Vertice* right = h->twin->origin;    

    if ((left->x < right->x) || ((left->x == right->x) && (left->y < right->y))){
        eventQueue.push(createNewEvent(left, LEFT_ENDPOINT, h));
        eventQueue.push(createNewEvent(right, RIGHT_ENDPOINT, h));
    } else {
        eventQueue.push(createNewEvent(left, RIGHT_ENDPOINT, h->twin));
        eventQueue.push(createNewEvent(right, LEFT_ENDPOINT, h->twin));
    }

}

int orientation(Vertice* a, Vertice* b, Vertice* c){
    int val = (b->y - a->y) * (c->x - b->x) - (b->x - a->x) * (c->y - b->y);
    if (val == 0) return 0; // Casos colineares
    return (val > 0) ? 1 : 2;
}

bool onSegment(Vertice* p, Vertice* q, Vertice* r){
    return (q->x <= std::max(p->x, r->x) && q->x >= std::min(p->x, r->x) &&
            q->y <= std::max(p->y, r->y) && q->y >= std::min(p->y, r->y));
}

bool intersection(HalfEdge& prev, HalfEdge& current){
    // Pular caso as arestas são irmãs (dividem um endpoint)
    // if (prev.origin == current.twin->origin || current.origin == prev.twin->origin) {
    //     return false;
    // }


    Vertice* p1 = prev.origin;
    Vertice* q1 = prev.twin->origin;
    Vertice* p2 = current.origin;
    Vertice* q2 = current.twin->origin;

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

void printStatus(set<HalfEdge*, SetComparator>& status){
    printf("status: ");
    for (auto it = status.begin(); it != status.end(); ++it) {
        printf("(%d, %d)-(%d, %d) ", (*it)->origin->x, (*it)->origin->y, (*it)->twin->origin->x, (*it)->twin->origin->y);
    }
    printf("\n");
}

bool SweepLine::addEvent(Event* e){
    auto first = status.insert(e->halfEdge).first;
    // printStatus(status);
    
    // caso não seja o primeiro nodo, é necessário verificar intersecção 
    if (first != status.begin()){
        auto prev = first; prev--;
        if (intersection(**prev, *e->halfEdge)){
            return true;
        }
        
        auto next = first; next++;
        if (next != status.end()){
            if (intersection(**next, *e->halfEdge)){
                return true;
            }
        }
    }
    return false;
}

bool SweepLine::removeEvent(Event* e){
    auto node = status.find(e->halfEdge);
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


bool SweepLine::findIntersection(HALF_EDGES halfEdges){
    for (HalfEdge* h : halfEdges){
        insertHalfEdgesHeap(h);    
    }

    while (!eventQueue.empty()){
        Event* e = eventQueue.top();
        sweepY = e->vertex->y;
        comparator.sweepY = sweepY;

        eventQueue.pop();

        switch (e->type){
            case LEFT_ENDPOINT:
                if (addEvent(e)) return true;
                break;
            case RIGHT_ENDPOINT:
                if (removeEvent(e)) return true;
                break;
            // case INTERSECTION:
            //     return true;
            default:
                break;
        }
    }

    return false;
}
