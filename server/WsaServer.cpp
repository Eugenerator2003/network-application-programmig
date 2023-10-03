#include <chrono>
#include "WsaServer.h"

WsaServer::WsaServer(std::function<int(int, int)> graphFunc) : WsaServer("127.0.0.1", graphFunc) {
	//WsaServer("127.0.0.1");
}

WsaServer::WsaServer(std::string IP, std::function<int(int, int)> graphFunc) : WsaServer(IP, 56666, graphFunc) {
	//WsaServer(IP, 56666);
}

WsaServer::WsaServer(std::string ip, int port, std::function<int(int, int)> graphFunc)
{
	this->mtx = new std::mutex();
	this->graphFunc = graphFunc;
	this->graphQueueCommon = std::queue<GraphData>();
	this->graphQueueComputing = std::queue<GraphData>();
	this->ip = ip;
	this->port = port;
	this->deconstructed = false;

	// Key variables for all program
	int erStat;								// Keeps socket errors status

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, ip.c_str(), &ip_to_num);

	if (erStat <= 0) {
		std::cout << "Error in IP translation to special numeric format" << std::endl;
	}
	else {
		// WinSock initialization
		WSADATA wsData;

		erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

		if (erStat != 0) {
			std::cout << "Error WinSock version initializaion #";
			std::cout << WSAGetLastError();
		}
		else {
			std::cout << "WinSock initialization is OK" << std::endl;

			// Server socket initialization
			SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);

			if (serverSock == INVALID_SOCKET) {
				std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
				closesocket(serverSock);
				WSACleanup();
			}
			else {
				std::cout << "Server socket initialization is OK" << std::endl;

				// Server socket binding
				sockaddr_in servInfo;
				ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure

				servInfo.sin_family = AF_INET;
				servInfo.sin_addr = ip_to_num;
				servInfo.sin_port = htons(port);

				erStat = bind(serverSock, (sockaddr*)&servInfo, sizeof(servInfo));

				if (erStat != 0) {
					std::cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << std::endl;
					closesocket(serverSock);
					WSACleanup();
					exit(1);
				}
				else {
					std::cout << "Binding socket to Server info is OK" << std::endl;
					this->sock = serverSock;
				}
			}
		}
	}
}

WsaServer::~WsaServer()
{
	if (!this->deconstructed) {
		this->endServer();
	}
}

int WsaServer::getPort()
{
	return this->port;
}

std::string WsaServer::getIp() {
	return this->ip;
}

void WsaServer::setGraphFunc(std::function<int(int, int)> func)
{
	this->graphFunc = func;
}

int WsaServer::listenClients(int nclients)
{
	this->nclients = nclients;

	//Starting to listen to any Clients
	int erStat = listen(this->sock, nclients);

	if (erStat != 0) {
		std::cout << "Can't start to listen to. Error # " << WSAGetLastError() << std::endl;
		closesocket(this->sock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Listening..." << std::endl;
	}

	//Client socket creation and acception in case of connection
	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

	int clientInfo_size = sizeof(clientInfo);

	this->clients = new SOCKET[nclients];
	for (int i = 0; i < nclients; i++) {
		SOCKET ClientConn = accept(this->sock, (sockaddr*)&clientInfo, &clientInfo_size);
		clients[i] = ClientConn;

		if (ClientConn == INVALID_SOCKET) {
			std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << std::endl;
			closesocket(this->sock);
			closesocket(ClientConn);
			WSACleanup();
			return 1;
		}
		else {
			std::cout << "Connection to a client established successfully" << std::endl;
			char clientIP[22];

			inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);	// Convert connected client's IP to standard string format

			std::cout << "Client connected with IP address " << clientIP << std::endl;
		}
	}
	return 0;
}

int WsaServer::startServer(int threadcount)
{
	if (this->nclients >= threadcount) {
		threadcount = this->nclients + 1 + 2;
	}
	this->pool = new Pool::ThreadPool(threadcount);
	this->serverWorking = true;
	for (int i = 0; i < this->nclients; i++) {
		pool->enqueue(std::function<void()>([this]() { requestHandle(); }));
	}
	pool->enqueue(std::function<void()>([this]() { compute(); }));
	return 1;
}

void WsaServer::endServer()
{
	this->serverWorking = false;

	const char* buffer = "ex";

	for (int i = 0; i < this->nclients; i++) {
		send(this->clients[i], buffer, 3, 0);
	}

	for (int i = 0; i < this->nclients; i++) {
		closesocket(this->clients[i]);
	}

	closesocket(this->sock);
	WSACleanup();

	delete this->pool;
	delete this->mtx;
	this->deconstructed = deconstructed;
}


void WsaServer::requestHandle()
{
	this->mtx->lock();
	int idx = handleIdx;
	handleIdx++;
	this->mtx->unlock();

	char* buf = new char[8];
	int* values = new int[2];

	while (this->serverWorking) {
		int packet_size = recv(this->clients[idx], buf, 8, 0);
		if (packet_size != SOCKET_ERROR) {
			if (!strcmp(buf, "ex")) {
				break;
			}
			memcpy(values, buf, 8);
			this->mtx->lock();
			this->graphQueueCommon.push(GraphData(&this->clients[idx], values[0], values[1]));
			this->mtx->unlock();
		}
		else {
			break;
		}
	}

	std::cout << "connection with " << idx << " client has ended" << std::endl;
}

void WsaServer::compute()
{
	while (this->serverWorking)
	{
		while (!this->graphQueueCommon.empty()) {
			this->mtx->lock();
			int size = this->graphQueueCommon.size();
			while (size)
			{
				this->pool->enqueue(std::function<void()>([this]() { computeUnit(); }));
				this->graphQueueComputing.push(this->graphQueueCommon.front());
				this->graphQueueCommon.pop();
				size--;
			}
			this->mtx->unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void WsaServer::computeUnit() {
	char buffer[4];
	this->mtx->lock();
	GraphData data = this->graphQueueComputing.front();
	this->graphQueueComputing.pop();
	/*this->mtx->unlock();*/
	int result = this->graphFunc(data.source, data.destination);
	std::cout << "recv from " << *data.socket << " socket: from " << data.source << " to " << data.destination << " is " << result << std::endl;
	memcpy(buffer, &result, 4);
	send(*data.socket, buffer, 4, 0);
	this->mtx->unlock();
}

