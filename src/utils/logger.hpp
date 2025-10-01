#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <iomanip>
#include <string>

enum class LogLevel {
    STARTUP,    // Logs de inicialização
    SHUTDOWN,   // Logs de finalização
    RUNTIME,    // Logs em tempo real (sempre visíveis)
    DEBUG       // Logs de debug (opcionais)
};

class Logger {
private:
    static bool showDebug;
    static bool runtimeStarted;

public:
    static void setDebugMode(bool enabled);
    static void setRuntimeMode(bool started);
    
    static void log(LogLevel level, const std::string& message);
    static void logRuntime(const std::string& status, float flowIN, float flowOUT, int newCounter, int pos);
    static void clearRuntimeArea();
};

#endif