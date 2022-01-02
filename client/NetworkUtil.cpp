#include "NetworkUtil.hpp"
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "CryptoUtil.hpp"
#pragma comment(lib, "ws2_32.lib")

using namespace std;

std::string SendSocketMessage(std::string msg)
{
	//cout << "sending message: " << msg << "\n";

	// communication key
	//char key[5] = { 'K', 'C', 'Q'};

	//msg = XOR(msg, key);

	//cout << "sending message encrypted:" << msg << ", size: " << msg.size() << "\n";

	string ipAddress = "localhost";
	int port = 8000;

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << "\n";
		return "-999";
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << "\n";
		WSACleanup();
		return "-999";
	}

	//cout << "socket created!" << "\n";

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << "\n";
		closesocket(sock);
		WSACleanup();
		return "-999";
	}

	//cout << "connected to server!" << "\n";

	// Do-while loop to send and receive data
	char buf[4096];

	// Send the text
	int sendResult = send(sock, msg.c_str(), msg.size(), 0);
	if (sendResult == SOCKET_ERROR)
	{
		cout << "send error" << "\n";
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
			//cout << string(buf, 0, iResult) << "\n";
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

std::string SendVaultRequest(std::string authkey) {
	return SendSocketMessage("vaultRequest|" + authkey);
}

std::string SendVaultDeleteRequest(std::string authkey) {
	return SendSocketMessage("deleteVault|" + authkey);
}

std::string SendVaultUpdateRequest(std::string authkey, std::string newdata, string vaultKey) {
	//char key[sizeof(vaultKey)];
	//strcpy_s(key, vaultKey.c_str());
	//newdata = XOR(newdata, key);
	return SendSocketMessage("updateVault|" + authkey + "|" + newdata);
}

std::string SendVaultCreateRequest(std::string authkey, std::string data, string vaultKey) {
	//char key[sizeof(vaultKey)];
	//strcpy_s(key, vaultKey.c_str());

	//// if we are creating an empty vault, no need to encrypt it
	//if (data.size() > 0)
	//	data = XOR(data, key);

	return SendSocketMessage("createVault|" + authkey + "|" + data);
}
