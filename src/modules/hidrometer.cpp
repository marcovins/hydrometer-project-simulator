#include "hidrometer.hpp"
#include "../utils/logger.hpp"
#include <iostream>
#include <iomanip>

Hidrometer::Hidrometer(float diameterIN,
           float lengthIN,
           float roughnessIN,
           float diameterOUT,
           float lengthOUT,
           float roughnessOUT)
    : pipeIN(std::make_unique<Pipe>(diameterIN, lengthIN, roughnessIN)),
      pipeOUT(std::make_unique<Pipe>(diameterOUT, lengthOUT, roughnessOUT))
{
    Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::Constructor - Iniciando construção do hidrómetro");
    Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::Constructor - Pipe IN: D=" + std::to_string(diameterIN) + "m, L=" + std::to_string(lengthIN) + "m, R=" + std::to_string(roughnessIN) + "m");
    Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::Constructor - Pipe OUT: D=" + std::to_string(diameterOUT) + "m, L=" + std::to_string(lengthOUT) + "m, R=" + std::to_string(roughnessOUT) + "m");

    this->status.store(false);
    this->counter.store(0);
    this->counterFloat = 0.0f;
    this->running.store(true);
    
    Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::Constructor - Status inicial: Inactive");
    Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::Constructor - Contador inicial: 0");
    Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::Constructor - Iniciando thread de atualização...");
    
    this->update_thread = std::thread([this]() {
        Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::UpdateThread - Thread de atualização iniciada");
        while (running.load()) {
            this->update();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Logger::log(LogLevel::SHUTDOWN, "[DEBUG] Hidrometer::UpdateThread - Thread de atualização finalizada");
    });
    
    Logger::log(LogLevel::DEBUG, "[DEBUG] Hidrometer::Constructor - Construção concluída com sucesso");
}

Hidrometer::~Hidrometer() {
    Logger::log(LogLevel::SHUTDOWN, "[DEBUG] Hidrometer::Destructor - Finalizando hidrómetro...");
    running.store(false);
    
    if (update_thread.joinable()) {
        update_thread.join();
        Logger::log(LogLevel::SHUTDOWN, "[DEBUG] Hidrometer::Destructor - Thread finalizada com sucesso");
    }
}

Pipe* Hidrometer::getPipeIN() const { return this->pipeIN.get(); }
Pipe* Hidrometer::getPipeOUT() const { return this->pipeOUT.get(); }
int Hidrometer::getCounter() const { return this->counter.load(); }
bool Hidrometer::getStatus() const { return this->status.load(); }

void Hidrometer::activate() { 
    Logger::log(LogLevel::STARTUP, "[DEBUG] Hidrometer::activate - Ativando hidrómetro");
    this->status.store(true);
    Logger::log(LogLevel::STARTUP, "[DEBUG] Hidrometer::activate - Status atual: Active");
}

void Hidrometer::deactivate() { 
    Logger::log(LogLevel::SHUTDOWN, "[DEBUG] Hidrometer::deactivate - Desativando hidrómetro");
    this->status.store(false);
}

void Hidrometer::update() {
    static int updateCount = 0;
    updateCount++;
            
    if (this->status.load()) {
        float flowIN = this->pipeIN->getFlowRate();
        float flowOUT = flowIN * 0.9f; // simula perda de 10%
        this->pipeOUT->setFlowRate(flowOUT);

        // Incrementa contador de forma mais realista
        // Intervalo de atualização: 0.1s
        // Conversão: m³/s * 0.1s = m³ por intervalo
        // Depois convertemos para litros: m³ * 1000 = L
        float volumeIncrement = flowOUT * 0.1f; // m³ por intervalo de 0.1s
        this->counterFloat += volumeIncrement * 1000.0f; // Converte m³ para litros
        this->counter.store(static_cast<int>(this->counterFloat));
        int newCounter = this->counter.load();
                    
    } else {
        this->pipeOUT->setFlowRate(0.0f);
    }
}
