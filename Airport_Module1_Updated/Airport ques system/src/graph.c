
#include <stdio.h>
#include "graph.h"

#define INF 999999
#define MAX_EDGES 10
#define BF_MAX_EDGES (V * (V - 1))

/* ================= AIRPORT LOCATIONS ================= */

static const char *locations[V] = {
    "Check-in",
    "Security",
    "Immigration",
    "Waiting Area",
    "Boarding Gate"
};

/* ================= AIRPORT GRAPH ================= */

static const int graph[V][V] = {
    {0,  4,  2,  0,  0},
    {4,  0,  1,  5,  0},
    {2,  1,  0,  8, 10},
    {0,  5,  8,  0,  2},
    {0,  0, 10,  2,  0}
};

/* ================= COMMON PATH HELPER ================= */

static void printPath(int parent[], int vertex) {

    if (parent[vertex] == -1) {
        printf("%s", locations[vertex]);
        return;
    }

    printPath(parent, parent[vertex]);
    printf(" -> %s", locations[vertex]);
}

/* =====================================================
                     DIJKSTRA ALGORITHM
   ===================================================== */

static int minDistance(int dist[], int visited[]) {

    int min = INF;
    int minIndex = -1;

    for (int i = 0; i < V; i++) {

        if (!visited[i] && dist[i] < min) {
            min = dist[i];
            minIndex = i;
        }
    }

    return minIndex;
}

