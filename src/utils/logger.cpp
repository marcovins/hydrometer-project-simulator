#include "logger.hpp"

bool Logger::showDebug = false;
bool Logger::runtimeStarted = false;

void Logger::setDebugMode(bool enabled) {
    showDebug = enabled;
}

void Logger::setRuntimeMode(bool started) {
    runtimeStarted = started;
}

void Logger::log(LogLevel level, const std::string& message) {
    switch (level) {
        case LogLevel::STARTUP:
        case LogLevel::SHUTDOWN:
            std::cout << message << std::endl;
            break;
            
        case LogLevel::RUNTIME:
            if (runtimeStarted) {
                std::cout << message << std::endl;
            }
            break;
            
        case LogLevel::DEBUG:
            if (showDebug && !runtimeStarted) {
                std::cout << message << std::endl;
            }
            break;
    }
}

void Logger::logRuntime(const std::string& status, float flowIN, float flowOUT, int newCounter, int pos) {
    if (!runtimeStarted) return;
    
    // Converte m³/s para m³/h para melhor visualização
    float flowIN_m3h = flowIN * 3600.0f;  // m³/s * 3600s/h
    float flowOUT_m3h = flowOUT * 3600.0f;
    
    // Converte contador de litros para m³
    float counter_m3 = newCounter / 1000.0f; // L para m³
    
    // Limpa completamente a tela antes de desenhar
    std::cout << "\033[2J\033[H"; // Clear screen + move to top
    
    // Exibe as 4 linhas sempre atualizadas
    std::cout << "┌─────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Hidrometro "<< pos << ":" << std::setw(10) << status 
              << " │ Contador: " << std::fixed << std::setprecision(3) << std::setw(8) << counter_m3 << " m³              │" << std::endl;
    std::cout << "│ Vazão IN: " << std::fixed << std::setprecision(2) << std::setw(8) << flowIN_m3h 
              << " m³/h │ Vazão OUT: " << std::setw(8) << flowOUT_m3h << " m³/h      │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────┘" << std::endl;
    std::cout << std::flush;
}

void Logger::clearRuntimeArea() {
    // Limpa a tela completamente
    std::cout << "\033[2J\033[H" << std::flush;
}