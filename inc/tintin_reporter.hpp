#include <ctime>
#include <iostream>


class Tintin_reporter
{

    const std::string log_file_path;

public:

    Tintin_reporter();
    ~Tintin_reporter();

    void log_to_file(const std::string& message);
    std::string format_message(const std::string& message);

};