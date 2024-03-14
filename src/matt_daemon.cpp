#include <iostream>
#include <fstream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <errno.h>
#include "../inc/Server.hpp"
#include "../inc/utils.hpp"

//TODO: Replace with /var/lock/matt_daemon.lock
#define LOCK_FILE "/tmp/lock/matt_daemon.lock"



void ft_exit(int status) {

    ft_log("INFO", "Quitting.\n");
    try {
        std::remove(LOCK_FILE);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    Server::_running = false;
    exit(status);
}

void signalHandler( int signum ) {
    Server::_running = false;

    ft_log("INFO", "Signal handler.\n");
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

    ft_log("INFO", "Started.\n");    

    // create lock file
    std::ofstream newLockFile(LOCK_FILE);
    if (newLockFile.is_open()) {
        newLockFile.close();
    } else {
        std::cerr << "Can't open :" << LOCK_FILE << std::endl;
        ft_log("ERROR", "Error File locked\n");
        return 1;
    }

    ft_log("INFO", "Creating server\n");
    try {
        Server server = Server(std::to_string(PORT));
        ft_log("INFO", "Server created\n");

        // Fork the process and quit the parent process, leaving child process as daemon
        ft_log("INFO", "Entering daemon mode\n");
        pid_t c_pid = fork(); 
        if (c_pid == -1) { 
            perror("fork"); 
            exit(EXIT_FAILURE); 
        } 
        else if (c_pid > 0) { 
            exit(EXIT_SUCCESS);
        }
        ft_log("INFO", "Started. PID: " + std::to_string(getpid()) + "\n");
    
        // Catch all signals
        set_signals();

        server.start();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ft_log("ERROR", "Server runtime error.\n");
        return 1;
    }
    
    return 0;
}