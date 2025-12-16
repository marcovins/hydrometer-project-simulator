#include "hidrometer_manager.hpp"
#include "../utils/logger.hpp"
#include <algorithm>
#include <sstream>

HidrometerManager::HidrometerManager() : systemRunning(false) {
    Logger::log(LogLevel::STARTUP, "[HidrometerManager] Inicializado");
}

HidrometerManager::~HidrometerManager() {
    pararTodos();
    Logger::log(LogLevel::SHUTDOWN, "[HidrometerManager] Finalizado");
}

bool HidrometerManager::adicionarHidrometro(int idUsuario, const std::string& idSha) {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    // Verifica se usuário já tem o máximo de hidrômetros
    auto& hidrometros = hidrometrosPorUsuario[idUsuario];
    if (hidrometros.size() >= MAX_HIDROMETROS_POR_USUARIO) {
        std::ostringstream oss;
        oss << "[HidrometerManager] Usuario " << idUsuario 
            << " ja possui o maximo de " << MAX_HIDROMETROS_POR_USUARIO << " hidrometros";
        Logger::log(LogLevel::RUNTIME, oss.str());
        return false;
    }
    
    // Verifica se hidrômetro já existe para este usuário
    for (const auto& info : hidrometros) {
        if (info->idSha == idSha) {
            std::ostringstream oss;
            oss << "[HidrometerManager] Hidrometro " << idSha 
                << " ja existe para usuario " << idUsuario;
            Logger::log(LogLevel::RUNTIME, oss.str());
            return false;
        }
    }
    
    // Cria novo hidrômetro
    auto novoHidrometro = std::make_unique<HidrometerInfo>(idSha, idUsuario);
    
    std::ostringstream oss;
    oss << "[HidrometerManager] Hidrometro " << idSha 
        << " adicionado para usuario " << idUsuario;
    Logger::log(LogLevel::STARTUP, oss.str());
    
    // Se sistema já está rodando, inicia este hidrômetro
    if (systemRunning) {
        novoHidrometro->running = true;
        novoHidrometro->hidrometer->activate();
        novoHidrometro->workerThread = std::thread(&HidrometerManager::workerLoop, this, novoHidrometro.get());
    }
    
    hidrometros.push_back(std::move(novoHidrometro));
    return true;
}

bool HidrometerManager::removerHidrometro(int idUsuario, const std::string& idSha) {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    auto it = hidrometrosPorUsuario.find(idUsuario);
    if (it == hidrometrosPorUsuario.end()) {
        return false;
    }
    
    auto& hidrometros = it->second;
    for (auto hit = hidrometros.begin(); hit != hidrometros.end(); ++hit) {
        if ((*hit)->idSha == idSha) {
            // Para o hidrômetro completamente
            (*hit)->running = false;
            (*hit)->hidrometer->shutdown();
            
            if ((*hit)->workerThread.joinable()) {
                (*hit)->workerThread.join();
            }
            
            std::ostringstream oss;
            oss << "[HidrometerManager] Hidrometro " << idSha 
                << " removido do usuario " << idUsuario;
            Logger::log(LogLevel::SHUTDOWN, oss.str());
            
            hidrometros.erase(hit);
            return true;
        }
    }
    
    return false;
}

void HidrometerManager::removerTodosHidrometrosUsuario(int idUsuario) {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    auto it = hidrometrosPorUsuario.find(idUsuario);
    if (it == hidrometrosPorUsuario.end()) {
        return;
    }
    
    auto& hidrometros = it->second;
    for (auto& info : hidrometros) {
        info->running = false;
        info->hidrometer->shutdown();
        
        if (info->workerThread.joinable()) {
            info->workerThread.join();
        }
    }
    
    std::ostringstream oss;
    oss << "[HidrometerManager] Todos os " << hidrometros.size() 
        << " hidrometros do usuario " << idUsuario << " foram removidos";
    Logger::log(LogLevel::SHUTDOWN, oss.str());
    
    hidrometrosPorUsuario.erase(it);
}

