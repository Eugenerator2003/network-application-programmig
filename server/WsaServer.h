#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <mutex>
#include <queue>
#include "ThreadPool.h"

#pragma comment(lib, "Ws2_32.lib")

struct GraphData {
	SOCKET *socket;
	int source;
	int destination;

	GraphData(SOCKET *socket, int source, int destination) {
		this->socket = socket;
		this->source = source;
		this->destination = destination;
	};
};

class WsaServer
{
public:
	WsaServer(std::function<int(int, int)> graphFunc);
	WsaServer(std::string IP, std::function<int(int, int)> graphFunc);
	WsaServer(std::string IP, int port, std::function<int(int, int)> graphFunc);

	~WsaServer();

	int getPort();
	std::string getIp();

	void setGraphFunc(std::function<int(int, int)> graphFunc);

	int listenClients(int nclients);
	int startServer(int threadcount);
	void endServer();

private:
	int port;
	std::string ip;
	SOCKET sock;
	int nclients;
	SOCKET* clients;
	std::function<int(int, int)> graphFunc;

	Pool::ThreadPool* pool;
	int handleIdx = 0;
	bool serverWorking = false;

	std::mutex* mtx;
	
	std::queue<GraphData> graphQueueCommon;
	std::queue<GraphData> graphQueueComputing;

	bool deconstructed;

	void compute();
	virtual void computeUnit();
	virtual void requestHandle();
};

