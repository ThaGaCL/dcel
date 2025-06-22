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

/*
    Cria uma nova face triangular com um vértice e uma semi-aresta
*/
Face* Mesh::createNewFace(Vertice* v1, Vertice* v2, Vertice* v3) {
    Face* f = createNewFace(nFaces++);
    if (!f) return NULL;
    
    // Now build the half-edges with corrected winding
    HalfEdge* he1 = createHalfEdgeNode(v2, f->idx, nHalfEdges++);
    HalfEdge* he2 = createHalfEdgeNode(v3, f->idx, nHalfEdges++);
    HalfEdge* he3 = createHalfEdgeNode(v1, f->idx, nHalfEdges++);

    // he3->twin = he->twin;

    he3->next = he1;
    he1->next = he2;
    he2->next = he3;

    he3->prev = he2;
    he1->prev = he3;
    he2->prev = he1;

    he3->leftFace = f;
    he1->leftFace = f;
    he2->leftFace = f;

    f->halfEdge = he1;

    return f;
}

Face* Mesh::createNewFace(Vertice* v1, Vertice* v2, Vertice* v3, Vertice* v4){
    Face* f = createNewFace(nFaces++);
    if (!f) return NULL;

    // Match the triangle orientation: he1 = v2, he2 = v3, he3 = v4, he4 = v1
    HalfEdge* he1 = createHalfEdgeNode(v2, f->idx, nHalfEdges++);
    HalfEdge* he2 = createHalfEdgeNode(v3, f->idx, nHalfEdges++);
    HalfEdge* he3 = createHalfEdgeNode(v4, f->idx, nHalfEdges++);
    HalfEdge* he4 = createHalfEdgeNode(v1, f->idx, nHalfEdges++);

    // Set up the circular linked list of half-edges
    he4->next = he1; he1->next = he2; he2->next = he3; he3->next = he4;
    he4->prev = he3; he1->prev = he4; he2->prev = he1; he3->prev = he2;

    // Set the left face for each half-edge
    he1->leftFace = f;
    he2->leftFace = f;
    he3->leftFace = f;
    he4->leftFace = f;

    // Set the half-edge for the face
    f->halfEdge = he1;

    return f;
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

Face* Mesh::createNewFace(int idx){
    Face* face = new Face;
    face->halfEdge = nullptr; 
    face->idx = idx;

    faces.push_back(face);

    return face;
}

void Mesh::findTwin(HalfEdge* he){
    HalfEdge* twin = NULL;

    for (HalfEdge* h : halfEdges){
        // Verifica se a semi-aresta é a simétrica
        if (h->origin == he->next->origin && h->next->origin == he->origin && h->leftFace != he->leftFace){
            he->twin = h;
            h->twin = he;
            break;
        }
    }
}

// os primeiros três vértices formam um plano, enquanto o quarto vértice é o topo do tetraedro
void Mesh::loadTetrahedron(Vertice* v1, Vertice* v2, Vertice* v3, Vertice* v4){
    if (!v1 || !v2 || !v3 || !v4) return;
    
    defineFace(v1, v2, v3, nFaces++);
    defineFace(v1, v4, v2, nFaces++);
    defineFace(v2, v4, v3, nFaces++);
    defineFace(v3, v4, v1, nFaces++);

    for (HalfEdge* he : halfEdges){
        findTwin(he);
    }
}

bool Mesh::faceDoesExist(int idx){
    if (faces.size() == 0){
        return false;
    }

    for (Face* f : faces){
        if (f->idx == idx){
            return true;
        }
    }

    return false;
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

Face* Mesh::findFace(int idx) {
    for (Face* f : faces) {
        if (f->idx == idx) {
            return f;
        }
    }
    return nullptr; 
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

    Face* face = findFace(faceIdx);
    if (!face) {
        delete he;
        return nullptr; 
    }

    he->leftFace = face;
    face->halfEdge = he;

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

void adjustIndexesAfterFaceRemoval(FACES& faces){
    for (unsigned int i = 0; i < faces.size(); i++){
        if (faces[i]) {
            faces[i]->idx = i;
        }
    }
}

void Mesh::removeHalfEdge(HalfEdge* he){
    if (!he) return;

    // Remove a semi-aresta da lista de semi-arestas
    for (auto it = halfEdges.begin(); it != halfEdges.end(); ++it) {
        if (*it == he) {
            halfEdges.erase(it);
            // nHalfEdges--;
            break;
        }
    }

    // Deleta a semi-aresta
    delete he;
    he = nullptr;
}

void Mesh::removeFace(Face* f){
    if (!f) return;

    // Remove a face da lista de faces
    for (auto it = faces.begin(); it != faces.end(); ++it) {
        if (*it == f) {
            faces.erase(it);
            break;
        }
    }

    // Remove as semi-arestas associadas à face
    HalfEdge* he = f->halfEdge;
    if (!he) return;
    do {
        // Remove a semi-aresta da lista de semi-arestas
        for (auto it = halfEdges.begin(); it != halfEdges.end(); ++it) {
            if (*it == he) {
                halfEdges.erase(it);
                // nHalfEdges--;
                break;
            }
        }
        he = he->next;
    } while (he != f->halfEdge);

    // Deleta a face
    delete f;
    f = nullptr;
}

void Mesh::removeFaceKeepHalfEdges(Face* f){
    if (!f) return;

    // Remove a face da lista de faces
    for (auto it = faces.begin(); it != faces.end(); ++it) {
        if (*it == f) {
            faces.erase(it);
            break;
        }
    }

    // Deleta a face
    delete f;
    f = nullptr;
}

void Mesh::unmergeFace(Face* quad) {
    HalfEdge* he1 = quad->halfEdge;
    HalfEdge* he2 = he1->next;
    HalfEdge* he3 = he2->next;
    HalfEdge* he4 = he3->next;

    Face* newFace = createNewFace(nFaces++);
    newFace->halfEdge = he1;
    quad->halfEdge = he4;

    HalfEdge* divisor = createHalfEdgeNode(he1->origin, newFace->idx, nHalfEdges++);
    HalfEdge* divisorTwin = createHalfEdgeNode(he3->origin, quad->idx, nHalfEdges++);

    divisor->twin = divisorTwin;
    divisorTwin->twin = divisor;

    he2->prev = he3;
    he2->next = divisor;
    divisor->prev = he2;
    divisor->next = he3;
    he3->prev = divisor;
    he3->next = he2;

    he4->prev = he1;
    he4->next = divisorTwin;
    divisorTwin->prev = he4;
    divisorTwin->next = he1;
    he1->prev = divisorTwin;
    he1->next = he4;

    he2->leftFace = newFace;
    he3->leftFace = newFace;
    he1->leftFace = quad;
    he4->leftFace = quad;
}


void Mesh::triangulateFaces() {
    vector<Face*> toTriangulate;
    for (Face* face : faces) {
        int count = 1;
        HalfEdge* start = face->halfEdge;
        HalfEdge* he = start->next;
        while (he != start) {
            count++;
            he = he->next;
        }

        if (count > 3) {
            toTriangulate.push_back(face);
        }
    }

    for (Face* face : toTriangulate) {
        unmergeFace(face);
    }
}


void Mesh::printDCEL(){
    std::unordered_map<Vertice*, int> vertIdx;
    std::unordered_map<HalfEdge*, int> heIdx;
    std::unordered_map<Face*, int> faceIdx;

    for (unsigned int i = 0; i < vertices.size(); i++)
        vertIdx[vertices[i]] = i;
    for (unsigned int i = 0; i < halfEdges.size(); i++)
        heIdx[halfEdges[i]] = i;
    for (unsigned int i = 0; i < faces.size(); i++)
        faceIdx[faces[i]] = i;

    printf("%lu %lu %lu\n", vertices.size(), halfEdges.size()/2, faces.size());

    for (unsigned int i = 0; i < vertices.size(); i++){
        printf("%d %d %d\n", vertices[i]->x, vertices[i]->y, vertices[i]->z);
    }

    for (unsigned int i = 0; i < faces.size(); i++){
        printf("%d\n", heIdx[faces[i]->halfEdge] + 1);
    }

    for (unsigned int i = 0; i < halfEdges.size(); i++){
        printf("%d %d %d %d %d\n",
            halfEdges[i]->origin->idx + 1,
            halfEdges[i]->twin ? (heIdx[halfEdges[i]->twin] + 1) : 0,
            halfEdges[i]->leftFace ? (faceIdx[halfEdges[i]->leftFace] + 1) : 0,
            halfEdges[i]->next ? (heIdx[halfEdges[i]->next] + 1) : 0,
            halfEdges[i]->prev ? (heIdx[halfEdges[i]->prev] + 1) : 0
        );
    }
}