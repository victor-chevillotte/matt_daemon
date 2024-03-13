#include <ctime>
#include <iostream>


class Tintin_reporter
{

    const std::string log_file_path;

    Tintin_reporter();
    ~Tintin_reporter();

public:


        void log_to_file(const std::string& message);
        std::string _format_message(const std::string& message);

};