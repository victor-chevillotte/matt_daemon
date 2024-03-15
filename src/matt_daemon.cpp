#include <iostream>
#include <fstream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include "../inc/server.hpp"
#include "../inc/utils.hpp"

#define LOCK_FILE "/var/lock/matt_daemon.lock"


void ftExit(int status) {

    ftLog("INFO", "Quitting.\n");
    try {
        std::remove(LOCK_FILE);
    } catch (const std::exception& e) {
        ftLog("ERROR", "Error removing lock file.\n");
        std::cerr << e.what() << std::endl;
    }
    Server::_running = false;
    exit(status);
}

void signalHandler( int signum ) {
    Server::_running = false;

    ftLog("INFO", "Signal handler.\n");
    ftExit(signum);
}

void setSignals() {
    const int SIGNALS[] = {SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGUSR1, SIGUSR2};
    for (int i = 0; i < int(sizeof(SIGNALS) / sizeof(SIGNALS[0])); i++) {
        signal(SIGNALS[i], signalHandler);
    }
}

bool isFileExisting(const std::string& name) {
    struct stat buffer;
    // check works even if file is a symlink or chmod 000
    // stat() is a function which is used to get the status of file
    return (stat (name.c_str(), &buffer) == 0); 
}


int main() {
    const int PORT = 4242;


    // Run as root
    if (getuid() != 0) {
        std::cerr << "You must be root to run this program." << std::endl;
        return 1;
    }
    setreuid(geteuid(), getuid());

    ftLog("INFO", "Started.\n");

    // Lock file
    int fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        std::cerr << "Can't open :" << LOCK_FILE << std::endl;
        ftLog("ERROR", "Can't open :" + std::string(LOCK_FILE) + "\n");
        return 1;
    }
    int rc = flock(fd, LOCK_EX | LOCK_NB); 
    if (rc == -1)
    {
        if (errno == EWOULDBLOCK) {
        std::cerr << "Can't open :" << LOCK_FILE << std::endl;
        ftLog("ERROR", "Error file locked.\n");
        }
        close(fd);
        return 1;
    }

    ftLog("INFO", "Creating server\n");
    try {
        Server server = Server(std::to_string(PORT));
        ftLog("INFO", "Server created\n");

        // Fork the process and quit the parent process, leaving child process as daemon
        ftLog("INFO", "Entering daemon mode\n");
        pid_t c_pid = fork(); 
        if (c_pid == -1) { 
            perror("fork"); 
            exit(EXIT_FAILURE); 
        } 
        else if (c_pid > 0) { 
            exit(EXIT_SUCCESS);
        }

        ftLog("INFO", "Started. PID: " + std::to_string(getpid()) + "\n");
    
        // Catch all signals
        setSignals();

        server.start();
        ftExit(0);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ftLog("ERROR", "Server runtime error.\n");
        ftExit(1);
    }
    
    return 0;
}