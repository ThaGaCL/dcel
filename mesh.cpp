#include "mesh.h"

bool Mesh::checkIfVertexExists(int x, int y, int z){
    for (Vertice* v : vertices){
        if (v->x == x && v->y == y && v->z == z){
            return true;
        }
    }
    return false;
}

Vertice* Mesh::createNewVertex(int x, int y, int z){
    Vertice* v = nullptr;

    if (checkIfVertexExists(x, y, z))
        return nullptr;

    v = new Vertice;
    v->x = x;
    v->y = y;
    v->z = z;
    v->halfEdge = nullptr;
    v->idx = nVertices++; 
    vertices.push_back(v);

    return v;
}

void Mesh::defineFace(Vertice* v1, Vertice* v2, Vertice* v3, int idx){
    Face* f = createNewFace(idx);
    if (!f) return;

    HalfEdge* he1 = createHalfEdgeNode(v1, f->idx, nHalfEdges++);
    HalfEdge* he2 = createHalfEdgeNode(v2, f->idx, nHalfEdges++);
    HalfEdge* he3 = createHalfEdgeNode(v3, f->idx, nHalfEdges++);

    he1->next = he2; he1->prev = he3;
    he2->next = he3; he2->prev = he1;
    he3->next = he1; he3->prev = he2;
}

void Mesh::findTwin(HalfEdge* he) {
    Vertice* from = he->origin;
    Vertice* to = he->next->origin;

    for (HalfEdge* candidate : halfEdges) {
        if (candidate == he || candidate->twin != nullptr)
            continue;

        if (candidate->origin == to &&
            candidate->next->origin == from &&
            candidate->leftFace != he->leftFace)
        {
            he->twin = candidate;
            candidate->twin = he;
            printHalfEdge(he);
            break;
        }
    }
}

// os primeiros três vértices formam um plano, enquanto o quarto vértice é o topo do tetraedro
void Mesh::loadTetrahedron(Vertice* v1, Vertice* v2, Vertice* v3, Vertice* v4){
    if (!v1 || !v2 || !v3 || !v4) return;
    
    defineFace(v1, v2, v3, nFaces++);
    defineFace(v4, v1, v3, nFaces++);
    defineFace(v3, v2, v4, nFaces++);
    defineFace(v1, v4, v2, nFaces++);

    for (HalfEdge* he : halfEdges){
        findTwin(he);
    }
}

bool Mesh::faceDoesExist(int idx){
    if (faces.size() == 0){
        return false;
    }

    if (faces[idx]) return true;
    return false;
}

Face* Mesh::createNewFace(int idx){
    Face* face = new Face;
    face->halfEdge = nullptr; 
    face->idx = idx;

    faces.push_back(face);

    return face;
}
void Mesh::printHalfEdge(HalfEdge* he){
    if (!he) {
        std::cout << "HalfEdge: NULL" << std::endl;
        return;
    }
    std::cout << "HalfEdge: " << he << std::endl;
    if (he->origin)
        std::cout << "Origin: " << he->origin->x << " " << he->origin->y << " " << he->origin->z << std::endl;
    else
        std::cout << "Origin: NULL" << std::endl;

    if (he->leftFace)
        std::cout << "Left Face: " << he->leftFace->idx + 1<< std::endl;
    else
        std::cout << "Left Face: NULL" << std::endl;

    if (he->next && he->next->origin)
        std::cout << "Next: " << he->next->origin->x << " " << he->next->origin->y << " " << he->next->origin->z << std::endl;
    else
        std::cout << "Next: NULL" << std::endl;

    if (he->prev && he->prev->origin)
        std::cout << "Prev: " << he->prev->origin->x << " " << he->prev->origin->y << " " << he->prev->origin->z << std::endl;
    else
        std::cout << "Prev: NULL" << std::endl;

    if (he->twin && he->twin->origin)
        std::cout << "Twin: " << he->twin->origin->x << " " << he->twin->origin->y << " " << he->twin->origin->z << std::endl;
    else
        std::cout << "Twin: NULL" << std::endl;
}

/**
    Função que cria uma nova semi-aresta
 */
HalfEdge* Mesh::createHalfEdgeNode(Vertice* origin, int faceIdx, int idx){
    HalfEdge* he = new HalfEdge;
	he->twin = NULL;
	he->next = NULL;
	he->prev = NULL;
    he->origin = origin;

    if (!he->origin->halfEdge)
        he->origin->halfEdge = he;

    he->idx = idx;

    he->leftFace = faces[faceIdx];
    faces[faceIdx]->halfEdge = he;

    halfEdges.push_back(he);
    return he;
}

// to-do: constructInitialHalfEdges()

/*
    Para saber o próximo, é necessário checar a origem da simétrica da semi-aresta i
    e então verificar na mesma face de i qual semi-aresta tem a mesma origem da simétrica
*/
void Mesh::findNext(HalfEdge* he){
    HalfEdge* twin = he->twin;
    HalfEdge* next = nullptr;

    for (HalfEdge* h : halfEdges){
        if (h->leftFace == he->leftFace && h->origin == twin->origin){
            next = h;
            break;
        }
    } 

    he->next = next;
}

/*
    Para saber o anterior, é necessário checar a origem da semi-aresta i
    e então a simétrica que sai do mesmo vértice tem que ter a mesma face que a semi-aresta i
*/
void Mesh::findPrev(HalfEdge* he){
    HalfEdge* prev = nullptr;

    for (HalfEdge* h : halfEdges){
        if (h->leftFace == he->leftFace && h->next == he){
            prev = h;
            break;
        }
    }

    he->prev = prev;
}

