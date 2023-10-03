#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <queue>
#include <functional>
#include "ThreadPool.h"

#pragma comment(lib, "ws2_32.lib")

struct GraphData {
	int source;
	int destination;

	GraphData() {

	};

	GraphData(int source, int destination) {
		this->source = source;
		this->destination = destination;
	};
};

class WsaClient
{
public:
	WsaClient();
	WsaClient(std::string serverAddress);
	WsaClient(std::string serverAddress, int port);

	~WsaClient();

	void connectToServer();
	bool ready();
	void sendToServer(GraphData info);
	void closeConnection();

private:
	SOCKET clientsock;

	int serverPort;
	std::string serverAddress;
	bool isConnected;

	std::mutex* mtx;
	std::queue<GraphData> bufferSending;
	std::queue<GraphData> bufferReceiving;
	Pool::ThreadPool *pool;

	bool deconstructed;

	void sending();
	void receiving();
};

