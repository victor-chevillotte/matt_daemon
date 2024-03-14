#include "../inc/utils.hpp"

void ft_log(const std::string& level, const std::string& message) {
    Tintin_reporter* reporter = Tintin_reporter::GetInstance();
    reporter->log_to_file(level, message);
}
