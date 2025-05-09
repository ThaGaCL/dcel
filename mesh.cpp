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
            edge = new Edge;
            edge->dest = idxDest;
            edge->faceIdx = i;
            printf("edge %d, (%d, %d), (%d, %d)\n", i, vertices[idxOrig]->x, vertices[idxOrig]->y, vertices[idxDest]->x, vertices[idxDest]->y);

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
            printf("hf: (%d, %d), (%d, %d)\n", he->origin->x, he->origin->y, vertices[dest]->x, vertices[dest]->y);
            for (Edge* candidate : edgesToProcess[dest]) {
                printf("candidate: (%d, %d), (%d, %d)\n", vertices[dest]->x, vertices[dest]->y, vertices[candidate->dest]->x, vertices[candidate->dest]->y);
                if (candidate->dest == origin) {
                    twin = createHalfEdgeNode(vertices[dest], candidate->faceIdx, idx++);
                    break;
                }
            }
        }
        printf("HalfEdges size: %ld, edgesSize: %ld\n", halfEdges.size(), edgesMap.size());

        if (!twin){
            // Se não encontrar a semi-aresta simétrica, significa que a malha não é válida
            // Ela é aberta
            printf("aberta");
            return;
        }

        he->twin = twin;
        twin->twin = he;

        halfEdges.push_back(he);
        halfEdges.push_back(twin);

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

bool Mesh::isOpen(){
    // identificar se é aberta : alguma aresta é fronteira de somente uma face
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

bool Mesh::isSubdivPlanar(){
    // identificar se não é subdivisão planar : alguma aresta é fronteira de mais de duas faces
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

bool Mesh::isTopologyValid(){
    // identificar se é aberta : alguma aresta é fronteira de somente uma face
    // identificar se não é subdivisão planar : alguma aresta é fronteira de mais de duas faces
    // identificar se é superposta : alguma face tem auto-interseção ou intersecta outras faces
    // caso esteja tudo certo, retornar true

    if (isOpen()) {
        printf("aberta 2\n");
        return false;
    };
    if (isSubdivPlanar()) {
        printf("não subdivisão planar\n");
        return false;
    };
    if (isOverlapped()) return false;

    return true;
}
