#include "../inc/tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter() {
    std::cout << "Tintin reporter created" << std::endl;
}

Tintin_reporter::~Tintin_reporter() {
    std::cout << "Tintin reporter destroyed" << std::endl;
}


std::string Tintin_reporter::_format_message(const std::string& message) {
    // Time format : DD / MM / YYYY - HH : MM : SS
}

void    Tintin_reporter::log_to_file(const std::string& message) {
    std::ofstream log_file(log_file_path, std::ios::app);
    if (log_file.is_open()) {
        log_file << _format_message(message) << std::endl;
        log_file.close();
    } else {
        std::cerr << "Failed to open log file" << std::endl;
    }
}