// Enzo Tres mediano 2211731
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_NODES 26
#define INFINITY 9999
#define NUMVERTICES 7

// structs------------------------------------------------------------------------------------------
typedef struct graph Graph;
typedef struct node Node;
typedef struct graphnode GraphNode;
typedef struct dTable DTable;
typedef struct info Info;
typedef struct minHeapNode MinHeapNode;
typedef struct minHeap MinHeap;

struct node {
    char c; //character
    int weight; //peso da arresta do character anterior ate o character no Node com esse valor
    Node* next; //Pointer to get to the next node
};

struct graphnode {
    char c; //character
    Node* listAdj; //Pointer to get to the next node
    int distSource; //Distance to the source node
};

struct graph {
    int n; //numero de vertices
    GraphNode** GraphNodes; //List of vertices represented by GraphNodes (The graph nodes later point to chained list of nodes that it GraphNode connects to)

};

// Prototipos---------------------------------------------------------------------------------------
Graph* createGraph();
void createGraphNode(GraphNode** n, char c, int* weights, char* nodes, int numNodes);
Node* createNode(char c, int peso);
void freeGraph(Graph* graph);
void printGraph(Graph* graph);
void dijkstra(Graph* graph, char source);
int minDisIndex(int dist[], bool visited[]);
void printPath(int parent[], int target, Graph* graph);
void dFS(Graph* graph, int checked[], int vertex);

// Main---------------------------------------------------------------------------------------------

int main() {
    char* src = 'A';                    //Source can be changed to test 
    Graph* graph = createGraph();
    printGraph(graph);                  //Print to check if graph was made correctly
    printf("Path with Dijkstra:\n");
    dijkstra(graph, src);
    int checked[MAX_NODES] = { 0 };     //This is different from the checklist used in the Dijkstra function
    printf("\nPath with DFS:\n");
    dFS(graph, checked, (src - 'A'));   //It is src - A becuase i decided to use a hashing method to know which nodes to reach

    freeGraph(graph);
}

// Functions ---------------------------------------------------------------------------------------

Graph* createGraph() { //Creates the entire graph through use of hard coding the paths but uses functions so that it isnt fully hardcoded
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) {
        exit(2);
    }

    graph->GraphNodes = (GraphNode**)malloc(sizeof(GraphNode*) * MAX_NODES);
    if (graph->GraphNodes == NULL) {
        exit(2);
    }

    for (int i = 0; i < MAX_NODES; i++) {
        graph->GraphNodes[i] = NULL;
    }

    int w[] = {5, 4, 2 };
    char n[] = { 'B', 'C', 'D' };
    createGraphNode(&graph->GraphNodes['A' - 'A'], 'A', w, n, 3);

    int w2[] = { 5, 6, 6, 9 };
    char n2[] = { 'A', 'C', 'E', 'H' };
    createGraphNode(&graph->GraphNodes['B' - 'A'], 'B', w2, n2, 4);

    int w3[] = { 4, 6, 3, 4 };
    char n3[] = { 'A', 'B', 'D', 'E' };
    createGraphNode(&graph->GraphNodes['C' - 'A'], 'C', w3, n3, 4);

    int w4[] = { 2, 3, 5, 9 };
    char n4[] = { 'A', 'C', 'E', 'F' };
    createGraphNode(&graph->GraphNodes['D' - 'A'], 'D', w4, n4, 4);

    int w5[] = { 6, 4, 5, 2, 6 };
    char n5[] = { 'B', 'C', 'D' , 'F', 'H' };
    createGraphNode(&graph->GraphNodes['E' - 'A'], 'E', w5, n5, 5);

    int w6[] = { 9, 2, 3 };
    char n6[] = { 'D', 'E', 'H' };
    createGraphNode(&graph->GraphNodes['F' - 'A'], 'F', w6, n6, 3);

    int w7[] = { 9, 6, 3 };
    char n7[] = { 'B', 'E', 'F' };
    createGraphNode(&graph->GraphNodes['H' - 'A'], 'H', w7, n7, 3);
    
    graph->n = MAX_NODES;
    
    return graph;
}

void createGraphNode(GraphNode** n, char c, int* weights, char* nodes, int numNodes) {  //Creates graph nodes that later point to chainedlist of adj
    *n = (GraphNode*)malloc(sizeof(GraphNode));
    if (*n == NULL) {
        exit(2);
    }
    (*n)->c = c;
    (*n)->distSource = INFINITY;
    (*n)->listAdj = NULL;

    Node* currentNode = (*n)->listAdj;

    for (int i = 0; i < numNodes; i++) {
        (*n)->listAdj = createNode(nodes[i], weights[i]);
        (*n)->listAdj->next = currentNode;
        currentNode = (*n)->listAdj;
    }
}

