Eduarda Saibert
Thales Gabriel Carvalho de Lima 
**Relatório de Geometria Computacional: DCEL**

# 1 Problema

O trabalho consiste em fazer uma implementação da leitura de uma lista de regiões que compõe
uma subdivisão planar (malha), testar se é topologicamente bem definida e gerar uma DCEL
(Doubly Connected Edge List) como saída.

# 2 Entrada

Para entrada de arquivos, considerou-se a seguinte definição: A malha será descrita por um
texto. A primeira linha tem dois números inteiros, _n_ e _f_ , separados por um espaço. O primeiro é
o número de vértices, _n_ , e o segundo é o número de faces, _f_ , da malha.
As _n_ linhas seguintes contêm as coordenadas de cada vértice, sendo dois números inteiros
separados por espaços, _xi_ e _yi_ , onde _i_ é o índice do vértice, variando de 1 a _n_ , na ordem em que
aparecem.
A partir daí, as próximas _f_ linhas contêm os índices dos vértices de cada face, separados por
espaços, uma face por linha.
Além disso o programa considera que a face externa sempre está no sentido contrário à das
faces internas.

# 3 Programa

```
Para o programa, utiliza-se:
```
```
make {all} -- compilação
./malha < in.txt > out.txt -- iniciar programa
make clean -- apagar objetos
```
# 4 A Classe Mesh

```
Para a construção da classe Mesh, são necessárias as seguintes definições:
```
- **Vértice** : a estrutura de vértice contém as coordenadas (x, y) e um ponteiro para uma semi-
    aresta que tem o vértice como origem.
- **Face** : cada face contém um ponteiro para uma semi-aresta que tem a respectiva face como
    face esquerda.
- **Semi-aresta** : nas semi-arestas, são armazenados um vértice de origem, um ponteiro para a
    semi-aresta oposta (twin), um ponteiro para a próxima semi-aresta na mesma face (next), um
    ponteiro para a semi-aresta anterior na mesma face (prev) e um ponteiro para a face esquerda
    da respectiva semi-aresta.


```
Na leitura do arquivo de entrada são gerados todos os vértices e todas as faces. Os ponteiros
encontrados dentro dessas estruturas são preenchidos ao longo das funções seguintes.
```
# 5 Construindo Arestas

Além das definições acima também é necessário a definição das arestas para a construção das
semi-arestas. Cada aresta irá guardar um ponteiro para o índice de destino da aresta e um índice
da face que referenciou tal aresta. Não é necessário guardar o vértice de origem das arestas pois
elas serão armazenadas em um HashMap com a seguinte proposta:

```
unordered_map<int, vector<Edge*>> edgesMap;
```
Assim, elas serão indexadas por um índice ao vértice de origem. Como mais de uma aresta pode
ter um vértice de origem, foi proposto que cada indexação resulte em um vetor de arestas. Em uma
entrada de malha padrão esse vetor nunca chegará a n (número de vértices), ou seja, o HashMap
ainda preserva seu custo de busca.

# 6 Construindo Semi-arestas

A partir de um loop no HashMap de arestas, é possível definir as semi-arestas. A cada itera-
ção, serão criadas duas semi-arestas, sendo uma oposta da outra. Para definir a aresta oposta, é
necessário iterar sobre o array de possíveis candidatos. A aresta oposta efetiva deve ter o vértice
de destino igual ao vértice de origem da aresta irmã.
No caso de não haverem candidatos ou depois da checagem de candidatos não existir uma aresta
oposta, a função de construção de semi-arestas deve encerrar. Após isso, as funções de checagem
de topologia irão constatar a malha como aberta.
Caso contrário, serão chamadas duas outras funções:

- **Achar Pŕoxima** : Sendo _u_ a semi-aresta de origem e _v_ sua oposta temos _prox_ ( _v_ ) = _w_ se
    _f ace_ ( _u_ ) = _f ace_ ( _w_ )e _origem_ ( _w_ ) = _origem_ ( _v_ )
