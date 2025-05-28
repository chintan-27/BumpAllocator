#include "../include/Logger.hpp"

bool Logger::enabled = true;
std::mutex Logger::mutex_;