Node* createNode(char c, int weight) {  //creates Node (self explanatory)
    Node* currNode = (Node*)malloc(sizeof(Node));
    if (currNode == NULL) {
        exit(2);
    }
    currNode->c = c;
    currNode->weight = weight;
    currNode->next = NULL;

    return currNode;
}

void freeGraph(Graph* graph) { //Frees graph (self explanatory)
    
    Node* currNode = NULL;
    Node* prevNode = NULL;
    GraphNode* currGraphNode = NULL;

    for (int i = 0; i < MAX_NODES; i++) {
        currGraphNode = graph->GraphNodes[i];
        if (graph->GraphNodes[i] == NULL) {
            free(currGraphNode);
            continue;
        }
        prevNode = currGraphNode->listAdj;
        while (prevNode != NULL) {
            currNode = prevNode->next;
            free(prevNode);
            prevNode = currNode;
        }
        free(currGraphNode);
    }
    free(graph);
}

void printGraph(Graph* graph) { //Prints graph (self explanitory)
    printf("Here is how the graph turned out:\n");
    Node* currNode = NULL;
    Node* prevNode = NULL;
    GraphNode* currGraphNode = NULL;

    for (int i = 0; i < MAX_NODES; i++) {
        if (graph->GraphNodes[i] == NULL) {
            continue;
        }
        currGraphNode = graph->GraphNodes[i];
        prevNode = currGraphNode->listAdj;
        printf("|GraphNode: %c |", currGraphNode->c);
        while (prevNode != NULL) {
            currNode = prevNode->next;
            printf("-> |Node: %c %d|", prevNode->c, prevNode->weight);
            prevNode = currNode;
        }
        printf("\n");
    }
}

void dijkstra(Graph* graph, char source) {
    int sourceIndex = source - 'A';

    int weight[MAX_NODES]; //Three created so that each array could keep track of nodes in a Hashing method
    bool visited[MAX_NODES]; 
    int parent[MAX_NODES]; 

    int i;
    for (i = 0; i < MAX_NODES; i++) {
        weight[i] = INFINITY;
        visited[i] = false;
        parent[i] = -1;
    }

    weight[sourceIndex] = 0;

    int count;
    for (count = 0; count < NUMVERTICES; count++) {
        
        int minIndex = minDisIndex(weight, visited);
        visited[minIndex] = true;

        Node* curr = graph->GraphNodes[minIndex]->listAdj;

        while (curr != NULL) {
            int currIndex = curr->c - 'A';
            if (!visited[currIndex] && weight[minIndex] + curr->weight < weight[currIndex]) {
                weight[currIndex] = weight[minIndex] + curr->weight;
                parent[currIndex] = minIndex;  //Keep track of parent for later print
            }
            curr = curr->next;  //Done after so that parent could have gotten data
        }
    }

    // Print the shortest paths
    printf("Vertex\t\tDistance\tPath");
    for (i = 0; i < graph->n; i++) {
        if (graph->GraphNodes[i] == NULL) {
            continue;
        }
        printf("\n%c -> %c\t\t%d\t\t%c ", source, i + 'A', weight[i], source);
        printPath(parent, i, graph);
    }
}

int minDisIndex(int dist[], bool visited[]) {   //Tracks the Index tha has the smallest distance so that whoever it returns to can go to that node
    int min = INFINITY, minIndex;
    int i;
    for (i = 0; i < MAX_NODES; i++) {
        if (!visited[i] && dist[i] < min) {
            min = dist[i];
            minIndex = i;
        }
    }
    return minIndex;
}

void printPath(int parent[], int curr, Graph* graph) { //Function that showed the path from source to last node given in path
    if (parent[curr] == -1) {
        return;
    }
    printPath(parent, parent[curr], graph);
    printf("%c ", curr + 'A');
}

void dFS(Graph* graph, int checked[], int vertex) {
    checked[vertex] = 1;
    Node* currNode = graph->GraphNodes[vertex];
    printf("%c ", currNode->c);

    while (currNode != NULL) {
        if (checked[currNode->c - 'A'] != 1) {  //1 significa visitado
            dFS(graph, checked, (currNode->c - 'A'));
        }
        currNode = currNode->next;
    }

}