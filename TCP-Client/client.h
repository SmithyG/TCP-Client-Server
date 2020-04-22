#pragma once

class Client
{
public:
	Client(int argc, char* argv[]);
	void Connect();
	void Send() const;
	void Receive() const;
	void Shutdown() const;

private:
	void InitiliseSocket(int argc, char* argv[]);

	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints{};
};