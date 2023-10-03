#include "WsaClient.h"
#include "ThreadPool.h"

WsaClient::WsaClient() : WsaClient("127.0.0.1")
{
}

WsaClient::WsaClient(std::string serverAddress) : WsaClient(serverAddress, 56666)
{
}

WsaClient::WsaClient(std::string serverAddress, int port)
{
	this->serverAddress = serverAddress;
	this->serverPort = port;
	this->isConnected = false;
	this->deconstructed = false;
	this->mtx = new std::mutex();

	//Key constants
	//const char SERVER_IP[] = "192.168.96.47";					// Enter IPv4 address of Server
	const short SERVER_PORT_NUM = 56666;				// Enter Listening port on Server side

	// Key variables for all program
	int erStat;										// For checking errors in sockets functions

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"

	// WinSock initialization
	WSADATA wsData;
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		std::cout << "Error WinSock version initializaion #";
		std::cout << WSAGetLastError();
		return;
	}
	else {
		std::cout << "WinSock initialization is OK" << std::endl;

		// Socket initialization
		SOCKET clientsock = socket(AF_INET, SOCK_STREAM, 0);

		if (clientsock == INVALID_SOCKET) {
			std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
			closesocket(clientsock);
			WSACleanup();
		}
		else {
			std::cout << "Client socket initialization is OK" << std::endl;
			this->clientsock = clientsock;
		}
	}
}

WsaClient::~WsaClient()
{
	this->closeConnection();
}

void WsaClient::connectToServer()
{
	this->pool = new Pool::ThreadPool(2);

	in_addr ip_to_num;
	inet_pton(AF_INET, this->serverAddress.c_str(), &ip_to_num);

	// Establishing a connection to Server
	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(this->serverPort);

	int erStat = connect(this->clientsock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << std::endl;
		closesocket(this->clientsock);
		WSACleanup();
		return;
	}
	else {
		std::cout << "Connection established SUCCESSFULLY." << std::endl;
		this->isConnected = true;
		this->pool->enqueue(std::function<void()>([this]() { sending(); }));
		this->pool->enqueue(std::function<void()>([this]() { receiving(); }));
	}
}

bool WsaClient::ready()
{
	return this->isConnected;
}

void WsaClient::sendToServer(GraphData info)
{
	this->mtx->lock();
	this->bufferSending.push(info);
	this->mtx->unlock();
}

void WsaClient::closeConnection()
{
	if (!this->deconstructed) {
		closesocket(this->clientsock);
		WSACleanup();
		this->isConnected = false;
		delete this->pool;
		delete this->mtx;
		this->deconstructed = true;
	}
}

void WsaClient::sending()
{
	int values[2];
	char buffer[8];
	std::cout << "Start sending" << std::endl;
	while (this->isConnected) {
		if (!this->bufferSending.empty()) {
			this->mtx->lock();
			GraphData data = this->bufferSending.front();
			this->bufferSending.pop();
			this->bufferReceiving.push(data);
			this->mtx->unlock();

			values[0] = data.source;
			values[1] = data.destination;
			memcpy(buffer, values, 8);
			int state = send(this->clientsock, buffer, 8, 0);

			if (state == SOCKET_ERROR)
			{
				std::cout << "Connection closed from sending (error)" << std::endl;
				break;
			}
			std::cout << "Sended to server" << std::endl;
		}
	}
	std::cout << "Sending stopped" << std::endl;
}

void WsaClient::receiving()
{
	int value;
	char buffer[4];
	std::cout << "Start receiving" << std::endl;
	while (this->isConnected) {
		int state = recv(this->clientsock, buffer, 4, 0);

		if (state == SOCKET_ERROR)
		{
			isConnected = false;
			std::cout << "Connection closed from receiving (error or exit)" << std::endl;
			break;
		}

		if (!strcmp(buffer, "ex")) {
			isConnected = false;
			std::cout << "Connection closed from receiving (from server)" << std::endl;
			break;
		}

		this->mtx->lock();
		GraphData data = this->bufferReceiving.front();
		this->bufferReceiving.pop();
		this->mtx->unlock();

		memcpy(&value, buffer, 4);
		std::cout << "Min distance from " << data.source << " to " << data.destination << " is " << value << std::endl;
	}
	std::cout << "Receiving stopped" << std::endl;
}


