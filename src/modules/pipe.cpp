#include "pipe.hpp"
#include <iostream>
#include <iomanip>

    Pipe::Pipe(float diameter, float length, float roughness){
            std::cout << "[DEBUG] Pipe::Constructor - Criando pipe: D=" << diameter << "m, L=" << length << "m, R=" << roughness << "m" << std::endl;
            
            this->diameter = diameter;
            this->length = length;
            this->roughness = roughness;
            this->flowRate = 0.0;
            
            std::cout << "[DEBUG] Pipe::Constructor - Calculando vazão máxima para ΔP=100kPa..." << std::endl;
            this->maxFlow = this->maxFlowForDeltaP(100000.0); // deltaP padrão de 100kPa
            std::cout << "[DEBUG] Pipe::Constructor - Vazão máxima calculada: " << std::fixed << std::setprecision(6) << this->maxFlow << " m³/s" << std::endl;
            std::cout << "[DEBUG] Pipe::Constructor - Pipe criado com sucesso" << std::endl;
        }

    float Pipe::getDiameter() const { return this->diameter; }
    float Pipe::getLength() const { return this->length; }
    float Pipe::getRoughness() const { return this->roughness; }
    float Pipe::getFlowRate() const { return this->flowRate; }
    float Pipe::getMaxFlow() const { return this->maxFlow; }

    // Retorna vazão (m^3/s) para uma queda de pressão deltaP (Pa)
    double Pipe::maxFlowForDeltaP(double deltaP,
                            double rho,          // kg/m^3
                            double mu,       // Pa·s
                            double g) const  // m/s^2
    {
        std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Calculando vazão para ΔP=" << deltaP << "Pa, ρ=" << rho << "kg/m³, μ=" << mu << "Pa·s" << std::endl;
        
        if (this->diameter <= 0 || this->length <= 0 || deltaP <= 0) {
            std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Parâmetros inválidos, retornando 0.0" << std::endl;
            return 0.0;
        }

        double V = 1.0;
        int convergenceIter = 0;
        std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Iniciando iteração para convergência da velocidade" << std::endl;
        
        for (int iter = 0; iter < 100; ++iter) {
            double Re = (rho * V * this->diameter) / mu;
            double f;
            if (Re < 2000.0) {
                f = 64.0 / Re;
                if (iter == 0) std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Regime laminar detectado (Re=" << Re << ")" << std::endl;
            } else {
                // Swamee–Jain (turbulento)
                double term = (roughness / (3.7 * diameter)) + (5.74 / pow(Re, 0.9));
                f = 0.25 / (pow(log10(term), 2.0));
                if (iter == 0) std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Regime turbulento detectado (Re=" << Re << ")" << std::endl;
            }

            double hf = deltaP / (rho * g); // m
            double V_new = sqrt((2.0 * g * hf * diameter) / (f * length));

            if (abs(V_new - V) < 1e-9) { 
                V = V_new; 
                convergenceIter = iter + 1;
                break; 
            }
            V = V_new;
        }

        double area = M_PI * pow(diameter, 2) / 4.0;
        double flowRate = V * area;
        
        std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Convergência em " << convergenceIter << " iterações" << std::endl;
        std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Velocidade final: " << std::fixed << std::setprecision(4) << V << " m/s" << std::endl;
        std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Área da seção: " << std::fixed << std::setprecision(6) << area << " m²" << std::endl;
        std::cout << "[DEBUG] Pipe::maxFlowForDeltaP - Vazão calculada: " << std::fixed << std::setprecision(6) << flowRate << " m³/s" << std::endl;
        
        return flowRate;
    }

    void Pipe::setFlowRate(float flowRate) { 
        float oldFlowRate = this->flowRate;
        this->flowRate = std::min(flowRate, this->maxFlow);
        
        // Log apenas se houve mudança significativa (>1% ou primeira definição)
        if (oldFlowRate == 0.0f || std::abs(this->flowRate - oldFlowRate) / oldFlowRate > 0.01f) {
            std::cout << "[DEBUG] Pipe::setFlowRate - Vazão alterada: " 
                      << std::fixed << std::setprecision(6) << oldFlowRate 
                      << " -> " << this->flowRate << " m³/s";
            if (flowRate > this->maxFlow) {
                std::cout << " (limitada ao máximo de " << this->maxFlow << " m³/s)";
            }
            std::cout << std::endl;
        }
    }
