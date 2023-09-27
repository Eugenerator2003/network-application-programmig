#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const short BUFF_SIZE = 1024;

void start_recv(SOCKET self);
void out_column(int* row);

int main(void)
{
	ifstream in;
	string SERVER_IP;
	in.open("addr.txt", ifstream::in);
	if (in.is_open()) {
		getline(in, SERVER_IP);
	}
	in.close();

	cout << "SERVER IP IS " << SERVER_IP << endl;

	//Key constants
	//const char SERVER_IP[] = "192.168.96.47";					// Enter IPv4 address of Server
	const short SERVER_PORT_NUM = 56666;				// Enter Listening port on Server side

	// Key variables for all program
	int erStat;										// For checking errors in sockets functions

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP.c_str(), &ip_to_num);


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

	// Socket initialization
	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
	}
	else
		cout << "Client socket initialization is OK" << endl;

	// Establishing a connection to Server
	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERVER_PORT_NUM);

	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;
	
	start_recv(ClientSock);

	closesocket(ClientSock);
	WSACleanup();

	return 0;
}

void out_row(int* row) {
	int n = sizeof(row) / sizeof(int) + 1;
	for (int i = 0; i < n; i++) {
		cout << row[i] << " ";
	}
	cout << endl;
}

void start_recv(SOCKET self) {
	while (true) {
		char* buffer = new char[BUFF_SIZE];

		int* column = new int[BUFF_SIZE / 4];

		short packet_size = recv(self, buffer, BUFF_SIZE, 0);
		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send data to server. Error # " << WSAGetLastError() << endl;
			return;
		}

		int rank_arr[] = { 0 };
		memcpy(rank_arr, buffer, 4);
		int rank = rank_arr[0];
		int n_elements = (rank + 1) * 4;

		while (true) {
			short packet_size = recv(self, buffer, BUFF_SIZE, 0);
			//cout << buffer << endl;
			if (!strcmp(buffer, "ex")) {
				return;
			}

			if (!strcmp(buffer, "next")) {
				break;
			}

			int* column = new int[rank + 1];
			cout << "received: ";
			memcpy(column, buffer, n_elements);
			out_row(column);
			for (int i = 0; i < rank; i++) {
				column[i] = column[i] * column[rank];
			}
			cout << "computed: ";
			out_row(column);
			memcpy(buffer, column, n_elements - 4);
			packet_size = send(self, buffer, n_elements - 4, 0);
			if (packet_size == SOCKET_ERROR) {
				cout << "Can't send data to server. Error # " << WSAGetLastError() << endl;
				return;
			}
		}
	}
}
