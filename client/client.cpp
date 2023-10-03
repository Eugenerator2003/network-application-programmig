#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <regex>
#include "WsaClient.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main(void)
{
	ifstream in;
	string SERVER_IP;
	string ans;
	in.open("server_addr.txt", ifstream::in);
	if (in.is_open()) {
		getline(in, SERVER_IP);
	}
	in.close();

	//string SERVER_IP = "127.0.0.1";

	cout << "SERVER IP IS " << SERVER_IP << endl;

	WsaClient client = WsaClient(SERVER_IP);

	client.connectToServer();

	if (client.ready()) {
		cout << "Enter source and destination vertices or ex to exit: " << endl;
		while (true) {
			regex reg = regex("[0-9]+ [0-9]+");
			getline(cin, ans);
			if (ans == "ex") {
				client.closeConnection();
				break;
			}
			else if (regex_match(ans, reg)) {
				char* line = const_cast<char*>(ans.c_str());
				char* next_token;

				GraphData data;
				char* pos = strtok_s(line, " ", &next_token);
				data.source = atof(pos);
				pos = strtok_s(NULL, " ", &next_token);
				data.destination = atof(pos);

				client.sendToServer(data);
			}
			else {
				cout << "Wrong answer" << endl;
			}
		}
	}

	cout << "To continue press enter";
	getline(cin, ans);

	return 0;
}