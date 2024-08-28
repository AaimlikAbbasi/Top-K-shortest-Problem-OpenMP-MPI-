#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mpi.h>
#include <queue>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <ctime>
#include <cstdlib>
using namespace std;

#define MAX_VERTICES 3793
#define INF 999999999

struct Edge {
    std::string source;
    std::string target;
    int weight;
    std::string type;
};

Edge parse_line(const std::string& line, std::unordered_map<std::string, int>& cityIds) {
    std::istringstream iss(line);
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(iss, field, ',')) {
        fields.push_back(field);
    }

    if (fields.size() != 4) {
        throw std::invalid_argument("Error parsing line: " + line);
    }

    std::string source = fields[0];
    std::string target = fields[1];
    int weight = std::stoi(fields[2]);
    std::string type = fields[3];

    if (cityIds.find(source) == cityIds.end()) {
        cityIds[source] = cityIds.size() + 1;
    }

    if (cityIds.find(target) == cityIds.end()) {
        cityIds[target] = cityIds.size() + 1;
    }

    std::string sourceName = "";
    std::string targetName = "";
    for (auto& pair : cityIds) {
        if (pair.second == cityIds[source]) {
            sourceName = pair.first;
        }
        if (pair.second == cityIds[target]) {
            targetName = pair.first;
        }
    }

    return Edge{ sourceName, targetName, weight, type };
}

int read_graph(const std::string& filename, std::vector<Edge>& edges) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 0;
    }

    int numEdges = 0;
    std::string line;
    std::unordered_map<std::string, int> cityIds;

    while (std::getline(file, line)) {
        try {
            Edge edge = parse_line(line, cityIds);
            edges.push_back(edge);
            numEdges++;
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    file.close();
    return numEdges;
}

void display_graph(const std::vector<Edge>& edges) {
    std::cout << "Graph:\n";
    for (const Edge& edge : edges) {
        std::cout << edge.source << " -> " << edge.target << " : " << edge.weight << " : " << edge.type << std::endl;
    }
}

void findShortestPath(int edges[][3], int n, int m, int start, int end, int k) {
    // Initialize graph
    std::vector<std::vector<std::pair<int, int>>> g(n);
    for (int i = 0; i < m; i++) {
        g[edges[i][0]].push_back({ edges[i][1], edges[i][2] });
    }

    // Vector to store distances
    std::vector<std::vector<int>> dis(n, std::vector<int>(k, INF));

    // Initialize priority queue to store pair of {distance, vertex}
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
    pq.push({ 0, start });
    dis[start][0] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();
        if (dis[u][k - 1] < d)
            continue;
        std::vector<std::pair<int, int>> v = g[u];
        for (int i = 0; i < v.size(); i++) {
            int dest = v[i].first;
            int cost = v[i].second;
            if (d + cost < dis[dest][k - 1]) {
                dis[dest][k - 1] = d + cost;
                std::sort(dis[dest].begin(), dis[dest].end());
                pq.push({ d + cost, dest });
            }
        }
    }

    // Printing K shortest paths
    std::cout << "Shortest paths from " << start << " to " << end << ":\n";
    for (int i = 0; i < k; i++) {
        std::cout << "Path " << i + 1 << ": ";
        if (dis[end][i] == INF) {
            std::cout << "No path";
        }
        else {
            std::cout << dis[end][i];
        }
        std::cout << std::endl;
    }
}


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int num_processes, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<Edge> edges;
    std::map<std::string, int> cityIndices;

    int numVertices = 0;
    int numEdges = 0;
    int edgesArray[MAX_VERTICES][3] = { 0 }; // Initialize to zero

    if (rank == 0)
    {
        numEdges = read_graph("classic.txt", edges);
        if (numEdges == 0) {
            std::cerr << "Error reading graph from file\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        display_graph(edges);

        for (const Edge& edge : edges) {
            if (cityIndices.find(edge.source) == cityIndices.end()) {
                cityIndices[edge.source] = numVertices++;
               
            }
            if (cityIndices.find(edge.target) == cityIndices.end()) {
                cityIndices[edge.target] = numVertices++;
            }
        }
        cout<<numEdges<<endl;
        for (int i = 0; i < numEdges; ++i) {
            
            edgesArray[i][0] = cityIndices[edges[i].source];
            edgesArray[i][1] = cityIndices[edges[i].target];
            edgesArray[i][2] = edges[i].weight;
        }

      /*  std::cout << "Edges array:" << std::endl;
        for (int i = 0; i < numEdges; ++i) {
            std::cout << "{ " << edgesArray[i][0] << ", " << edgesArray[i][1] << ", " << edgesArray[i][2] << " }" << std::endl;
        }*/
        MPI_Bcast(&numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&numEdges, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(edgesArray, numEdges * 3, MPI_INT, 0, MPI_COMM_WORLD);

        srand(time(NULL));
        std::vector<std::pair<int, int>> randomPairs;
        for (int i = 0; i < 10; ++i) {
            int startVertex = rand() % numVertices;
            int endVertex = rand() % numVertices;
            randomPairs.push_back({ startVertex, endVertex });
        }



        for (int i = 1; i < num_processes; ++i) {
            if (!randomPairs.empty()) {
                std::pair<int, int> pairToSend = randomPairs.back();
                randomPairs.pop_back();
                MPI_Send(&pairToSend, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
                std::cout << "Process 0 sent pair: Start Vertex: " << pairToSend.first << ", End Vertex: " << pairToSend.second << " to Process " << i << std::endl;
            }
        }

        if (!randomPairs.empty()) {
            std::pair<int, int> pairToProcess = randomPairs.back();
            randomPairs.pop_back();
            findShortestPath(edgesArray, numVertices, numEdges, pairToProcess.first, pairToProcess.second, 3);
        }
    }
    else {
        MPI_Bcast(&numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&numEdges, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(edgesArray, numEdges * 3, MPI_INT, 0, MPI_COMM_WORLD);

        // Process received pair
        std::pair<int, int> receivedPair;
        MPI_Recv(&receivedPair, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " received pair: Start Vertex: " << receivedPair.first << ", End Vertex: " << receivedPair.second << std::endl;
        findShortestPath(edgesArray, numVertices, numEdges, receivedPair.first, receivedPair.second, 3);
    }


    MPI_Finalize();

    return 0;
}