- **Achar Anterior** : No mesmo princípio, _ant_ ( _v_ ) = _w_ se _f ace_ ( _u_ ) = _f ace_ ( _w_ )e _prox_ ( _w_ ) = _u_

# 7 Checagem de Topologia

Na checagem de topologia, é necessário tratar três casos degenerados: os casos em que a estru-
tura gerada é aberta, não é subdivisão planar ou contém intersecção.

## 7.1 Checagem de Abertura

Para saber se uma malha é aberta, basta iterar sobre as semi-arestas procurando uma aresta
que é fronteira de somente uma face. Para isso, basta que sua oposta não exista ou não esteja
contida em uma face.


## 7.2 Checagem da Subdivisão Planar

Para saber se uma malha é subdivisão planar é necessário que ela tenha fronteira com somente
duas faces. Assim, é preciso checar se alguma aresta faz divisa com mais de duas faces. Para isso,
itera-se sobre a lista circular de cada face, inserindo cada face única em um array. Caso o número
de faces coletadas seja mais do que dois, a malha não é uma subdivisão planar.

## 7.3 Checagem de Intersecções (Sweep Line)

Para a checagem de intersecções (sejam elas ocorrendo em uma única face ou entre duas ou
mais faces), optou-se por utilizar o algoritmo de varredura. Nesse algoritmo, uma reta infinita _y_
caminha entre a projeção da malha procurando intersecções.
A fim de tornar isso possível definimos a estrutura _Evento_. Um evento contém uma semi-aresta
e seu tipo. Cada evento pode ser um vértice esquerdo (left endpoint) ou um vértice direito (right
endpoint). Um vértice esquerdo marca o começo de uma aresta e um vértice direito o fim dela.
A organização dos eventos é feita em duas estruturas de dados.
Todos os eventos são colocados em uma fila preferencial (heap) a fim de serem devidamente
ordenados. Essa ordenação também precisa ser definida. Foi-se preferido que a ordenação acon-
tecesse de baixo para cima, da esquerda para a direita. Além disso, eventos de vértice esquerdo
ganham preferência sobre eventos de vértice direito.

```
priority_queue<Event*, vector<Event*>, EventComparator> eventQueue;
```
As semi-arestas dos eventos ativos são armazenados em uma árvore binária balanceada. En-
quanto a fila de eventos não acaba, para cada vértice esquerdo é chamada a função que adiciona
o evento aos eventos ativos e para cada vértice direito é chamada a função que remove o evento
dos eventos ativos. Essas duas funções também checam intersecções na árvore. Caso uma intersec-
ção ou auto-intersecção seja encontrada, retorna. Assim, o pior caso do algoritmo de varredura é
quando não há intersecção (será iterado sobre todos os eventos).

```
set<HalfEdge*, SetComparator> status;
```
# 8 Saída

Na primeira linha tem três números, _n_ , _m_ e _f_ , que são os números de vértices, arestas e faces,
respectivamente. Os vértices, as faces e as semi-arestas são indexados iniciando em 1, de acordo
com a posição relativa no texto.
As _n_ linhas seguintes são as descrições dos vértices, um por linha, com três números inteiros
separados por espaços:

- _xi_ e _yi_ (coordenadas do vértice)
- índice de uma semi-aresta que tem este vértice como origem.

```
Em seguida, temos f linhas com as descrições das faces, cada uma contendo:
```
- índice de uma semi-aresta que tem esta face como face esquerda.


As próximas 2 _m_ linhas contêm os dados das semi-arestas da DCEL. Cada linha representa uma
semi-aresta e possui cinco números inteiros separados por espaços, na seguinte ordem:

1. **origem** (índice do vértice origem)
2. **simétrica** (índice da semi-aresta simétrica)
3. **esquerda** (índice da face esquerda)
4. **próxima** (índice da próxima semi-aresta, em torno da face esquerda)
5. **anterior** (índice da semi-aresta anterior, em torno da face esquerda).


