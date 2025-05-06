#ifndef mesh_h
#define mesh_h

#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

typedef struct HalfEdge HalfEdge;
typedef struct Face Face;

typedef struct {
    int x;
    int y;
    int idx;                        
    HalfEdge* halfEdge;                     // Pointeiro para uma semi-aresta que tem v como origem (halfEdge(v))
} Vertice;

typedef struct {
    // Vertice* origin;                        // Ponteiro para o vértice de origem da aresta (v)
    int dest;                          // Ponteiro para o vértice de destino da aresta (u)
    int faceIdx;
} Edge;

typedef struct HalfEdge {
    int idx;
    Vertice* origin;                        // Ponteiro para o vértice de origem da semi-aresta (v)
    HalfEdge* twin;                         // Ponteiro para a semi-aresta oposta
    HalfEdge* next;                         // Ponteiro para a próxima semi-aresta na mesmo face
    HalfEdge* prev;                         // Ponteiro para a semi-aresta anterior na mesma face
    Face* leftFace;                         // Ponteiro para a face à esquerda da semi-aresta
} HalfEdge;

typedef struct Face {
    int idx;                          
    HalfEdge* halfEdge;                     // Pointeiro para uma semi-aresta que tem f como face esquerda
} Face;

using namespace std;

using VERTICES = vector<Vertice*>;          // Vetor de vértices
using FACES = vector<Face*>;                // Vetor de faces
using HALF_EDGES = vector<HalfEdge*>;       // Vetor de semi-arestas
using EDGES = vector<Edge*>;                // Vetor de arestas (auxiliar para a construção de semi-arestas)

using FACE_VERTICES = vector<vector<int>>; // Vetor de vetores de inteiros, onde cada vetor interno representa os índices dos vértices de uma face


class Mesh {
    public: 
        Mesh() {};
        ~Mesh() {
            for (auto v : vertices) delete v;
            for (auto f : faces) delete f;
            for (auto he : halfEdges) delete he;
        };
        void load();
        void constructHalfEdges();
        void constructEdges();
        bool isTopologyValid();

    private:
        unsigned int nVertices;
        unsigned int nFaces;
        unsigned int nEdges;

        bool faceDoesExist(int idx);
        bool checkIfVertexExists(int x, int y);
        bool isOpen();
        bool isSubdivPlanar();
        bool isOverlapped();
        bool autoIntersect(Face* f);

        Face* createNewFace(int idx);
        HalfEdge* createHalfEdgeNode(Vertice* origin, int faceIdx, int idx);
        Vertice* createNewVertex(int x, int y, int idx);

        void findNext(HalfEdge* he);
        void findPrev(HalfEdge* he);
        void printHalfEdge(HalfEdge* he);

        VERTICES vertices;                
        FACES faces;
        HALF_EDGES halfEdges;
        FACE_VERTICES faceVertices; 
        EDGES edges;                   
		unordered_map<int, vector<Edge*>> edgesMap;
};

#endif
