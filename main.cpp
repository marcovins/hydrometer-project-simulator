#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <termios.h>
#include <unistd.h>
#include "src/modules/simulator.hpp"
#include "src/utils/logger.hpp"

// Variável global para controlar finalização
Simulator* globalSimulator = nullptr;

// Handler para Ctrl+C
void signalHandler(int signal) {
    if (signal == SIGINT && globalSimulator != nullptr) {
        Logger::log(LogLevel::SHUTDOWN, "\n[INFO] Ctrl+C detectado - Finalizando...");
        globalSimulator->stop();
        
        // Restaura terminal
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= (ICANON | ECHO | ISIG);
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        
        exit(0);
    }
}

int main() {
    // Configura handler para Ctrl+C
    signal(SIGINT, signalHandler);
    
    // Ativa modo debug apenas no início
    Logger::setDebugMode(true);
    Logger::setRuntimeMode(false);
    
    Logger::log(LogLevel::STARTUP, "========================================");
    Logger::log(LogLevel::STARTUP, "[INFO] SIMULADOR DE HIDRÓMETRO INICIANDO");
    Logger::log(LogLevel::STARTUP, "========================================");
    Logger::log(LogLevel::STARTUP, "[INFO] Criando instância do simulador...");
    
    Simulator simulator;
    globalSimulator = &simulator; // Define ponteiro global para o handler
    
    Logger::log(LogLevel::STARTUP, "[INFO] Iniciando simulação...");
    simulator.run();
    
    Logger::log(LogLevel::STARTUP, "");
    Logger::log(LogLevel::STARTUP, "[INFO] ====== STATUS INICIAL ======");
    Logger::log(LogLevel::STARTUP, "[INFO] Hidrómetro status: Active");
    Logger::log(LogLevel::STARTUP, "[INFO] Contador inicial: 0 L");
    Logger::log(LogLevel::STARTUP, "[INFO] Simulação rodando: Sim");
    Logger::log(LogLevel::STARTUP, "[INFO] ==============================");
    Logger::log(LogLevel::STARTUP, "");
    Logger::log(LogLevel::STARTUP, "[INFO] === MODO MONITORAMENTO ===");
    Logger::log(LogLevel::STARTUP, "[INFO] Use as setas ↑↓←→ para ajustar vazão");
    Logger::log(LogLevel::STARTUP, "[INFO] Pressione ESC para sair");
    Logger::log(LogLevel::STARTUP, "");
    
    // Agora muda para modo runtime (apenas 4 linhas visíveis)
    Logger::setDebugMode(true);
    Logger::setRuntimeMode(true);
    Logger::clearRuntimeArea();
    
    // Loop de monitoramento - verifica se o simulador ainda está rodando
    while (simulator.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Volta para modo de finalização
    Logger::setRuntimeMode(false);
    Logger::setDebugMode(true);
    
    Logger::log(LogLevel::SHUTDOWN, "");
    Logger::log(LogLevel::SHUTDOWN, "[INFO] Finalizando simulação...");
    simulator.stop();
    Logger::log(LogLevel::SHUTDOWN, "[INFO] Simulação finalizada com sucesso!");
    Logger::log(LogLevel::SHUTDOWN, "========================================");

    return 0;
}