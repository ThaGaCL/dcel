#ifndef mesh_h
#define mesh_h

#include <vector>;
#include <iostream>;

typedef struct {
    int x;
    int y;
    HalfEdge* halfEdge;                 // Pointeiro para uma semi-aresta que tem v como origem (halfEdge(v))
} Vertice;

typedef struct {
    Vertice* origin;                    // Ponteiro para o vértice de origem da semi-aresta (v)
    HalfEdge* twin;                     // Ponteiro para a semi-aresta oposta
    HalfEdge* next;                     // Ponteiro para a próxima semi-aresta na mesmo face
    HalfEdge* prev;                     // Ponteiro para a semi-aresta anterior na mesma face
    Face* leftFace;                     // Ponteiro para a face à esquerda da semi-aresta
} HalfEdge;

typedef struct {
    HalfEdge* halfEdge;                 // Pointeiro para uma semi-aresta que tem f como face esquerda
} Face;

using namespace std;

using VERTICES = vector<Vertice*>;      // Vetor de vértices
using FACES = vector<Face*>;            // Vetor de faces
using HALF_EDGES = vector<HalfEdge*>;   // Vetor de semi-arestas

class Mesh {
    public: 
        Mesh() {};
        ~Mesh() {
            for (auto v : vertices) delete v;
            for (auto f : faces) delete f;
            for (auto he : halfEdges) delete he;
        };
        void load();

    private:
        unsigned int nVertices;
        unsigned int nFaces;
        unsigned int nEdges;

        VERTICES vertices;                
        FACES faces;
        HALF_EDGES halfEdges;
};

#endif