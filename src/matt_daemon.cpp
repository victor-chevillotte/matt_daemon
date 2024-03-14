#include <iostream>
#include <fstream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include "../inc/Server.hpp"
#include "../inc/utils.hpp"

//TODO: Replace with /var/lock/matt_daemon.lock
#define LOCK_FILE "/var/lock/matt_daemon.lock"



void ft_exit(int status) {

    ft_log("INFO", "Quitting.\n");
    try {
        std::remove(LOCK_FILE);
    } catch (const std::exception& e) {
        ft_log("ERROR", "Error removing lock file.\n");
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

bool is_file_existing(const std::string& name) {
    struct stat buffer;
    // check works even if file is a symlink or chmod 000
    // stat() is a function which is used to get the status of file. It fills the buffer pointed to by buffer with the status information of the file pointed to by name.
    return (stat (name.c_str(), &buffer) == 0); 
}


int main() {
    const int PORT = 4242;

    ft_log("INFO", "Started.\n");    

    // Run as root
    if (getuid() != 0) {
        std::cerr << "You must be root to run this program." << std::endl;
        ft_log("ERROR", "You must be root to run this program.\n");
        return 1;
    }
    setreuid(geteuid(), getuid());

    // Lock file
    ft_log("DEBUG", "Opening file lock.\n");
    int fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        std::cerr << "Can't open :" << LOCK_FILE << std::endl;
        ft_log("ERROR", "Can't open :" + std::string(LOCK_FILE) + "\n");
        return 1;
    }
    int rc = flock(fd, LOCK_EX | LOCK_NB); 
    if (rc == -1 && errno == EWOULDBLOCK)
    {
        std::cerr << "Lock file already exists." << std::endl;
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
        ft_exit(0);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ft_log("ERROR", "Server runtime error.\n");
        ft_exit(1);
    }
    
    return 0;
}