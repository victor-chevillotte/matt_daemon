#include "../inc/Server.hpp"
#include <cstring>

Server::Server(const std::string port)
	: _host("127.0.0.1"), _name("matt_daemon"), _port(port)
{
	_sock = newSocket();
}

Server::~Server()
{
}

void Server::start()
{
	pollfd server_fd = {_sock, POLLIN, 0}; // POLLHUP & POLLERR are automatically checked
	_pollfds.push_back(server_fd);

	while (Server::running)
	{
		if (poll(_pollfds.begin().base(), _pollfds.size(), -1) < 0)
			if (Server::running)
				throw std::runtime_error("Error while polling from fd.");
		for (pollfds_iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
		{

			if (it->revents & POLLHUP)
			{
				onClientDisconnect(it->fd);
				break;
			}

            // POLLIN
			if (it->revents & POLLIN)
			{
                // POLLIN for Server
				if (it->fd == _sock)
				{
					onClientConnect();
					break;
				}
                // POLLIN for Clients
				else {
					onClientMessage(it->fd);

               }
            }
			// POLLOUT for Clients
			if ((it->fd != _sock) && (it->revents & POLLOUT))
			{
				// No need to send messages to clients
			}

			// POLLERR for Clients
			if ((it->fd != _sock) && (it->revents & POLLERR))
			{
				onClientDisconnect(it->fd);
			}

			//server POLLERR -> Server::running = false
			if ((it->fd == _sock) && (it->revents & POLLERR))
			{
				Server::running = false;
			}

		}
		for (std::vector<int>::iterator it = _fdToDelete.begin(); it != _fdToDelete.end(); ++it)
		{
			deleteClient(*it);
			std::cout << "Client deleted" << std::endl;
		}
		_fdToDelete.clear();
	}
	for (pollfds_iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
	{
        pollfd pfd = *it;
        std::cout << "Closing client socket: " << pfd.fd << std::endl;
		close(pfd.fd);
	}
    _pollfds.clear();
	close(_sock);
	std::cout << "Server has been turned down. Goodbye !" << std::endl;
}

int Server::newSocket()
{

	/* creating socket :
	 * domain : AF_INET -> Socket using IPV4
	 * type : SOCK_STREAM : Dialogue support guaranteeing integrity, providing a binary data stream, and integrating a mechanism for out-of-band data transmissions.
	 * protocol : 0 indicates that the caller does not want to specify the protocol and will leave it up to the service provider.
	 */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw std::runtime_error("Error while opening socket.");

	/*Adding option to socket :
	 * Socket Layer : SOL_SOCKET : Means we modify the socket itslef
	 * option : SO_REUSEADDR : Forcefully attaching socket to the port
	 * value : 1 for forcing socket to use port given
	 */
	int val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
		throw std::runtime_error("Error while setting socket options.");

	/* Setting the socket to NON-BLOCKING mode allowing it to return any data that the system has in it's read buffer
	 * for that socket even if the fd is still in use. It won't wait for that data to be terminated and will send an error.
	 * command : F_SETFL : setting for state attribute of fd
	 * arg : O_NONBLOCK meaning described previously
	 */


// Clear address structure
sockaddr_in serv_address = {}; // Define and initialize serv_address

// Bind the socket to the IP address and port
if (bind(sockfd, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0)
    throw std::runtime_error("Error while setting socket IP address and port.");

	// Define max connexions and let socket be able to listen for requests
	if (listen(sockfd, MAX_CONNECTIONS) < 0)
		throw std::runtime_error("Error while listening on socket.");

	return sockfd;
}

void Server::onClientConnect()
{

	// adding new fd to poll
	int fd;
	sockaddr_in s_address = {};
	socklen_t s_size = sizeof(s_address);

	fd = accept(_sock, (sockaddr *)&s_address, &s_size);
	if (fd < 0)
		throw std::runtime_error("Error while accepting new client.");

	pollfd pollfd = {fd, POLLIN | POLLOUT, 0};
	_pollfds.push_back(pollfd);

	char hostname[NI_MAXHOST];
	if (getnameinfo((struct sockaddr *)&s_address, sizeof(s_address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
		throw std::runtime_error("Error while getting hostname of new client.");

	// Checks hostname size limit of 63 chars
	std::string host(hostname);
	if (host.length() > 63)
		host = inet_ntoa(s_address.sin_addr);

	std::cout << "Client connnected" << std::endl;
}

void Server::onClientDisconnect(int fd)
{
	// removing fd of leaving client from poll
	_fdToDelete.push_back(fd);
	std::cout << "Client has disconnected" << std::endl;
}

void Server::onClientMessage(int fd)
{
	readMessage(fd);
}

void Server::readMessage(int fd)
{
    int read_bytes = -10;
	char buffer[BUFFER_SIZE + 1];

	std::memset(buffer, 0, BUFFER_SIZE + 1);
	while (read_bytes != 0)
	{
		bzero(buffer, BUFFER_SIZE);
		read_bytes = recv(fd, buffer, BUFFER_SIZE, 0);
		if (read_bytes < 0)
			break;
		buffer[read_bytes] = '\0';
        std::cout << "Message received from client " << fd << ": " << buffer << std::endl;
	}
}

void Server::deleteClient(int fd)
{
    for (pollfds_iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollfds.erase(it);
			close(fd);
			break;
		}
	}
}

bool Server::running = true;