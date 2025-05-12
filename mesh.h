#ifndef mesh_h
#define mesh_h

// #include "io.h" // Definições de Structs e tipos
#include "sweepLine.h"

class Mesh {
    public: 
        Mesh() {};
        ~Mesh() {
            for (auto v : vertices) delete v;
            for (auto f : faces) delete f;
            for (auto he : halfEdges) delete he;
        };
        void load();
        void printDCEL();
        void constructHalfEdges();
        void constructEdges();
        bool isTopologyValid();

    private:
        unsigned int nVertices = 0;
        unsigned int nFaces = 0;
        unsigned int nHalfEdges = 0;

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
