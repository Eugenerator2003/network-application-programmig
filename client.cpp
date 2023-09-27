#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

const short BUFF_SIZE = 1024;

void start_recv(int self);
void out_row(int* row);

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

	int clientsock;
	
    if ((clientsock= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    int PORT = 56666;
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "\nInvalid address/ Address not supported" << endl;
        return -1;
    }

    int status {0};
    if ((status = connect(clientsock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        cout << "\nConnection Failed" << endl;
        return -1;
    }
	
	start_recv(clientsock);

	close(clientsock);
	shutdown(clientsock, SHUT_RDWR);
	return 0;
}

void out_row(int* row) {
	int n = sizeof(row) / sizeof(int) + 1;
	for (int i = 0; i < n; i++) {
		cout << row[i] << " ";
	}
	cout << endl;
}

void start_recv(int self) {
	while (true) {
		char* buffer = new char[BUFF_SIZE];

		int* column = new int[BUFF_SIZE / 4];

        // read??
		short packet_size = recv(self, buffer, BUFF_SIZE, 0);

		int rank_arr[] = { 0 };
		memcpy(rank_arr, buffer, 4);
		int rank = rank_arr[0];
        cout << "recv rank is " << rank << endl;
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
		}
	}
}