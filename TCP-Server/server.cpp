#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>

#include "server.h"


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(int argc, char* argv[])
{
	auto server = Server();

	server.AcceptClient();
	server.Receive();
	server.Shutdown();
}

Server::Server()
{
	InitiliseSocket();
}

void Server::InitiliseSocket()
{
	WSADATA wsaData;
	auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		throw std::runtime_error("WSAStartup failed with error: " + result);
	}

	struct addrinfo *addrInfoResult = NULL;
	struct addrinfo hints{};

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrInfoResult);

	if (result != 0)
	{
		WSACleanup();
		throw std::runtime_error("getaddrinfo failed with error: " + result);
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(addrInfoResult->ai_family, addrInfoResult->ai_socktype, addrInfoResult->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		freeaddrinfo(addrInfoResult);
		WSACleanup();
		throw std::runtime_error("socket failed with error: " + WSAGetLastError());
	}

	// Setup the TCP listening socket
	result = bind(listenSocket, addrInfoResult->ai_addr, static_cast<int>(addrInfoResult->ai_addrlen));
	if (result == SOCKET_ERROR)
	{
		freeaddrinfo(addrInfoResult);
		closesocket(listenSocket);
		WSACleanup();
		throw std::runtime_error("bind failed with error: " + WSAGetLastError());
	}

	freeaddrinfo(addrInfoResult);

	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		closesocket(listenSocket);
		WSACleanup();
		throw std::runtime_error("listen failed with error:" + WSAGetLastError());
	}
}

void Server::AcceptClient()
{
	// Accept a client socket
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		closesocket(listenSocket);
		WSACleanup();
		throw std::runtime_error("accept failed with error: " + WSAGetLastError());
	}

	// No longer need server socket
	closesocket(listenSocket);
}

void Server::Receive() const
{
	char recvbuf[DEFAULT_BUFLEN];
	const auto recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	int result = -1;
	do
	{
		result = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (result > 0)
		{
			std::cout << "Bytes received: " << result << std::endl;
			std::string recvData(recvbuf, result);
			std::cout << "Data received: " << recvData.c_str() << std::endl;

			// Echo the buffer back to the sender
			const auto sendResult = send(clientSocket, recvbuf, result, 0);
			if (sendResult == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				WSACleanup();
				throw std::runtime_error("send failed with error: " + WSAGetLastError());
			}

			std::cout << "Bytes sent: " << sendResult << std::endl;
		}
		else if (result == 0)
		{
			std::cout << "Connection closing...\n";
		}
		else
		{
			closesocket(clientSocket);
			WSACleanup();
			throw std::runtime_error("recv failed with error: " + WSAGetLastError());
		}
	}while (result > 0);
}

void Server::Shutdown() const
{
	// shutdown the connection since we're done
	const auto result = shutdown(clientSocket, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		closesocket(clientSocket);
		WSACleanup();
		throw std::runtime_error("shutdown failed with error: " + WSAGetLastError());
	}

	// cleanup
	closesocket(clientSocket);
	WSACleanup();
}
