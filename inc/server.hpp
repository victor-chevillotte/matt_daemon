#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <poll.h>
#include <sys/socket.h> // socket()
#include <fcntl.h> //fcnt parameters
#include <netinet/in.h> // sockaddr_in
#include <utility> // std::make_pair
#include <unistd.h> // close
#include <arpa/inet.h> /* htons et inet_aton */
#include <stdexcept>
#include <netdb.h>
#include "../inc/tintin_reporter.hpp"
#include "../inc/utils.hpp"

#define BUFFER_SIZE 10000
#define MAX_CONNECTIONS_POOL 10

class Server;

class Server
{
	std::string					_host;
	std::string					_name;
	std::string					_port;
	int							_sock;
	std::vector<pollfd>			_pollfds;
    std::vector<int>            _fdToDelete;
	std::vector<pollfd> 	    _pollfdsToAdd;


public:

	typedef std::vector<pollfd>::iterator 					pollfds_iterator;

	Server();
	Server(const std::string& port);
	Server(const Server &src);
	~Server();

	static bool 		_running;


	Server& 			operator=(const Server &src);
	
	void 				start();
	int					newSocket();
	void				onClientConnect();
	void				onClientMessage(int fd);
	void				onClientDisconnect(int fd);
    void                readMessage(int fd);
	void				addConnectedClients();
	void                deleteDisconnectedClients();

};