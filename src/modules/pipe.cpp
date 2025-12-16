#include "pipe.hpp"
#include "../utils/logger.hpp"
#include <iostream>
#include <iomanip>

    Pipe::Pipe(float diameter, float length, float roughness){
            
            this->diameter = diameter;
            this->length = length;
            this->roughness = roughness;
            this->flowRate = 0.0;
            
            this->maxFlow = this->maxFlowForDeltaP(100000.0); // deltaP padrão de 100kPa
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
        
        if (this->diameter <= 0 || this->length <= 0 || deltaP <= 0) {
            return 0.0;
        }

        double V = 1.0;
        
        for (int iter = 0; iter < 100; ++iter) {
            double Re = (rho * V * this->diameter) / mu;
            double f;
            if (Re < 2000.0) {
                f = 64.0 / Re;
            } else {
                // Swamee–Jain (turbulento)
                double term = (roughness / (3.7 * diameter)) + (5.74 / pow(Re, 0.9));
                f = 0.25 / (pow(log10(term), 2.0));
            }

            double hf = deltaP / (rho * g); // m
            double V_new = sqrt((2.0 * g * hf * diameter) / (f * length));

            if (abs(V_new - V) < 1e-9) { 
                V = V_new; 
                break; 
            }
            V = V_new;
        }

        double area = M_PI * pow(diameter, 2) / 4.0;
        double flowRate = V * area;
                
        return flowRate;
    }

    void Pipe::setFlowRate(float flowRate_IN) {
        if (flowRate_IN < 0.0f){
            Logger::log(LogLevel::DEBUG, "[DEBUG] Pipe::setFlowRate - Vazão mínima atingida (" + std::to_string(this->flowRate) + " m³/s)");
            return;
        }
        
        // Usa tolerância de 0.1% para comparação de floats
        const float tolerance = 0.001f; // 0.1%
        if (flowRate_IN > this->maxFlow * (1.0f + tolerance)) {
            Logger::log(LogLevel::DEBUG, "[DEBUG] Pipe::setFlowRate - Vazão máxima atingida (" + std::to_string(this->flowRate) + " m³/s)");
            return;
        }
        
        // Limita ao maxFlow se estiver próximo
        this->flowRate = std::min(flowRate_IN, this->maxFlow);
    }
