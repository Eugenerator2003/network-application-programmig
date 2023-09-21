#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <random>
#include <regex>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int max_value = 10;

int** getMatrx();
int** transpose(int** matrx, int rank);
bool send_rank(int rank);
bool start_solution(int rank);
void out_matr(int**matrx, int rank,const char& name);
void recvfromclient(int* sended, int* ccolumns, char* buffer, const int nelements, const int rank, int* rows, int** C);

int n_clients = 2;
SOCKET* clients = new SOCKET[n_clients];

int main(void)
{
	ifstream in;
	string IP_SERV;
	in.open("addr.txt", ifstream::in);
	if (in.is_open()) {
		getline(in, IP_SERV);
	}
	in.close();

	cout << "IP IS " << IP_SERV << endl;

	//Key constants
	//const char IP_SERV[] = "192.168.96.47";			// Enter local Server IP address
	const int PORT_NUM = 56666;				// Enter Open working server port
	const short BUFF_SIZE = 1024;			// Maximum size of buffer for exchange info between server and client

	// Key variables for all program
	int erStat;								// Keeps socket errors status

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, IP_SERV.c_str(), &ip_to_num);

	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}


	// WinSock initialization
	WSADATA wsData;

	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;

	// Server socket initialization
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;

	// Server socket binding
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT_NUM);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to Server info is OK" << endl;

	//Starting to listen to any Clients
	erStat = listen(ServSock, SOMAXCONN_HINT(2));

	if (erStat != 0) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listening..." << endl;
	}

	//Client socket creation and acception in case of connection
	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

	int clientInfo_size = sizeof(clientInfo);

	for (int i = 0; i < n_clients; i++) {
		SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
		clients[i] = ClientConn;

		if (ClientConn == INVALID_SOCKET) {
			cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			//return 1;
		}
		else {
			cout << "Connection to a client established successfully" << endl;
			char clientIP[22];

			inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);	// Convert connected client's IP to standard string format

			cout << "Client connected with IP address " << clientIP << endl;
		}
	}

	string ans = "";
	regex reg = regex("go [0-9]+");
	while (true) {

		cout << "start solution: go *rank*" << endl;
		cout << "exit: ex" << endl;
		getline(cin, ans);

		if (regex_match(ans, reg)) {
			int rank = stoi(ans.substr(3, ans.size()));
			cout << "rank is " << rank << endl;
			if (!send_rank(rank)) {
				break;
			}
			if (!start_solution(rank)) {
				break;
			}
		}
		else if (ans == "ex") {
			break;
		}
		else {
			cout << "There is no same answer" << endl;
		}
	}

	for (int i = 0; i < n_clients; i++) {
		send(clients[i], "ex", 3 * 4, 0);
		closesocket(clients[i]);
	}
	closesocket(ServSock);
	WSACleanup();
	delete clients;

	return 0;

}

int** getMatrx(int rank, bool rand = true)
{
	random_device seed;
	mt19937 mersenne(seed());
	int** matrx = new int* [rank];
	for (int i = 0; i < rank; i++) {
		matrx[i] = new int[rank];
		for (int j = 0; j < rank; j++) {
			if (rand) {
				matrx[i][j] = mersenne() % (1 + max_value);
			}
			else {
				matrx[i][j] = 0;
			}
		}
	}
	return matrx;
}

void out_matr(int** matrx, int rank,const char *name) {
	cout << "Matrix " << name << endl;
	for (int i = 0; i < rank; i++) {
		for (int j = 0; j < rank; j++) {
			cout << matrx[i][j] << "\t";
		}
		cout << endl;
	}
}

int** transpose(int **matrx, int rank) {
	int t = 0;
	for (int i = 0; i < rank; i++) {
		for (int j = i; j < rank; j++) {
			t = matrx[i][j];
			matrx[i][j] = matrx[j][i];
			matrx[j][i] = t;
		}
	}
	return matrx;
}

bool send_rank(const int rank) {
	char buffer[4];
	int rankarr[1] = { rank };
	memcpy(buffer, rankarr, 4);
	for (int c = 0; c < n_clients; c++) {
		short packet_size = send(clients[c], buffer, 4, 0);
		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send data to client " << c + 1 << ".Error # " << WSAGetLastError() << endl;
			return false;
		}
	}
	return true;
}

bool start_solution(const int rank) {
	int** A = getMatrx(rank);
	int** B = getMatrx(rank);
	int** C = getMatrx(rank, false);

	out_matr(A, rank, "A");
	out_matr(B, rank, "B");

	int nelements = (rank + 1) * 4;
	int* rows = new int[n_clients];
	char* buffer = new char[(rank + 1) * 4];
	int* ccolumns = new int[rank];
	int sended = 0;
	for (int j = 0; j < rank; j++) {
		int* acolumns = new int[rank + 1];
		for (int i = 0; i < rank; i++) {
			acolumns[i] = A[i][j];
			ccolumns[i] = 0;
		}

		for (int i = 0; i < rank; i++) {
			// получение
			if (sended == n_clients) {
				recvfromclient(&sended, ccolumns, buffer, nelements, rank, rows, C);
			}

			int elem = B[j][i];
			acolumns[rank] = elem;
			memcpy(buffer, acolumns, nelements);

			// отправка
			if (sended < n_clients) {
				rows[sended] = i;
				short packet_size = send(clients[sended], buffer, nelements, 0);
				if (packet_size == SOCKET_ERROR) {
					cout << "Can't send data to client " << sended + 1 << ".Error # " << WSAGetLastError() << endl;
					delete rows, buffer, A, B, C, acolumns;
					return false;
				}
				sended++;
			}
		}
	}
	for (int i = 0; i < n_clients; i++) {
		if (rows[i] >= 0) {
			recvfromclient(&sended, ccolumns, buffer, nelements, rank, rows, C);
			break;
		}
		send(clients[i], "next", 5 * 4, 0);
	}
	transpose(C, rank);
	out_matr(C, rank, "C");
	delete rows, buffer, A, B, C;
	return true;
}

void recvfromclient(int *sended, int*ccolumns, char*buffer, const int nelements, const int rank, int* rows, int** C) {
	for (int c = 0; c < n_clients; c++) {
		if (rows[c] >= 0) {
			short packet_size = recv(clients[c], buffer, nelements - 4, 0);
			memcpy(ccolumns, buffer, nelements - 4);
			for (int k = 0; k < rank; k++) {
				C[rows[c]][k] += ccolumns[k];
			}
			rows[c] = -1;
			//out_matr(C, rank, "C");
		}
	}
	*sended = 0;
}
