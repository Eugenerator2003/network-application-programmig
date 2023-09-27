#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <regex>
#include <random>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int max_value = 10;

int** getMatrx();
int** transpose(int** matrx, int rank);
bool send_rank(int rank);
bool start_solution(int rank);
void out_matr(int**matrx, int rank,const char& name);
void recvfromclient(int* sended, int* ccolumns, char* buffer, const int nelements, const int rank, int* rows, int** C);

int n_clients;
int* clients;

int main(void)
{
    int opt = 1;
	cout << "Enter number of clients: ";
	cin >> n_clients;
	clients = new int[n_clients];

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

	int serversock {0};

    if ((serversock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(serversock, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    //address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, IP_SERV.c_str(), &(address.sin_addr));
    address.sin_port = htons(PORT_NUM);
  
    // Forcefully attaching socket to the port 8080
    if (bind(serversock, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serversock, n_clients) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < n_clients; i++) {
        int clientsock {0};
        if ((clientsock = accept(serversock, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        clients[i] = clientsock;
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
		close(clients[i]);
	}
	close(serversock);
	shutdown(serversock, SHUT_RDWR);
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
		// if (packet_size == SOCKET_ERROR) {
		// 	cout << "Can't send data to client " << c + 1 << ".Error # " << WSAGetLastError() << endl;
		// 	return false;
		// }
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
