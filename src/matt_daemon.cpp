#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include<unistd.h>
#include <csignal>
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

bool createDirectoryIfNotExists(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        // Le dossier existe.
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        // Le dossier n'existe pas et doit être créé.
        if (mkdir(path.c_str(), 0755) == -1) {
            std::cerr << "Erreur lors de la création du dossier" << std::endl;
            return false;
        }
        return true;
    } else {
        // Le dossier existe mais ne peut pas être ouvert (peut-être en raison de chmod 000).
        struct stat info;
        if (stat(path.c_str(), &info) != 0) {
            std::cerr << "Impossible d'accéder au dossier et de vérifier son existence " << std::endl;
            return false;
        }
        if (S_ISDIR(info.st_mode)) {
            // Le dossier existe.
            return true;
        }
        std::cerr << "Le chemin existe, mais ce n'est pas un dossier." << std::endl;
        return false;
    }
}

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

void write_log(const std::string& message) {
    // create log file if does not exists 
    // write to log file with format [TIMESTAMP] message
    // /var/log/matt_daemon is existng directory ?
    bool directory_success = createDirectoryIfNotExists("log");
    if (!directory_success) {
        ft_exit(EXIT_FAILURE);
    }
    std::string message_with_timestamp = "[" + std::to_string(std::time(nullptr)) + "] " + message;
    std::ofstream logFile("/var/log/matt_daemon/matt_daemon.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    } else {
        std::cerr << "Failed to open or create log file" << std::endl;
        ft_exit(EXIT_FAILURE);
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
    std::cout << "Matt Daemon" << std::endl;
    write_log("Started");

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

    // TODO: Create the socket

    // TODO: Bind the socket

    std::cout << "Listening on port " << PORT << std::endl;

    // TODO: Log loop
    while (1) {
        sleep(1);
        
    }

    // delete lock file

    return 0;
}