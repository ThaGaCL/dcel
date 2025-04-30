#include "mesh.h"

void Mesh::load(){
    // A primeira linha tem dois n´umeros inteiros, n e f, separados por um espaço. 
    // O primeiro é o número de vértices, n, e o segundo é o numero de faces, f , da malha.
    // As n linhas seguintes contém as coordenadas de cada vértice, sendo dois números inteiros separados por
    // espaços, xi e yi, onde i é o índice do vértice, variando de 1 a n, na ordem em que aparecem. A partir
    // daí, as próximas f linhas contém os índices dos vértices de cada face, separados por espaçcos, uma face por
    // linha.

    Vertice* v;
    Face* f;
    vector<int> faceVertices;
    int vertexIndex;

    scanf("%u %u", &nVertices, &nFaces);

    for (unsigned int i = 0; i < nVertices; i++) {
        v = new Vertice;
        scanf("%d %d", &v->x, &v->y);
        v->halfEdge = nullptr;
        vertices.push_back(v);
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

Face* Mesh::createNewFace(HalfEdge* he){
    Face* face = new Face;
    face->halfEdge = he; 

    faces.push_back(face);

    return face;
}

void Mesh::printHalfEdge(HalfEdge* he){
    std::cout << "HalfEdge: " << he << std::endl;
    std::cout << "Origin: " << he->origin->x << " " << he->origin->y << std::endl;
    std::cout << "Left Face: " << he->leftFace->halfEdge << std::endl;
    std::cout << "Next: " << he->next->origin->x << " " << he->next->origin->y << std::endl;
    std::cout << "Prev: " << he->prev->origin->x << " " << he->prev->origin->y << std::endl;
    std::cout << "Twin: " << he->twin->origin->x << " " << he->twin->origin->y << std::endl;
}

void printEdge(Edge* edge){
    std::cout << "Origin: " << edge->origin->x << " " << edge->origin->y << std::endl;
    std::cout << "Dest: " << edge->dest->x << " " << edge->dest->y << std::endl;
    std::cout << "Face Index: " << edge->faceIdx << std::endl;
    std::cout << std::endl;
}

void swap(Edge* &a, Edge* &b){
    Edge* temp = a;
    a = b;
    b = temp;
}

/*
    constrói arestas comuns para servir de auxiliar na criação de semi-arestas

void Mesh::constructEdges(){
    Edge* edge;
    int idx;

    for (unsigned int i = 0; i < nFaces; i++){
        for (unsigned int j = 0; j < faceVertices[i].size(); j++){
            edge = new Edge;

            idx = faceVertices[i][j] - 1;           // os indices começam em 0 e as faces em 1
            edge->origin = vertices[idx];

            idx = faceVertices[i][(j+1) % faceVertices[i].size()] - 1;
            edge->dest = vertices[idx];

            edge->faceIdx = i;

            edges.push_back(edge);
        }
    }

    // 'ordena' as arestas para que, por exemplo, a aresta (v1, v2) fique antes da aresta (v2, v1)
    for (unsigned int i = 0; i < edges.size(); i++){
        for (unsigned int j = i+1; j < edges.size(); j++){
            if (edges[i]->origin == edges[j]->dest && edges[i]->dest == edges[j]->origin){
                swap(edges[i+1], edges[j]);
            }
        }
    }

    constructHalfEdges();
}
*/

void Mesh::constructEdges(){
	Edge* edge;
	int idxOrig;
	int idxDest;
	unsigned int size;

	for (unsigned int i = 0; i < nFaces; i++){
		size = faceVertices[i].size();
		for(unsigned int j = 0; j < size; j++){
			idxOrig = faceVertices[i][j] - 1;
			idxDest = faceVertices[i][(j+1) % size] - 1;

			edgesMap[idxOrig] = idxDest;
			printf("idxOrig: %d; idxDest: %d\n", idxOrig, idxDest);
		}			
	}

	constructHalfEdges();
}

HalfEdge* Mesh::createHalfEdgeNode(int idx){
    HalfEdge* he = new HalfEdge;
	he->twin = NULL;
	he->next = NULL;
	he->prev = NULL;
    he->origin = edges[idx]->origin;

    if (!faceDoesExist(edges[idx]->faceIdx)){
        he->leftFace = createNewFace(he);
    } else {
        he->leftFace = faces[edges[idx]->faceIdx];
    }

    return he;
}

/*
void Mesh::constructHalfEdges(){
    HalfEdge* he;
    HalfEdge* twin;
    unsigned int size = edges.size();

    for (unsigned int i = 0; i < size; i+=2){
        he = createHalfEdgeNode(i);
        twin = createHalfEdgeNode(i+1);

        he->twin = twin;
        twin->twin = he;

        halfEdges.push_back(he);
        halfEdges.push_back(twin);
    }

    // Faz a transversal da lista de semi-arestas para encontrar a próxima e a anterior
    for (unsigned int i = 0; i < halfEdges.size(); i++){
        findNext(halfEdges[i]);
        findPrev(halfEdges[i]);
        printHalfEdge(halfEdges[i]);
    }
}
*/

void Mesh::constructHalfEdges(){
	HalfEdge* he;
	HalfEdge* twin;

	for (auto pair : edgesMap){
		//he = createHalfEdgeNode();	
		// e se em vez do hash ser <int, int> ser do tipo <int, edge> ?
		// assim não perderia a propriedade O(1) na busca nem as faces....
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
        if (h->origin == twin->origin && h->leftFace == he->leftFace){
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
    HalfEdge* twin = he->twin;
    HalfEdge* prev = nullptr;

    for (HalfEdge* h : halfEdges){
        if (h->origin == he->origin && h->leftFace == twin->leftFace){
            prev = h;
            break;
        }
    }

    he->prev = prev;
}
