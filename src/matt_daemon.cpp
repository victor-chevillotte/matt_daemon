#include <iostream>
#include <fstream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <errno.h>
#include "../inc/Server.hpp"


void ft_exit(int status) {
    
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("INFO", "Quitting.\n"); //not working
    try {
        std::remove("/var/lock/matt_daemon.lock");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    Server::_running = false;
    exit(status);
}

void signalHandler( int signum ) {
    std::cout << "Signal (" << signum << ") received.\n";
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("INFO", "Signal handler.");
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
    reporter->log_to_file("INFO", "Started.\n");    

    // create lock file
    std::ofstream newLockFile("/var/lock/matt_daemon.lock");
    if (newLockFile.is_open()) {
        newLockFile.close();
    } else {
        std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
        reporter->log_to_file("ERROR", "Error File locked\n");
        return 1;
    }

    reporter->log_to_file("INFO", "Creating server\n");
    Server server = Server(std::to_string(PORT));
    reporter->log_to_file("INFO", "Server created\n");


    reporter->log_to_file("INFO", "Entering daemon mode\n");
    // Fork the process and quit the parent process, leaving child process as daemon
    pid_t c_pid = fork(); 
    if (c_pid == -1) { 
        perror("fork"); 
        exit(EXIT_FAILURE); 
    } 
    else if (c_pid > 0) { 
        exit(EXIT_SUCCESS);
    }
    reporter->log_to_file("INFO", "Started. PID: " + std::to_string(getpid()) + "\n");
  
    // Catch all signals
    set_signals();

    server.start();
    
    return 0;
}