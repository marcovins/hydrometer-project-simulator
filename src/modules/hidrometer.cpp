#include "hidrometer.h"
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
        std::cout << "[DEBUG] Hidrometer::Constructor - Iniciando construção do hidrómetro" << std::endl;
        std::cout << "[DEBUG] Hidrometer::Constructor - Pipe IN: D=" << diameterIN << "m, L=" << lengthIN << "m, R=" << roughnessIN << "m" << std::endl;
        std::cout << "[DEBUG] Hidrometer::Constructor - Pipe OUT: D=" << diameterOUT << "m, L=" << lengthOUT << "m, R=" << roughnessOUT << "m" << std::endl;
        
        this->status.store(false);
        this->counter.store(0);
        this->counterFloat = 0.0f;
        this->running.store(true);
        
        std::cout << "[DEBUG] Hidrometer::Constructor - Status inicial: " << (this->status.load() ? "Active" : "Inactive") << std::endl;
        std::cout << "[DEBUG] Hidrometer::Constructor - Contador inicial: " << this->counter.load() << std::endl;
        std::cout << "[DEBUG] Hidrometer::Constructor - Iniciando thread de atualização..." << std::endl;
        
        this->update_thread = std::thread([this]() {
            std::cout << "[DEBUG] Hidrometer::UpdateThread - Thread de atualização iniciada" << std::endl;
            while (running.load()) {
                this->update();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            std::cout << "[DEBUG] Hidrometer::UpdateThread - Thread de atualização finalizada" << std::endl;
        });
        
        std::cout << "[DEBUG] Hidrometer::Constructor - Construção concluída com sucesso" << std::endl;
    }

    Hidrometer::~Hidrometer() {
        std::cout << "[DEBUG] Hidrometer::Destructor - Iniciando destruição do hidrómetro" << std::endl;
        running.store(false);
        std::cout << "[DEBUG] Hidrometer::Destructor - Thread marcada para finalização" << std::endl;
        
        if (update_thread.joinable()) {
            std::cout << "[DEBUG] Hidrometer::Destructor - Aguardando finalização da thread..." << std::endl;
            update_thread.join();
            std::cout << "[DEBUG] Hidrometer::Destructor - Thread finalizada com sucesso" << std::endl;
        }
        std::cout << "[DEBUG] Hidrometer::Destructor - Destruição concluída" << std::endl;
    }

    Pipe* Hidrometer::getPipeIN() const { return this->pipeIN.get(); }
    Pipe* Hidrometer::getPipeOUT() const { return this->pipeOUT.get(); }
    int Hidrometer::getCounter() const { return this->counter.load(); }
    bool Hidrometer::getStatus() const { return this->status.load(); }

    void Hidrometer::activate() { 
        std::cout << "[DEBUG] Hidrometer::activate - Ativando hidrómetro" << std::endl;
        this->status.store(true); 
        std::cout << "[DEBUG] Hidrometer::activate - Status atual: " << (this->status.load() ? "Active" : "Inactive") << std::endl;
    }
    
    void Hidrometer::deactivate() { 
        std::cout << "[DEBUG] Hidrometer::deactivate - Desativando hidrómetro" << std::endl;
        this->status.store(false); 
        std::cout << "[DEBUG] Hidrometer::deactivate - Status atual: " << (this->status.load() ? "Active" : "Inactive") << std::endl;
    }

    void Hidrometer::update() {
        static int updateCount = 0;
        updateCount++;
        
        // Log a cada 50 atualizações (5 segundos)
        bool shouldLog = (updateCount % 50 == 0);
        
        if (this->status.load()) {
            float flowIN = this->pipeIN->getFlowRate();
            float flowOUT = flowIN * 0.9f; // simula perda de 10%
            this->pipeOUT->setFlowRate(flowOUT);

            // Incrementa contador: m³/s * 0.1s * 2L/m³ = Litros por intervalo
            int oldCounter = this->counter.load();
            float volumeIncrement = flowOUT * 0.1f * 2.0f; // m³/s * 0.1s * 2L/m³
            this->counterFloat += volumeIncrement;
            this->counter.store(static_cast<int>(this->counterFloat));
            int newCounter = this->counter.load();
            
            // Log detalhado do cálculo quando necessário
            if (shouldLog || volumeIncrement > 0.01f) { // Log se há incremento significativo
                std::cout << "[DEBUG] Hidrometer::update - Cálculo volume: " 
                          << std::fixed << std::setprecision(6) << flowOUT << " m³/s * 0.1s * 1000 = " 
                          << std::setprecision(3) << volumeIncrement << " L/intervalo" << std::endl;
                std::cout << "[DEBUG] Hidrometer::update - Counter float: " 
                          << std::setprecision(3) << this->counterFloat << " L" << std::endl;
            }
            
            if (shouldLog) {
                std::cout << "[DEBUG] Hidrometer::update - Status: ATIVO" << std::endl;
                std::cout << "[DEBUG] Hidrometer::update - Vazão IN: " << std::fixed << std::setprecision(6) << flowIN << " m³/s" << std::endl;
                std::cout << "[DEBUG] Hidrometer::update - Vazão OUT: " << std::fixed << std::setprecision(6) << flowOUT << " m³/s (perda de 10%)" << std::endl;
                std::cout << "[DEBUG] Hidrometer::update - Contador: " << oldCounter << " -> " << newCounter << " L" << std::endl;
            }
        } else {
            this->pipeOUT->setFlowRate(0.0f);
            if (shouldLog) {
                std::cout << "[DEBUG] Hidrometer::update - Status: INATIVO - Vazão OUT zerada" << std::endl;
            }
        }
    }
