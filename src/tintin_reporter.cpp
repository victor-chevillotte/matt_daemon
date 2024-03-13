#include "../inc/tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter() {
    std::cout << "Tintin reporter created" << std::endl;
}

Tintin_reporter::~Tintin_reporter() {
    std::cout << "Tintin reporter destroyed" << std::endl;
}


std::string Tintin_reporter::format_message(const std::string& message) {
    // Time format : DD / MM / YYYY - HH : MM : SS
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y - %X", &tstruct);
    return "[" + std::string(buf) + "] " + message;
}

bool    Tintin_reporter::create_directory_if_not_exists(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        // directory exists
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        // directory does not exist, create it
        if (mkdir(path.c_str(), 0755) == -1) {
            std::cerr << "Error while creating log folder" << std::endl;
            return false;
        }
        return true;
    } else {
        // directory exists but could not ne opened
        struct stat info;
        if (stat(path.c_str(), &info) != 0) {
            std::cerr << "Error while accessing directory and checking its existence " << std::endl;
            return false;
        }
        if (S_ISDIR(info.st_mode)) {
            // file exists
            return true;
        }
        std::cerr << "The path exists, but it is not a directory." << std::endl;
        return false;
    }
}

void    Tintin_reporter::log_to_file(const std::string& message) {
     // create log file if does not exists 
    // write to log file with format [TIMESTAMP] message
    // /var/log/matt_daemon is existng directory ?
    bool directory_success = create_directory_if_not_exists("/home/vchevill/Documents/matt_daemon/log");
    if (!directory_success) {
       return;
    }
    std::ofstream logFile("/home/vchevill/Documents/matt_daemon/log/matt_daemon.log", std::ios::app);
    if (logFile.is_open()) {
        std::string formated_message = format_message(message);
        logFile << formated_message;
        logFile.close();
    } else {
        std::cerr << "Failed to open or create log file" << std::endl;
    }
}

Tintin_reporter* Tintin_reporter::reporter_instance= nullptr;;

Tintin_reporter *Tintin_reporter::GetInstance()
{
    if(reporter_instance == nullptr) {
        reporter_instance = new Tintin_reporter();
    }
    return reporter_instance;
}
