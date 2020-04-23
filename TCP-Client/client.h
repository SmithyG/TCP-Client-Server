#pragma once
#include <string>
#include <thread>

class Client
{
public:
	Client(int argc, char* argv[]);
	void Connect();
	void Send(std::string message) const;
	void Shutdown() const;
	void StartReceiveThread();

private:
	void InitiliseSocket(int argc, char* argv[]);
	void Receive() const;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints{};

	std::thread receiveThread;
	std::thread sendThread;
};
