#pragma once
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>

class Tintin_reporter
{

protected:

    Tintin_reporter();
    static Tintin_reporter* reporter_instance;

public:

    Tintin_reporter(const Tintin_reporter &other);
    ~Tintin_reporter();

    static Tintin_reporter *GetInstance();

	Tintin_reporter& 			operator=(const Tintin_reporter &src);

    void logToFile(const std::string& log_level, const std::string& message);
    std::string formatMessage(const std::string& log_level, const std::string& message);
    bool createDirectoryIfNotExists(const std::string& path);

};