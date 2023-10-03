#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "WsaServer.h"

struct Edge {
    int source, destination, weight;
};

std::vector<int> bellmanFord(std::vector<Edge>& edges, int numVertices, int source) {
    std::vector<int> distance(numVertices, INT_MAX);
    source--;
    distance[source] = 0;

    // Релаксация рёбер
    for (int i = 1; i < numVertices; ++i) {
        for (const Edge& edge : edges) {
            if (distance[edge.source] != INT_MAX && distance[edge.source] + edge.weight < distance[edge.destination]) {
                distance[edge.destination] = distance[edge.source] + edge.weight;
            }
        }
    }

    // Проверка наличия циклов с отрицательным весом
    for (const Edge& edge : edges) {
        if (distance[edge.source] != INT_MAX && distance[edge.source] + edge.weight < distance[edge.destination]) {
            //std::cout << "Граф содержит циклы с отрицательным весом!\n";
            return std::vector<int>();
        }
    }

    // Вывод кратчайших путей
    //std::cout << "Вершина   Расстояние от источника\n";
    for (int i = 0; i < numVertices; ++i) {
        //std::cout << i << "\t\t" << distance[i] << "\n";
    }

    return distance;
}

int numVertices = 10;
std::vector<Edge> edges = {
        {0, 1, 5},
        {0, 2, 3},
        {1, 2, 2},
        {1, 3, 7},
        {2, 3, 4},
        {3, 4, 1},
        {4, 0, 2},
        {0, 5, 3},
        {5, 2, 6},
        {4, 5, 8},
        {6, 7, 2},
        {7, 8, 4},
        {8, 6, 6},
        {6, 9, 5},
        {7, 9, 1},
        {9, 8, 3},
        {9, 5, 9},
        {2, 7, 5},
        {1, 4, 3},
        {3, 6, 4},
        {8, 5, 2},
        {2, 4, 1},
        {7, 5, 3},
        {9, 0, 7}
};


int getMinDistance(int source, int destination) {
    if (source < 0 || source > numVertices || destination < 0 || destination > numVertices) {
        return -1;
    }

    std::vector<int> dists = bellmanFord(edges, numVertices, source);
    return dists[destination - 1];
}

int main()
{
    std::ifstream in;
    std::string IP_SERV;
    in.open("server_info.txt", std::ifstream::in);
    if (in.is_open()) {
        getline(in, IP_SERV);
    }
    in.close();

    //std::string IP_SERV = "127.0.0.1";

    WsaServer server = WsaServer(IP_SERV, std::function<int (int, int)>(getMinDistance));

    std::cout << "IP: " << server.getIp() << " PORT: " << server.getPort() << std::endl;

    int clients{ 0 };
    std::cout << "Enter the number of clients: ";
    std::cin >> clients;
    std::cin.ignore();
    
    server.setGraphFunc(getMinDistance);
    server.listenClients(clients);
    server.startServer(clients);
    std::string ans;

    while (true) {
        std::cout << "Enter ex to exit" << std::endl;
        getline(std::cin, ans);
        if (ans == "ex") {
            server.endServer();
            break;
        }
    }

    std::cout << "To continue press enter";
    std::getline(std::cin, ans);
}