HalfEdge* isPointColinearWithFace(Face* f, Vertice* v){
    HalfEdge* he = f->halfEdge;

    do {
        // Verifica se o ponto está colinear com a semi-aresta
        if (orientation(he->origin, he->next->origin, v) == 0 && onSegment(he->origin, v, he->next->origin)) {
            return he; 
        }
        he = he->next;
    } while (he != f->halfEdge);

    return nullptr;
}

/*
    Função que verifica se um vértice está dentro de uma face
    A ideia é verificar se o ponto está dentro do polígono definido pela face
*/
int Mesh::isPointLeft(Face* f, Vertice* v){
    HalfEdge* temp = f->halfEdge; 
    int count = 0;
    do {
        // Vertifica se o ponto está à esquerda da semi-aresta
        if (orientation(temp->origin, temp->next->origin, v) == 2)
            count++;

        temp = temp->next;
    } while (temp != f->halfEdge);

    if (count % 2 == 1) {
        return true; // O ponto está dentro da face
    }
}

/*
    Insere um HalfEdge colinar entre duas semi-arestas
*/
void Mesh::insertNewHalfEdgeBetweenEdges(Face* f, HalfEdge* newHe, HalfEdge* prevHe){
    HalfEdge* nextTemp = prevHe->next;
    newHe->leftFace = f;

    prevHe->next = newHe;
    newHe->prev = prevHe;

    newHe->next = nextTemp;
    nextTemp->prev = newHe;
}

/*
    Função que contrói uma nova semi-aresta colinear
*/
void Mesh::constructNewColinearHalfEdge(Face* f, Vertice* v, HalfEdge* he){
    HalfEdge* newHe = createHalfEdgeNode(v, f->idx, nHalfEdges++);
    HalfEdge* newTwin = createHalfEdgeNode(v, f->idx, nHalfEdges++);    // revisar - f->idx
    newHe->twin = newTwin;
    newTwin->twin = newHe;

    insertNewHalfEdgeBetweenEdges(f, newHe, he);
    insertNewHalfEdgeBetweenEdges(f, newTwin, he->twin);
}

/*
    identificar se é aberta : alguma aresta é fronteira de somente uma face
*/
bool Mesh::isOpen(){
    HalfEdge* temp;
    Face* f;

    // A topologia de cada face é percorrida pela lista circular
    for (unsigned int i = 0; i < nFaces; i++){
        f = faces[i];
        if (!f)
        return true;
    
        temp = f->halfEdge;

        if (f->halfEdge == NULL || f->halfEdge->next == NULL)
            return true;
        
        while (temp->next != NULL && temp->next != f->halfEdge){
            if (temp->twin == NULL)
                return true;
            if (temp->twin->leftFace == NULL || temp->twin->leftFace == f)
                return true;
            temp = temp->next;
        }
    }

    return false;
}

/*
    identificar se não é subdivisão planar : alguma aresta é fronteira de mais de duas faces
*/
bool Mesh::isSubdivPlanar(){
    HalfEdge* temp;
    Face* f;
    FACES facesCollected;
    int nDiff;

    // A topologia de cada face é percorrida pela lista circular
    // A ideia é coletar as faces que estão ligadas a uma aresta
    for (unsigned int i = 0; i < nFaces; i++){
        f = faces[i];
        temp = f->halfEdge;

        while (temp->next != f->halfEdge){
            if (temp->twin == NULL)
                return true;

            // Se a semi-aresta já foi visitada, não precisa verificar novamente
            if (temp->twin->leftFace == f)
                continue;

            // Coletar as faces que estão ligadas a uma aresta
            nDiff = 0;
            for (HalfEdge* he : halfEdges){
                if (he->origin == temp->origin && he->leftFace != f){
                    facesCollected.push_back(he->leftFace);
                    nDiff++;
                }
            }

            if (nDiff > 2)
                return true;

            temp = temp->next;
        }

    }

    return false; 
}

/*
    Identifica se há auto-intersecção com o algoritmo sweep line
*/
bool Mesh::isOverlapped(){
    SweepLine* sl = new SweepLine();

    if (sl->findIntersection(halfEdges))
        return true;

    return false;
}

/*
    Identifica se a malha é válida
    Se a malha não for válida, retorna false
    Se a malha for válida, retorna true
*/
bool Mesh::isTopologyValid(){
    // identificar se é aberta : alguma aresta é fronteira de somente uma face
    // identificar se não é subdivisão planar : alguma aresta é fronteira de mais de duas faces
    // identificar se é superposta : alguma face tem auto-interseção ou intersecta outras faces
    // caso esteja tudo certo, retornar true

    if (isOpen()) {
        printf("aberta\n");
        return false;
    };
    if (isSubdivPlanar()) {
        printf("não subdivisão planar\n");
        return false;
    };
    if (isOverlapped()) {
        printf("superposta\n");
        return false;
    };

    return true;
}

/*
    Função que imprime a malha
    A primeira linha contém o número de vértices, arestas e faces
    A segunda linha imprime a struct Vertice
    A terceira linha imprime a struct Face
    A quarta linha imprime a struct HalfEdge
*/
void Mesh::printDCEL(){
    printf("%d %d %d\n", nVertices, nHalfEdges/2, nFaces);

    for (unsigned int i = 0; i < nVertices; i++){
        printf("%d %d %d\n", vertices[i]->x, vertices[i]->y, vertices[i]->halfEdge->idx+1);
    }

    for (unsigned int i = 0; i < nFaces; i++){
        printf("%d\n", faces[i]->halfEdge->idx+1);
    }

    for (unsigned int i = 0; i < nHalfEdges; i++){
        printf("%d %d %d %d %d\n", halfEdges[i]->origin->idx+1, halfEdges[i]->twin->idx+1, halfEdges[i]->leftFace->idx+1, halfEdges[i]->next->idx+1, halfEdges[i]->prev->idx+1);
    }
}