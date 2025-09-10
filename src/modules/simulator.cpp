#include "simulator.h"
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>

    void Simulator::throwFlow() const{
        std::cout << "[DEBUG] Simulator::throwFlow - Iniciando geração de vazão variável" << std::endl;
        float maxFlow = this->hidrometer->getPipeIN()->getMaxFlow();
        std::cout << "[DEBUG] Simulator::throwFlow - Vazão máxima do pipe: " << std::fixed << std::setprecision(6) << maxFlow << " m³/s" << std::endl;
        
        int iteration = 0;
        while (this->running.load()) {
            iteration++;
            // Simula a variação da vazão no pipe de entrada
            float newFlowRate = static_cast<float>(rand() % 100) / 100.0f * maxFlow; // Vazão entre 0.0 e maxFlow m^3/s
            this->hidrometer->getPipeIN()->setFlowRate(newFlowRate);
            
            std::cout << "[DEBUG] Simulator::throwFlow - Iteração " << iteration 
                      << ": Nova vazão = " << std::fixed << std::setprecision(6) << newFlowRate 
                      << " m³/s (" << std::fixed << std::setprecision(2) << (newFlowRate/maxFlow*100) << "% do máximo)" << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "[DEBUG] Simulator::throwFlow - Thread de geração de vazão finalizada após " << iteration << " iterações" << std::endl;
    }

    Simulator::Simulator() {
        std::cout << "[DEBUG] Simulator::Constructor - Iniciando construção do simulador" << std::endl;
        this->running.store(false);
        std::cout << "[DEBUG] Simulator::Constructor - Status running: " << (this->running.load() ? "true" : "false") << std::endl;
        
        std::cout << "[DEBUG] Simulator::Constructor - Criando hidrómetro com parâmetros padrão..." << std::endl;
        this->hidrometer = std::make_unique<Hidrometer>(0.1f, 1.0f, 0.0001f, 0.1f, 1.0f, 0.0001f);
        std::cout << "[DEBUG] Simulator::Constructor - Hidrómetro criado com sucesso" << std::endl;
        std::cout << "[DEBUG] Simulator::Constructor - Imagem inicializada com tamanho padrão" << std::endl;
        std::cout << "[DEBUG] Simulator::Constructor - Construção do simulador concluída" << std::endl;
    }

    Simulator::~Simulator() {
        std::cout << "[DEBUG] Simulator::Destructor - Iniciando destruição do simulador" << std::endl;
        stop();
        std::cout << "[DEBUG] Simulator::Destructor - Destruição concluída" << std::endl;
    }

    Hidrometer* Simulator::getHidrometer() const { return this->hidrometer.get(); }
    Pipe* Simulator::getPipeIN() const { return this->hidrometer->getPipeIN(); }
    Pipe* Simulator::getPipeOUT() const { return this->hidrometer->getPipeOUT(); }
    int Simulator::getCounter() const { return this->hidrometer->getCounter(); }
    bool Simulator::getHidrometerStatus() const { return this->hidrometer->getStatus(); }
    bool Simulator::isRunning() const { return this->running.load(); }

    void Simulator::run() {
        std::cout << "[DEBUG] Simulator::run - Iniciando simulação" << std::endl;
        this->running.store(true);
        std::cout << "[DEBUG] Simulator::run - Status running definido como: " << (this->running.load() ? "true" : "false") << std::endl;
        
        std::cout << "[DEBUG] Simulator::run - Ativando hidrómetro..." << std::endl;
        this->hidrometer->activate();
        
        std::cout << "[DEBUG] Simulator::run - Iniciando thread de geração de vazão..." << std::endl;
        this->inputThread = std::thread(&Simulator::throwFlow, this);
        
        std::cout << "[DEBUG] Simulator::run - Iniciando thread de atualização dinâmica de imagem..." << std::endl;
        this->imageThread = std::thread(&Simulator::imageUpdateLoop, this);
        
        std::cout << "[DEBUG] Simulator::run - Simulação iniciada com sucesso (2 threads ativas)" << std::endl;
    }

    void Simulator::stop() {
        std::cout << "[DEBUG] Simulator::stop - Iniciando parada da simulação" << std::endl;
        running.store(false);
        std::cout << "[DEBUG] Simulator::stop - Status running definido como: " << (this->running.load() ? "true" : "false") << std::endl;
        
        std::cout << "[DEBUG] Simulator::stop - Desativando hidrómetro..." << std::endl;
        this->hidrometer->deactivate();
        
        if (inputThread.joinable()) {
            std::cout << "[DEBUG] Simulator::stop - Aguardando finalização da thread de vazão..." << std::endl;
            inputThread.join();
            std::cout << "[DEBUG] Simulator::stop - Thread de vazão finalizada" << std::endl;
        }
        
        if (imageThread.joinable()) {
            std::cout << "[DEBUG] Simulator::stop - Aguardando finalização da thread de imagem..." << std::endl;
            imageThread.join();
            std::cout << "[DEBUG] Simulator::stop - Thread de imagem finalizada" << std::endl;
        }
        
        std::cout << "[DEBUG] Simulator::stop - Simulação parada com sucesso (todas as threads finalizadas)" << std::endl;
    }

    void Simulator::updateImage() const {
        std::cout << "[DEBUG] Simulator::updateImage - Atualizando imagem do hidrômetro" << std::endl;
        int counter = this->getCounter();
        float flowRate = this->getPipeIN()->getFlowRate();
        this->image.generate_image(counter, flowRate);
        std::cout << "[DEBUG] Simulator::updateImage - Imagem atualizada com counter=" << counter << "L, flow=" << flowRate << "m³/s" << std::endl;
    }

    void Simulator::imageUpdateLoop() const {
        std::cout << "[DEBUG] Simulator::imageUpdateLoop - Iniciando thread de atualização dinâmica de imagem" << std::endl;
        int updateCount = 0;
        
        // Aguarda um pouco para o sistema se estabilizar
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        while (this->running.load()) {
            updateCount++;
            
            try {
                // Atualiza a imagem com dados atuais
                int counter = this->getCounter();
                float flowRate = this->getPipeIN()->getFlowRate();
                
                std::cout << "[DEBUG] Simulator::imageUpdateLoop - Update #" << updateCount 
                          << " - Counter: " << counter << "L, Flow: " << std::fixed << std::setprecision(3) 
                          << flowRate << "m³/s" << std::endl;
                
                // Tenta gerar a imagem
                this->image.generate_image(counter, flowRate);
                std::cout << "[DEBUG] Simulator::imageUpdateLoop - Imagem gerada com sucesso para update #" << updateCount << std::endl;
                
            } catch (const std::exception& e) {
                std::cout << "[ERROR] Simulator::imageUpdateLoop - Erro na geração de imagem: " << e.what() << std::endl;
            } catch (...) {
                std::cout << "[ERROR] Simulator::imageUpdateLoop - Erro desconhecido na geração de imagem" << std::endl;
            }
            
            // Aguarda 3 segundos antes da próxima atualização
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        
        std::cout << "[DEBUG] Simulator::imageUpdateLoop - Thread de atualização de imagem finalizada após " 
                  << updateCount << " atualizações" << std::endl;
    }