#ifndef HIDROMETER_H
#define HIDROMETER_H

#include "pipe.hpp"
#include <thread>
#include <chrono>
#include <memory>
#include <atomic>

#define DIAMETER_IN 0.015f
#define LENGTH_IN 0.15f
#define ROUGHNESS_IN 0.00005f
#define DIAMETER_OUT 0.015f
#define LENGTH_OUT 0.15f
#define ROUGHNESS_OUT 0.00005f

class Hidrometer {
    public:
        Hidrometer(float diameterIN = DIAMETER_IN, float lengthIN = LENGTH_IN, float roughnessIN = ROUGHNESS_IN,
                float diameterOUT = DIAMETER_OUT, float lengthOUT = LENGTH_OUT, float roughnessOUT = ROUGHNESS_OUT);

        ~Hidrometer();

        Pipe* getPipeIN() const;
        Pipe* getPipeOUT() const;
        int getCounter() const;
        bool getStatus() const;

        void activate();
        void deactivate();
        void shutdown();  // Para completamente o hidrômetro (finaliza thread)
        void setCounter(int valor);  // Restaura contador (para persistência)

    private:
        void update();

        std::unique_ptr<Pipe> pipeIN;
        std::unique_ptr<Pipe> pipeOUT;
        std::thread update_thread;
        std::atomic<bool> running;
        std::atomic<int> counter;
        std::atomic<bool> status;
        
        // Contador interno com maior precisão
        float counterFloat;
    };

#endif // HIDROMETER_H
