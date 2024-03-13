#include <ctime>
#include <iostream>


class Tintin_reporter
{

protected:

    Tintin_reporter();
    static Tintin_reporter* reporter_instance;
    const std::string log_file_path;

public:

    Tintin_reporter(Tintin_reporter &other) = delete;
    void operator=(const Tintin_reporter &) = delete;

    static Tintin_reporter *GetInstance();


    ~Tintin_reporter();

    void log_to_file(const std::string& message);
    std::string format_message(const std::string& message);

};