void runDijkstra(void) {

    int dist[V];
    int visited[V] = {0};
    int parent[V];

    int source, destination;

    printf("\n===== DIJKSTRA SHORTEST PATH =====\n");

    for (int i = 0; i < V; i++) {
        printf("%d. %s\n", i, locations[i]);
    }

    printf("Enter source location (0-4): ");

    if (scanf("%d", &source) != 1 ||
        source < 0 || source >= V) {

        printf("Invalid source location!\n");
        return;
    }

    printf("Enter destination location (0-4): ");

    if (scanf("%d", &destination) != 1 ||
        destination < 0 || destination >= V) {

        printf("Invalid destination location!\n");
        return;
    }

    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[source] = 0;

    for (int count = 0; count < V - 1; count++) {

        int u = minDistance(dist, visited);

        if (u == -1) {
            break;
        }

        visited[u] = 1;

        for (int v = 0; v < V; v++) {

            if (!visited[v] &&
                graph[u][v] > 0 &&
                dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {

                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    printf("\n===== SHORTEST ROUTE RESULT =====\n");

    if (dist[destination] == INF) {
        printf("No route available!\n");
        return;
    }

    printf("Source: %s\n", locations[source]);
    printf("Destination: %s\n", locations[destination]);

    printf("Shortest Path: ");
    printPath(parent, destination);

    printf("\nTotal Travel Time: %d units\n",
           dist[destination]);
}

/* =====================================================
                       PRIM ALGORITHM
   ===================================================== */

static int minKey(int key[], int inMST[]) {

    int min = INF;
    int minIndex = -1;

    for (int i = 0; i < V; i++) {

        if (!inMST[i] && key[i] < min) {
            min = key[i];
            minIndex = i;
        }
    }

    return minIndex;
}

void runPrim(void) {

    int parent[V];
    int key[V];
    int inMST[V] = {0};

    int start = 0;

    printf("\n===== PRIM'S MINIMUM SPANNING TREE =====\n");
    printf("Starting Location: %s\n", locations[start]);

    for (int i = 0; i < V; i++) {
        key[i] = INF;
        parent[i] = -1;
    }

    key[start] = 0;

    for (int count = 0; count < V; count++) {

        int u = minKey(key, inMST);

        if (u == -1) {
            break;
        }

        inMST[u] = 1;

        for (int v = 0; v < V; v++) {

            if (graph[u][v] > 0 &&
                !inMST[v] &&
                graph[u][v] < key[v]) {

                parent[v] = u;
                key[v] = graph[u][v];
            }
        }
    }

    int totalCost = 0;
    int connected = 1;

    for (int i = 0; i < V; i++) {

        if (!inMST[i]) {
            connected = 0;
        }
    }

    if (!connected) {
        printf("Network is disconnected!\n");
        return;
    }

    printf("\n===== MINIMUM CONNECTION NETWORK =====\n");

    for (int i = 0; i < V; i++) {

        if (parent[i] != -1) {

            printf("%s -- %s | Cost: %d\n",
                   locations[parent[i]],
                   locations[i],
                   graph[parent[i]][i]);

            totalCost += graph[parent[i]][i];
        }
    }

    printf("\nTotal Minimum Connection Cost: %d units\n",
           totalCost);
}

/* =====================================================
                      KRUSKAL ALGORITHM
   ===================================================== */

typedef struct {
    int source;
    int destination;
    int weight;
} Edge;

static void sortEdges(Edge edges[], int n) {

    for (int i = 0; i < n - 1; i++) {

        for (int j = 0; j < n - i - 1; j++) {

            if (edges[j].weight > edges[j + 1].weight) {

                Edge temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }
}

static int findParent(int parent[], int vertex) {

    if (parent[vertex] != vertex) {

        parent[vertex] =
            findParent(parent, parent[vertex]);
    }

    return parent[vertex];
}

static void unionSets(int parent[], int rank[],
                      int x, int y) {

    int rootX = findParent(parent, x);
    int rootY = findParent(parent, y);

    if (rank[rootX] < rank[rootY]) {

        parent[rootX] = rootY;

    } else if (rank[rootX] > rank[rootY]) {

        parent[rootY] = rootX;

    } else {

        parent[rootY] = rootX;
        rank[rootX]++;
    }
}

void runKruskal(void) {

    Edge edges[MAX_EDGES];

    int edgeCount = 0;
    int parent[V];
    int rank[V] = {0};

    int totalCost = 0;
    int selectedEdges = 0;

    printf("\n===== KRUSKAL'S MINIMUM SPANNING TREE =====\n");

    /* Create edge list from upper triangle */
    for (int i = 0; i < V; i++) {

        for (int j = i + 1; j < V; j++) {

            if (graph[i][j] > 0) {

                edges[edgeCount].source = i;
                edges[edgeCount].destination = j;
                edges[edgeCount].weight = graph[i][j];

                edgeCount++;
            }
        }
    }

    sortEdges(edges, edgeCount);

    for (int i = 0; i < V; i++) {
        parent[i] = i;
    }

    printf("\nSorted Edges:\n");

    for (int i = 0; i < edgeCount; i++) {

        printf("%s -- %s | Weight: %d\n",
               locations[edges[i].source],
               locations[edges[i].destination],
               edges[i].weight);
    }

    printf("\n===== SELECTED MST CONNECTIONS =====\n");

    for (int i = 0;
         i < edgeCount && selectedEdges < V - 1;
         i++) {

        int u = edges[i].source;
        int v = edges[i].destination;

        int rootU = findParent(parent, u);
        int rootV = findParent(parent, v);

        if (rootU != rootV) {

            printf("%s -- %s | Cost: %d\n",
                   locations[u],
                   locations[v],
                   edges[i].weight);

            unionSets(parent, rank, rootU, rootV);

            totalCost += edges[i].weight;
            selectedEdges++;
        }
    }

    if (selectedEdges != V - 1) {
        printf("\nNetwork is disconnected!\n");
        return;
    }

    printf("\nTotal Minimum Connection Cost: %d units\n",
           totalCost);
}

/* =====================================================
                    BELLMAN-FORD ALGORITHM
   ===================================================== */

typedef struct {
    int source;
    int destination;
    int weight;
} BFEdge;

void runBellmanFord(void) {

    BFEdge edges[BF_MAX_EDGES];

    int edgeCount = 0;
    int dist[V];
    int parent[V];

    int source, destination;

    printf("\n===== BELLMAN-FORD SHORTEST PATH =====\n");

    for (int i = 0; i < V; i++) {
        printf("%d. %s\n", i, locations[i]);
    }

    printf("Enter source location (0-4): ");

    if (scanf("%d", &source) != 1 ||
        source < 0 || source >= V) {

        printf("Invalid source location!\n");
        return;
    }

    printf("Enter destination location (0-4): ");

    if (scanf("%d", &destination) != 1 ||
        destination < 0 || destination >= V) {

        printf("Invalid destination location!\n");
        return;
    }

    /* Convert adjacency matrix to directed edge list */
    for (int i = 0; i < V; i++) {

        for (int j = 0; j < V; j++) {

            if (graph[i][j] > 0) {

                edges[edgeCount].source = i;
                edges[edgeCount].destination = j;
                edges[edgeCount].weight = graph[i][j];

                edgeCount++;
            }
        }
    }

    /* Initialize distances */
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[source] = 0;

    /* Relax edges V-1 times */
    for (int i = 0; i < V - 1; i++) {

        int updated = 0;

        for (int j = 0; j < edgeCount; j++) {

            int u = edges[j].source;
            int v = edges[j].destination;
            int weight = edges[j].weight;

            if (dist[u] != INF &&
                dist[u] + weight < dist[v]) {

                dist[v] = dist[u] + weight;
                parent[v] = u;
                updated = 1;
            }
        }

        if (!updated) {
            break;
        }
    }

    /* Detect negative-weight cycle */
    for (int i = 0; i < edgeCount; i++) {

        int u = edges[i].source;
        int v = edges[i].destination;
        int weight = edges[i].weight;

        if (dist[u] != INF &&
            dist[u] + weight < dist[v]) {

            printf("\nNegative weight cycle detected!\n");
            return;
        }
    }

    printf("\n===== BELLMAN-FORD RESULT =====\n");

    if (dist[destination] == INF) {
        printf("No route available!\n");
        return;
    }

    printf("Source: %s\n", locations[source]);
    printf("Destination: %s\n", locations[destination]);

    printf("Shortest Path: ");
    printPath(parent, destination);

    printf("\nShortest Distance: %d units\n",
           dist[destination]);
}

/* =====================================================
                  FLOYD-WARSHALL ALGORITHM
   ===================================================== */

void runFloydWarshall(void) {

    int dist[V][V];

    printf("\n===== FLOYD-WARSHALL ALL-PAIRS SHORTEST PATH =====\n");

    /* Initialize distance matrix */
    for (int i = 0; i < V; i++) {

        for (int j = 0; j < V; j++) {

            if (i == j) {
                dist[i][j] = 0;
            }
            else if (graph[i][j] > 0) {
                dist[i][j] = graph[i][j];
            }
            else {
                dist[i][j] = INF;
            }
        }
    }

    /* Floyd-Warshall algorithm */
    for (int k = 0; k < V; k++) {

        for (int i = 0; i < V; i++) {

            for (int j = 0; j < V; j++) {

                if (dist[i][k] != INF &&
                    dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {

                    dist[i][j] =
                        dist[i][k] + dist[k][j];
                }
            }
        }
    }

    /* Display shortest distance matrix */
    printf("\n===== SHORTEST DISTANCE MATRIX =====\n\n");

    printf("%-18s", "Location");

    for (int i = 0; i < V; i++) {
        printf("%-18d", i);
    }

    printf("\n");

    for (int i = 0; i < V; i++) {

        printf("%-18s", locations[i]);

        for (int j = 0; j < V; j++) {

            if (dist[i][j] == INF) {
                printf("%-18s", "INF");
            }
            else {
                printf("%-18d", dist[i][j]);
            }
        }

        printf("\n");
    }

    printf("\n===== LOCATION REFERENCE =====\n");

    for (int i = 0; i < V; i++) {
        printf("%d. %s\n", i, locations[i]);
    }

    printf("\nAll-pairs shortest distances calculated successfully!\n");
}

/* =====================================================
                    0/1 KNAPSACK
   ===================================================== */

#define MAX_ITEMS 20
#define MAX_CAPACITY 1000

void runKnapsack(void) {

    int n, capacity;

    int weight[MAX_ITEMS + 1];
    int value[MAX_ITEMS + 1];

    int dp[MAX_ITEMS + 1][MAX_CAPACITY + 1];

    printf("\n===== 0/1 KNAPSACK ALGORITHM =====\n");

    printf("Enter number of items (1-%d): ", MAX_ITEMS);

    if (scanf("%d", &n) != 1 ||
        n < 1 || n > MAX_ITEMS) {

        printf("Invalid number of items!\n");
        return;
    }

    printf("Enter knapsack capacity (1-%d): ",
           MAX_CAPACITY);

    if (scanf("%d", &capacity) != 1 ||
        capacity < 1 || capacity > MAX_CAPACITY) {

        printf("Invalid capacity!\n");
        return;
    }

    for (int i = 1; i <= n; i++) {

        printf("\nItem %d weight: ", i);

        if (scanf("%d", &weight[i]) != 1 ||
            weight[i] <= 0) {

            printf("Invalid weight!\n");
            return;
        }

        printf("Item %d value: ", i);

        if (scanf("%d", &value[i]) != 1 ||
            value[i] < 0) {

            printf("Invalid value!\n");
            return;
        }
    }

    /* Initialize DP table */

    for (int i = 0; i <= n; i++) {

        for (int w = 0; w <= capacity; w++) {

            if (i == 0 || w == 0) {
                dp[i][w] = 0;
            }

            else if (weight[i] <= w) {

                int include =
                    value[i] +
                    dp[i - 1][w - weight[i]];

                int exclude =
                    dp[i - 1][w];

                dp[i][w] =
                    (include > exclude) ?
                    include : exclude;
            }

            else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    /* Display result */

    printf("\n===== KNAPSACK RESULT =====\n");

    printf("Maximum Value: %d\n",
           dp[n][capacity]);

    printf("\nSelected Items:\n");

    int remainingCapacity = capacity;

    for (int i = n; i > 0; i--) {

        if (dp[i][remainingCapacity] !=
            dp[i - 1][remainingCapacity]) {

            printf("Item %d | Weight: %d | Value: %d\n",
                   i, weight[i], value[i]);

            remainingCapacity -= weight[i];
        }
    }

    printf("\nMaximum Capacity: %d\n", capacity);

    printf("Used Capacity: %d\n",
           capacity - remainingCapacity);

    printf("\n0/1 Knapsack completed successfully!\n");
}
