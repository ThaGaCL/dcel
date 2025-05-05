#include "mesh.h"

bool Mesh::checkIfVertexExists(int x, int y){
    for (Vertice* v : vertices){
        if (v->x == x && v->y == y){
            return true;
        }
    }
    return false;
}


Vertice* Mesh::createNewVertex(int x, int y, int idx){
    Vertice* v = nullptr;

    if (checkIfVertexExists(x, y))
        return nullptr;

    v = new Vertice;
    v->x = x;
    v->y = y;
    v->halfEdge = nullptr;
    v->idx = idx;
    vertices.push_back(v);

    return v;
}

void Mesh::load(){
    // A primeira linha tem dois n´umeros inteiros, n e f, separados por um espaço. 
    // O primeiro é o número de vértices, n, e o segundo é o numero de faces, f , da malha.
    // As n linhas seguintes contém as coordenadas de cada vértice, sendo dois números inteiros separados por
    // espaços, xi e yi, onde i é o índice do vértice, variando de 1 a n, na ordem em que aparecem. A partir
    // daí, as próximas f linhas contém os índices dos vértices de cada face, separados por espaçcos, uma face por
    // linha.

    Face* f;
    vector<int> faceVertices;
    int vertexIndex;

    scanf("%u %u", &nVertices, &nFaces);

    for (unsigned int i = 0; i < nVertices; i++) {
        int x, y;
        scanf("%d %d", &x, &y);

        createNewVertex(x, y, i);
    }

    for (unsigned int i = 0; i < nFaces; i++) {
        f = new Face;
        f->halfEdge = nullptr;
        faces.push_back(f);

        faceVertices.clear();
        while (scanf("%d", &vertexIndex) == 1) {
            faceVertices.push_back(vertexIndex);
            if (getchar() == '\n') break; 
        }

        this->faceVertices.push_back(faceVertices);
    }

    constructEdges();

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
    std::cout << "HalfEdge: " << he << std::endl;
    std::cout << "Origin: " << he->origin->x << " " << he->origin->y << std::endl;
    std::cout << "Left Face: " << he->leftFace << std::endl;
    std::cout << "Next: " << he->next->origin->x << " " << he->next->origin->y << std::endl;
    if (he->prev) std::cout << "Prev: " << he->prev->origin->x << " " << he->prev->origin->y << std::endl;
    else std::cout << "Prev: NULL" << std::endl;
    std::cout << "Twin: " << he->twin->origin->x << " " << he->twin->origin->y << std::endl;
}

// void printEdge(Edge* edge){
//     std::cout << "Origin: " << edge->origin->x << " " << edge->origin->y << std::endl;
//     std::cout << "Dest: " << edge->dest->x << " " << edge->dest->y << std::endl;
//     std::cout << "Face Index: " << edge->faceIdx << std::endl;
//     std::cout << std::endl;
// }
/*
    constrói arestas comuns para servir de auxiliar na criação de semi-arestas
*/

void Mesh::constructEdges(){
	Edge* edge;
	int idxOrig;
	int idxDest;
	unsigned int size;

	for (unsigned int i = 0; i < nFaces; i++){
		size = faceVertices[i].size();
        createNewFace(i);

		for(unsigned int j = 0; j < size; j++){
			idxOrig = faceVertices[i][j] - 1;
			idxDest = faceVertices[i][(j+1) % size] - 1;
            edge = new Edge;
            edge->dest = idxDest;
            edge->faceIdx = i;

			edgesMap[idxOrig].push_back(edge);
		}			
	}

	constructHalfEdges();
}

HalfEdge* Mesh::createHalfEdgeNode(Vertice* origin, int faceIdx, int idx){
    HalfEdge* he = new HalfEdge;
	he->twin = NULL;
	he->next = NULL;
	he->prev = NULL;
    he->origin = origin;
    he->idx = idx;

    he->leftFace = faces[faceIdx];
    faces[faceIdx]->halfEdge = he;

    return he;
}

// void Mesh::constructHalfEdges(){
// 	HalfEdge* he;
// 	HalfEdge* twin;
//     int idx = 0;

// 	std::unordered_map<int, Edge*> edgesToProcess = edgesMap;
//     for (auto it = edgesToProcess.begin(); it != edgesToProcess.end();)
//     {
//         int origin = it->first;
//         int dest = it->second->dest;

//         if (origin > dest)
//         {
//             ++it; 
//             continue;
//         }

//         he = createHalfEdgeNode(vertices[origin], it->second->faceIdx, idx++);
//         twin = createHalfEdgeNode(vertices[dest], edgesMap[dest]->faceIdx, idx++);

//         he->twin = twin;
//         twin->twin = he;

//         halfEdges.push_back(he);
//         halfEdges.push_back(twin);

//         it = edgesToProcess.erase(it);
//     }

//     for (HalfEdge *he : halfEdges){
//         findNext(he);
//     }

//     for (HalfEdge *he : halfEdges){
//         findPrev(he);
//         printHalfEdge(he);
//     }
// }

void Mesh::constructHalfEdges(){
    int idx = 0;

    std::unordered_map<int, vector<Edge*>> edgesToProcess = edgesMap;
    for (auto edgeVector = edgesToProcess.begin(); edgeVector != edgesToProcess.end(); ++edgeVector) {
    int origin = edgeVector->first;
    auto& edgeList = edgeVector->second;

    for (Edge* edge : edgeList) {
        int dest = edge->dest;

        if (origin > dest) continue;

        HalfEdge* he = createHalfEdgeNode(vertices[origin], edge->faceIdx, idx++);
        HalfEdge* twin = nullptr;

        // Como um vértice pode ter mais de uma semi-aresta, é necessário verificar todos os candidatos 
        if (edgesToProcess.count(dest)) {
            for (Edge* candidate : edgesToProcess[dest]) {
                if (candidate->dest == origin) {
                    twin = createHalfEdgeNode(vertices[dest], candidate->faceIdx, idx++);
                    break;
                }
            }
        }

        he->twin = twin;
        if (twin) twin->twin = he;

        halfEdges.push_back(he);
        if (twin) halfEdges.push_back(twin);

    }
    }
    for (HalfEdge *he : halfEdges){
        findNext(he);
    }

    for (HalfEdge *he : halfEdges){
        findPrev(he);
        printHalfEdge(he);
    }
}


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