void HidrometerManager::iniciarTodos() {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    systemRunning = true;
    int totalIniciados = 0;
    
    for (auto& [idUsuario, hidrometros] : hidrometrosPorUsuario) {
        for (auto& info : hidrometros) {
            if (!info->running) {
                info->running = true;
                info->hidrometer->activate();
                info->workerThread = std::thread(&HidrometerManager::workerLoop, this, info.get());
                totalIniciados++;
            }
        }
    }
    
    std::ostringstream oss;
    oss << "[HidrometerManager] " << totalIniciados << " hidrometros iniciados";
    Logger::log(LogLevel::STARTUP, oss.str());
}

void HidrometerManager::pararTodos() {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    systemRunning = false;
    int totalParados = 0;
    
    // Para todos os hidrômetros e suas threads internas
    for (auto& [idUsuario, hidrometros] : hidrometrosPorUsuario) {
        for (auto& info : hidrometros) {
            if (info->running) {
                info->running = false;
                info->hidrometer->shutdown();  // Para completamente (thread interna)
                totalParados++;
            }
        }
    }
    
    // Aguarda todas as threads do manager terminarem
    for (auto& [idUsuario, hidrometros] : hidrometrosPorUsuario) {
        for (auto& info : hidrometros) {
            if (info->workerThread.joinable()) {
                info->workerThread.join();
            }
        }
    }
    
    std::ostringstream oss;
    oss << "[HidrometerManager] " << totalParados << " hidrometros parados (threads finalizadas)";
    Logger::log(LogLevel::SHUTDOWN, oss.str());
}

std::vector<std::string> HidrometerManager::listarHidrometrosUsuario(int idUsuario) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(managerMutex));
    
    std::vector<std::string> lista;
    auto it = hidrometrosPorUsuario.find(idUsuario);
    
    if (it != hidrometrosPorUsuario.end()) {
        for (const auto& info : it->second) {
            lista.push_back(info->idSha);
        }
    }
    
    return lista;
}

int HidrometerManager::getTotalHidrometrosAtivos() const {
    int total = 0;
    for (const auto& [idUsuario, hidrometros] : hidrometrosPorUsuario) {
        total += hidrometros.size();
    }
    return total;
}

void HidrometerManager::exibirStatus() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(managerMutex));
    
    std::cout << "\n========== STATUS DOS HIDROMETROS ==========" << std::endl;
    std::cout << "Total de usuarios: " << hidrometrosPorUsuario.size() << std::endl;
    std::cout << "Total de hidrometros: " << getTotalHidrometrosAtivos() << std::endl;
    std::cout << "============================================\n" << std::endl;
    
    for (const auto& [idUsuario, hidrometros] : hidrometrosPorUsuario) {
        std::cout << "Usuario ID " << idUsuario << " (" << hidrometros.size() << " hidrometros):" << std::endl;
        
        for (const auto& info : hidrometros) {
            std::cout << "  - SHA: " << info->idSha 
                      << " | Status: " << (info->hidrometer->getStatus() ? "ATIVO" : "INATIVO")
                      << " | Leitura: " << info->hidrometer->getCounter() << " L" << std::endl;
        }
        std::cout << std::endl;
    }
}

void HidrometerManager::workerLoop(HidrometerInfo* info) {
    std::ostringstream oss;
    oss << "[Worker] Thread iniciada para hidrometro " << info->idSha 
        << " (Usuario " << info->idUsuario << ")";
    Logger::log(LogLevel::DEBUG, oss.str());
    
    while (info->running) {
        // A thread principal apenas monitora
        // O hidrômetro tem sua própria thread de atualização
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::ostringstream oss2;
    oss2 << "[Worker] Thread finalizada para hidrometro " << info->idSha;
    Logger::log(LogLevel::DEBUG, oss2.str());
}

int HidrometerManager::obterLeitura(int idUsuario, const std::string& idSha) const {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    auto it = hidrometrosPorUsuario.find(idUsuario);
    if (it == hidrometrosPorUsuario.end()) {
        return -1; // Usuário não encontrado
    }
    
    for (const auto& info : it->second) {
        if (info->idSha == idSha) {
            return info->hidrometer->getCounter();
        }
    }
    
    return -1; // Hidrômetro não encontrado
}
