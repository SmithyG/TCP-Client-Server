#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>

#include "client.h"


#include <iostream>
#include <stdexcept>
#include <string>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(int argc, char* argv[])
{
	auto client = Client(argc, argv);

	client.Connect();
	client.StartReceiveThread();

	while(true)
	{
		std::string msg;
		std::getline(std::cin, msg);
		client.Send(msg);
	}

	return 0;
}

Client::Client(int argc, char* argv[])
{
	InitiliseSocket(argc, argv);
}

void Client::InitiliseSocket(int argc, char* argv[])
{
	WSADATA wsaData;

	// Validate the parameters
	if (argc != 2)
	{
		throw std::logic_error("usage: " + std::string(argv[0]) + "server-name");
	}

	// Initialize Winsock
	auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult));
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		throw std::runtime_error("getaddrinfo failed with error: " + std::to_string(iResult));
	}
}

void Client::Connect()
{
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			WSACleanup();
			throw std::runtime_error("socket failed with error: " + WSAGetLastError());
		}

		// Connect to server.
		auto iResult = connect(ConnectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		WSACleanup();
		throw std::runtime_error("Unable to connect to server!\n");
	}

	std::cout << "Connected to server\n";
}

void Client::Send(const std::string message) const
{
	// Send an initial buffer
	const auto iResult = send(ConnectSocket, message.c_str(), static_cast<int>(strlen(message.c_str())), 0);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		WSACleanup();
		throw std::runtime_error("send failed with error: " + WSAGetLastError());
	}
	std::cout << "Bytes Sent: " << iResult << "\n";
}

void Client::Receive() const
{
	char recvbuf[DEFAULT_BUFLEN];
	const auto recvbuflen = DEFAULT_BUFLEN;
	// Receive until the peer closes the connection
	do
	{
		const auto result = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (result > 0)
		{
			printf("Bytes received: %d\n", result);
		}
		else if (result == 0)
		{
			printf("Connection closed\n");
			Shutdown();
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			Shutdown();
		}
	} while (result > 0);

	Shutdown();
}

void Client::Shutdown() const
{
	// shutdown the connection since no more data will be sent
	const auto iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		WSACleanup();
		throw std::runtime_error("shutdown failed with error: " + WSAGetLastError());
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}

void Client::StartReceiveThread()
{
	receiveThread = std::thread(&Client::Receive, this);
}
