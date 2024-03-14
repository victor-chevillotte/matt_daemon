#include <iostream>
#include <fstream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <errno.h>
#include "../inc/Server.hpp"

//TODO: Replace with /var/lock/matt_daemon.lock
#define LOCK_FILE "/tmp/lock/matt_daemon.lock"

void ft_exit(int status) {
    
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("INFO", "Quitting.\n"); //not working
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
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("INFO", "Signal handler.\n");
    ft_exit(signum);
}

void set_signals() {
    const int SIGNALS[] = {SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2};
    for (int i = 0; i < int(sizeof(SIGNALS) / sizeof(SIGNALS[0])); i++) {
        signal(SIGNALS[i], signalHandler);
    }
}

bool is_file_existing(const std::string& name) {
    struct stat buffer;
    // check works even if file is a symlink or chmod 000
    // stat() is a function which is used to get the status of file. It fills the buffer pointed to by buffer with the status information of the file pointed to by name.
    return (stat (name.c_str(), &buffer) == 0); 
}


int main() {
    // TODO: Replace with 4242
    const int PORT = 4343;
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("INFO", "Started.\n"); 

    // Check if lock file exists
    if (is_file_existing(LOCK_FILE)) {
        std::cerr << "Error: File locked\n";
        reporter->log_to_file("ERROR", "Error File locked\n");
        return 1;
    }

    // create lock file
    std::ofstream newLockFile(LOCK_FILE);
    if (newLockFile.is_open()) {
        newLockFile.close();
    } else {
        std::cerr << "Can't open :" << LOCK_FILE << std::endl;
        reporter->log_to_file("ERROR", "Error File locked\n");
        return 1;
    }

    reporter->log_to_file("INFO", "Creating server\n");
    try {
        Server server = Server(std::to_string(PORT));
        reporter->log_to_file("INFO", "Server created\n");

        // Fork the process and quit the parent process, leaving child process as daemon
        reporter->log_to_file("INFO", "Entering daemon mode\n");
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

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        reporter->log_to_file("ERROR", "Server runtime error.\n");
        return 1;
    }
    
    return 0;
}