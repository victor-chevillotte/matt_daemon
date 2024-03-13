#include <iostream>
#include <fstream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <errno.h>
#include "../inc/Server.hpp"


void ft_exit(int status) {
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("Quitting.\n");
    Server::_running = false;
    remove("/var/lock/matt_daemon.lock");
    exit(status);
}

void signalHandler( int signum ) {
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    std::cout << "Signal (" << signum << ") received.\n";
    reporter->log_to_file("Signal handler.");
    ft_exit(signum);
}

void set_signals() {
    const int SIGNALS[] = {SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2};
    for (int i = 0; i < int(sizeof(SIGNALS) / sizeof(SIGNALS[0])); i++) {
        signal(SIGNALS[i], signalHandler);
    }
}

int main() {
    // TODO: Replace with 4242
    const int PORT = 4343;
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("Started.\n");    

    // create lock file
    std::ofstream newLockFile("/var/lock/matt_daemon.lock");
    if (newLockFile.is_open()) {
        newLockFile.close();
    } else {
        std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
        reporter->log_to_file("Error File locked\n");
        return 1;
    }

    reporter->log_to_file("Creating server.\n");
    reporter->log_to_file("Server created.\n");
    reporter->log_to_file("Entering daemon mode.\n");

    // Fork the process and quit the parent process, leaving child process as daemon
    pid_t c_pid = fork(); 
    if (c_pid == -1) { 
        perror("fork"); 
        exit(EXIT_FAILURE); 
    } 
    else if (c_pid > 0) { 
        std::cout << "Parent process is exiting" << std::endl;
        exit(EXIT_SUCCESS);
    } 
    reporter->log_to_file("started. PID: " + std::to_string(c_pid) + "\n");
    std::cout << "Daemon process is running" << std::endl; 
  
    // Catch all signals
    set_signals();

    // Create Server
    Server server = Server(std::to_string(PORT));
    server.start();
    return 0;
}