#include <iostream>
#include <fstream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <errno.h>
#include "../inc/Server.hpp"


void ft_exit(int status) {
    Server::_running = false;
    remove("/var/lock/matt_daemon.lock");
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

bool is_file_existing(const std::string& name) {
    struct stat buffer;
    // check works even if file is a symlink or chmod 000
    // stat() is a function which is used to get the status of file. It fills the buffer pointed to by buffer with the status information of the file pointed to by name.
    return (stat (name.c_str(), &buffer) == 0); 
}

int main() {
    // TODO: Replace with 4242
    const int PORT = 4343;
    std::cout << "Matt Daemon" << std::endl;
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file("Started");

    // Check if the lock file exists and quit if it does
    // Only one daemon instance should be able to run at once.
    // When attempting to run a second daemon whereas one instance is already running, an error message indicating a creation/file opening on matt_daemon.lock attempt must pop.
    // A matt_daemon.lock file must be created in /var/lock/ when the daemon starts.
        
    if (is_file_existing("/var/lock/matt_daemon.lock")) {
        std::cerr << "Lock file already exists" << std::endl;
        return 1;
    }

    // create lock file
    std::ofstream newLockFile("/var/lock/matt_daemon.lock");
    if (newLockFile.is_open()) {
        newLockFile.close();
    } else {
        std::cerr << "Failed to create lock file" << std::endl;
        return 1;
    }

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
    return 0;
}