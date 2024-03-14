#include "../inc/Server.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>

// default constructor
Server::Server()  : _host("127.0.0.1"), _name("matt_daemon"), _port("4242"), _sock(-1) {
    _sock = newSocket();
}

// main constructor
Server::Server(const std::string& port)
    : _host("127.0.0.1"), _name("matt_daemon"), _port(port), _sock(-1) {
    _sock = newSocket();
}

// destructor
Server::~Server() {
    if (_sock != -1) {
        close(_sock);
    }
}

// copy constructor
Server::Server(const Server& other) :
    _host(other._host), _name(other._name), _port(other._port), _sock(other._sock), _pollfds(other._pollfds), _fdToDelete(other._fdToDelete), _pollfdsToAdd(other._pollfdsToAdd) {
}

// assignment operator
Server&	Server::operator=(const Server& rhs) {
    _host = rhs._host;
    _name = rhs._name;
    _port = rhs._port;
    _sock = rhs._sock;
    _pollfds = rhs._pollfds;
    _fdToDelete = rhs._fdToDelete;
    _pollfdsToAdd = rhs._pollfdsToAdd;
	return *this ;
}

//methods

void Server::start() {
    if (_sock == -1) {
        throw std::runtime_error("Socket not initialized.");
    }

    pollfd server_fd = {_sock, POLLIN, 0}; // POLLHUP & POLLERR are automatically checked
    _pollfds.push_back(server_fd);

    while (_running) {
        if (poll(_pollfds.data(), _pollfds.size(), -1) < 0) {
            if (_running) {
                throw std::runtime_error("Error while polling from fd.");
            }
        }

        for (pollfds_iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
            if (it->revents & POLLHUP) {
                onClientDisconnect(it->fd);
            } else if (it->revents & POLLIN) {
                if (it->fd == _sock) {
                    onClientConnect();
                } else {
                    onClientMessage(it->fd);
                }
            } else if (it->revents & POLLERR) {
                if (it->fd == _sock) {
                    _running = false;
                } else {
                    onClientDisconnect(it->fd);
                }
            }
        }
        addConnectedClients();
        deleteDisconnectedClients();
    }
    // Close all sockets before exiting
    for (pollfds_iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
        close(it->fd);
    }
    close(_sock);
}

int Server::newSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Error while opening socket.");
    }

    int val = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        close(sockfd);
        throw std::runtime_error("Error while setting socket options.");
    }

    struct sockaddr_in serv_address;
    std::memset(&serv_address, 0, sizeof(serv_address));
    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_address.sin_port = htons(std::stoi(_port));

    if (bind(sockfd, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0) {
        close(sockfd);
        throw std::runtime_error("Error while setting socket IP address and port.");
    }

    if (listen(sockfd, MAX_CONNECTIONS_POOL) < 0) {
        close(sockfd);
        throw std::runtime_error("Error while listening on socket.");
    }

    return sockfd;
}

void Server::onClientConnect() {
    sockaddr_in s_address;
    socklen_t s_size = sizeof(s_address);
    int fd = accept(_sock, (sockaddr *)&s_address, &s_size);
    if (fd < 0) {
        std::cerr << "Error while accepting new client." << std::endl;
        return;
    }
    if (_pollfds.size() > 3) {
        std::cout << "Max active connections reached." << std::endl;
        close(fd);
        return;
    }

    pollfd pollfd = {fd, POLLIN | POLLOUT, 0};
    _pollfdsToAdd.push_back(pollfd);

    std::cout << "Client connected" << std::endl;
}

void Server::onClientDisconnect(int fd) {
    _fdToDelete.push_back(fd);
    std::cout << "Client has disconnected" << std::endl;
}

void Server::onClientMessage(int fd) {
    readMessage(fd);
}

void Server::readMessage(int fd) {
    char buffer[BUFFER_SIZE + 1];
    ssize_t read_bytes;
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();

    read_bytes = recv(fd, buffer, BUFFER_SIZE, 0);
    if (read_bytes < 0) {
        _running = false;
    } else if (read_bytes == 0) {
        onClientDisconnect(fd);
    } else {
        buffer[read_bytes] = '\0';
        if (std::string(buffer) == "quit\n") {
            Tintin_reporter* reporter = Tintin_reporter::GetInstance();
            reporter->log_to_file("INFO", "Request quit.\n");
            _running = false;
            return;
        }
        std::cout << "Message received from client " << fd << ": " << buffer << std::endl;
        std::string message = "User input: " + std::string(buffer);
        reporter->log_to_file("LOG", message);
    }
}


void Server::addConnectedClients() {
    for (auto it = _pollfdsToAdd.begin(); it != _pollfdsToAdd.end(); ++it) {
        _pollfds.push_back(*it);
    }
    _pollfdsToAdd.clear();
}


void Server::deleteDisconnectedClients() {
    for (auto it = _fdToDelete.begin(); it != _fdToDelete.end(); ++it) {
        int fd = *it;
        for (auto it_poll = _pollfds.begin(); it_poll != _pollfds.end(); ++it_poll) {
            if (it_poll->fd == fd) {
                close(fd);
                _pollfds.erase(it_poll);
                break;
            }
        }
    }
    _fdToDelete.clear();
}

bool Server::_running = true;
