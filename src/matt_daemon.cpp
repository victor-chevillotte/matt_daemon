#include <iostream>
#include<unistd.h>
#include <csignal>
#include "../inc/Server.hpp"


void ft_exit(int status) {
    exit(status);
}

void signalHandler( int signum ) {
    std::cout << "Signal (" << signum << ") received.\n";
    ft_exit(signum);
}

void set_signals() {
    const int SIGNALS[] = {SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2};
    for (int i = 0; i < int(sizeof(SIGNALS) / sizeof(SIGNALS[0])); i++) {
        signal(SIGNALS[i], signalHandler);
    }
}


int main() {
    const int PORT = 4242;
    std::cout << "Matt Daemon" << std::endl;

    // TODO: Check if the lock file exists and quit if it does

    // TODO: Write the lock file

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

    std::cout << "Daemon process is running" << std::endl; 
  
    // Catch all signals
    set_signals();

    // Create Server
    Server server = Server(std::to_string(PORT));
    server.start();

    std::cout << "Listening on port " << PORT << std::endl;
    return 0;
}