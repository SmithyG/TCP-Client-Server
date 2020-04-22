#pragma once

class Server
{
public:
	Server();

	void AcceptClient();
	void Receive() const;
	void Shutdown() const;

private:
	void InitiliseSocket();

	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;
};
