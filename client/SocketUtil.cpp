#include "SocketUtil.hh"
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

std::string SendSocketMessage(std::string msg)
{
	string ipAddress = "localhost";
	int port = 8000;

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return "-999";
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return "-999";
	}

	//cout << "socket created!" << endl;

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return "-999";
	}

	//cout << "connected to server!" << endl;

	// Do-while loop to send and receive data
	char buf[4096];

	// Send the text
	int sendResult = send(sock, msg.c_str(), msg.size() + 1, 0);
	if (sendResult == SOCKET_ERROR)
	{
		cout << "send error" << endl;
		return "-999";
	}

	int iResult;

	ZeroMemory(buf, 4096);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return "-999";
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(sock, buf, 4096, 0);
		if (iResult > 0)
		{
			//cout << string(buf, 0, iResult) << endl;
			return string(buf, 0, iResult);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();

	return "-999";
}