#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

#ifdef NO_LOG
#define LOG(LEVEL)
#else
#define LOG(LEVEL)  Logger().GetStream(__FILE__, __LINE__) << #LEVEL << ": "
#endif

// Toy logger. Not a focus of this project.
class Logger {
public:
    Logger(): os_{std::cout} {}
    ~Logger() {
        os_ << '\n';
    }
    std::ostream& GetStream(std::string file, int line) { 
        os_ << "[" << file << ":" << line << "] ";
        return os_; 
    }
private:
    std::ostream& os_;
};

#endif // LOGGER_H