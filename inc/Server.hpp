#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <sys/socket.h> // socket()
#include <fcntl.h> //fcnt parameters
#include <netinet/in.h> // sockaddr_in
#include <utility> // std::make_pair
#include <unistd.h> // close
#include <arpa/inet.h> /* htons et inet_aton */
#include <netdb.h>

#define BUFFER_SIZE 10000
#define MAX_CONNECTIONS 3

class Server;

class Server
{
	const std::string			_host;
	const std::string			_name;
	const std::string			_port;
	int							_sock;
	std::vector<pollfd>			_pollfds;
    std::vector<int>            _fdToDelete;

public:

	typedef std::vector<pollfd>::iterator 					pollfds_iterator;
	
	Server(const std::string& port);
	~Server();

	static bool 		_running;

	void 				start();
	int					newSocket();
	void				onClientConnect();
	void				onClientMessage(int fd);
	void				onClientDisconnect(int fd);
    void                readMessage(int fd);
    void                deleteClient(int fd);
	void                deleteDisconnectedClients();

};