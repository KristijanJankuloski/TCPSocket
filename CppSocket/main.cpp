#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main() {
	// INIT WINSOCK
	WSADATA wsData;
	WORD version = MAKEWORD(2, 2);
	int BUFFER_SIZE = 30720;

	int wsOk = WSAStartup(version, &wsData);
	if (wsOk != 0) {
		cerr << "Cannot initialize winsock" << endl;
		return -1;
	}

	// CREATE SOCKET
	SOCKET listening = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listening == INVALID_SOCKET) {
		cerr << "Cannot create socket" << endl;
		return -1;
	}

	// BIND SOKCET TO IP AND PORT
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8080);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// MARK SOCKET FOR LISTENING
	listen(listening, SOMAXCONN);

	// WAIT FOR CONNECTION
	sockaddr_in client;
	int clientSize = sizeof(client);
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Cannot create client" << endl;
		return -1;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0) {
		cout << host << " connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	// WHILE LOOP
	char buffer[30720];
	while (true) {
		ZeroMemory(buffer, BUFFER_SIZE);

		int bytesRecieved = recv(clientSocket, buffer, BUFFER_SIZE, 0);
		if (bytesRecieved == SOCKET_ERROR) {
			std::cerr << "Error in revc()" << endl;
			break;
		}
		if (bytesRecieved == 0) {
			std::cout << "Client disconnected " << endl;
			break;
		}

		string serverMessage = "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: ";
		string responseBody = "{\"message\": \"Hello world\"}";

		serverMessage.append(std::to_string(responseBody.size()));
		serverMessage.append("\n\n");
		serverMessage.append(responseBody);

		int bytesSent = 0;
		int totalBytesSent = 0;
		while (totalBytesSent < serverMessage.size()) {
			bytesSent = send(clientSocket, serverMessage.c_str(), serverMessage.size(), 0);
			if (bytesSent < 0) {
				cerr << "Could not send response" << endl;
			}
			totalBytesSent += bytesSent;
		}
	}

	// CLOSE SOCKET
	closesocket(clientSocket);
	closesocket(listening);
	WSACleanup();
	return 0;
}