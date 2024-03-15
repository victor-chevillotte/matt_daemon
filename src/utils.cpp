#include "../inc/utils.hpp"

void ftLog(const std::string& level, const std::string& message) {
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->logToFile(level, message);
}
