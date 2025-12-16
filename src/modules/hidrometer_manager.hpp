#ifndef HIDROMETER_MANAGER_HPP
#define HIDROMETER_MANAGER_HPP

#include "hidrometer.hpp"
#include "../usuarios/domain/usuario.hpp"
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include <string>

#define MAX_HIDROMETROS_POR_USUARIO 5

// Estrutura para armazenar informações de um hidrômetro
struct HidrometerInfo {
    std::string idSha;
    int idUsuario;
    std::unique_ptr<Hidrometer> hidrometer;
    std::thread workerThread;
    std::atomic<bool> running;
    
    HidrometerInfo(const std::string& sha, int userId)
        : idSha(sha), idUsuario(userId), running(false) {
        hidrometer = std::make_unique<Hidrometer>();
    }
    
    // Impede cópia
    HidrometerInfo(const HidrometerInfo&) = delete;
    HidrometerInfo& operator=(const HidrometerInfo&) = delete;
    
    // Permite movimentação
    HidrometerInfo(HidrometerInfo&&) = default;
    HidrometerInfo& operator=(HidrometerInfo&&) = default;
};

// Gerenciador de hidrômetros por usuário
class HidrometerManager {
private:
    // Mapa: idUsuario -> lista de HidrometerInfo
    std::map<int, std::vector<std::unique_ptr<HidrometerInfo>>> hidrometrosPorUsuario;
    mutable std::mutex managerMutex;
    std::atomic<bool> systemRunning;

public:
    HidrometerManager();
    ~HidrometerManager();
    
    // Adiciona hidrômetro para um usuário
    bool adicionarHidrometro(int idUsuario, const std::string& idSha);
    
    // Remove hidrômetro específico
    bool removerHidrometro(int idUsuario, const std::string& idSha);
    
    // Remove todos os hidrômetros de um usuário
    void removerTodosHidrometrosUsuario(int idUsuario);
    
    // Inicia todos os hidrômetros
    void iniciarTodos();
    
    // Para todos os hidrômetros
    void pararTodos();
    
    // Obtém lista de hidrômetros de um usuário
    std::vector<std::string> listarHidrometrosUsuario(int idUsuario) const;
    
    // Obtém a leitura atual de um hidrômetro específico
    int obterLeitura(int idUsuario, const std::string& idSha) const;
    
    // Obtém total de hidrômetros ativos
    int getTotalHidrometrosAtivos() const;
    
    // Exibe o status de todos os hidrômetros
    void exibirStatus() const;

private:
    // Thread de trabalho para cada hidrômetro
    void workerLoop(HidrometerInfo* info);
};

#endif // HIDROMETER_MANAGER_HPP
