#include "../inc/tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter() {
    // std::cout << "Tintin reporter created" << std::endl;
}

Tintin_reporter::~Tintin_reporter() {
    // std::cout << "Tintin reporter destroyed" << std::endl;
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

void    Tintin_reporter::log_to_file(const std::string& message) {
    (void)message;
}