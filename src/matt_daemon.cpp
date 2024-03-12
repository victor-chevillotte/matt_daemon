#include <iostream>
#include <fstream>
#include <sys/stat.h>

bool fileExists(const std::string& name) {
    struct stat buffer;
    // check works even if file is a symlink or chmod 000
    // stat() is a function which is used to get the status of file. It fills the buffer pointed to by buffer with the status information of the file pointed to by name.
    return (stat (name.c_str(), &buffer) == 0); 
}

int main() {
    const int PORT = 4242;
    std::cout << "Matt Daemon" << std::endl;

    // Check if the lock file exists and quit if it does
    // Only one daemon instance should be able to run at once.
    // When attempting to run a second daemon whereas one instance is already running, an error message indicating a creation/file opening on matt_daemon.lock attempt must pop.
    // A matt_daemon.lock file must be created in /var/lock/ when the daemon starts.
        
    if (fileExists("/var/lock/matt_daemon.lock")) {
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

    // TODO: Fork the process and quit the parent process

    // TODO: Catch signals

    // TODO: Create the socket

    // TODO: Bind the socket

    std::cout << "Listening on port " << PORT << std::endl;

    // TODO: Log loop

    return 0;
}