#!/usr/bin/env python3

# Feito por Matheus de Moraes Cavazotti (github.com/mcavazotti)

# USO:
# Para desenhar um arquivo de saída do Trabalho 2, faça:
# $ python3 draw.py < arquivo

import matplotlib.pyplot as plt
from random import uniform, seed


# Definição de Ponto
class Point:
    def __init__(self, idx: int, x: int, y: int, edge: int) -> None:
        self.idx = idx
        self.x = x
        self.y = y
        self.edge = edge

    def __str__(self) -> str:
        return "[{}]({},{}){}".format(self.idx, self.x, self.y, self.edge)

    def __repr__(self) -> str:
        return self.__str__()

# Definição de Semi-Aresta
class HalfEdge:
    def __init__(self, idx: int, point: int, twin: int, face: int, next: int, prev: int) -> None:
        self.idx = idx
        self.point = point
        self.twin = twin
        self.face = face
        self.next = next
        self.prev = prev

    def __str__(self) -> str:
        return "[{}][o {},s {},f {},n {},p {}]".format(self.idx, self.point, self.twin, self.face, self.next, self.prev)

    def __repr__(self) -> str:
        return self.__str__()

# Definição de Face
class Face:
    def __init__(self, idx: int, edge: int) -> None:
        self.idx = idx
        self.edge = edge

    def __str__(self) -> str:
        return "[{}]{}".format(self.idx, self.edge)

    def __repr__(self) -> str:
        return self.__str__()


if __name__ == "__main__":
    seed()
    vList = []
    fList = []
    eList = []

    # Ler entrada
    v, a, f = map(int, input().split())
    # print(v, a, f)
    
    for i in range(1, v+1):
        x, y, e = map(int, input().split())
        vList.append(Point(i, x, y, e))
        # print(i, x, y, e)

    for i in range(1, f+1):
        e = int(input())
        fList.append(Face(i, e))
        # print(i, e)

    for i in range(1, 2*a+1):
        v, t, f, n, p = map(int, input().split())
        eList.append(HalfEdge(i, v, t, f, n, p))
        # print(i, v, t, f, n, p)

    # print(vList)
    # print(fList)
    # print(eList)

    # Inicializar figura
    fig, ax = plt.subplots()

    xCoord = []
    yCoord = []
    for f in fList:
        # Preencher faces
        edge = eList[f.edge - 1]
        tmpEdge = eList[edge.next -1]

        xCoord.clear()
        yCoord.clear()

        xCoord.append(vList[edge.point -1].x)
        yCoord.append(vList[edge.point -1].y)

        while tmpEdge.idx != edge.idx:
            xCoord.append(vList[tmpEdge.point -1].x)
            yCoord.append(vList[tmpEdge.point -1].y)

            tmpEdge = eList[tmpEdge.next - 1]
        
        ax.fill(xCoord, yCoord, color=(uniform(0.5, 1), uniform(0.5, 1), uniform(0.5, 1), 0.8))
        
        # Plotar pontos
        # ax.scatter(f.x, f.y, color="#f00", s=20, zorder=10)


    # Plotar arestas
    for e in eList:
        x1 = vList[e.point - 1].x
        y1 = vList[e.point - 1].y
        x2 = vList[eList[e.next - 1].point - 1].x
        y2 = vList[eList[e.next - 1].point - 1].y
        ax.plot([x1, x2], [y1, y2], color="#000", lw=1)


    # Ajustar a escala para não ter distorção
    plt.axis('scaled')

    # Mostrar figura
    plt.show()
