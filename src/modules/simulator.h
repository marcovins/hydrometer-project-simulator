#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <thread>
#include <memory>
#include <atomic>
#include "hidrometer.h"
#include "../utils/image.h"


class Simulator {
    public:
        Simulator();
        ~Simulator();

        Hidrometer* getHidrometer() const;
        Pipe* getPipeIN() const;
        Pipe* getPipeOUT() const;
        int getCounter() const;
        bool getHidrometerStatus() const;
        bool isRunning() const;

        void run();
        void stop();
        void generateImage() const { updateImage(); }

    private:
        void throwFlow() const;
        void updateImage() const;
        void imageUpdateLoop() const;

        std::atomic<bool> running;
        std::unique_ptr<Hidrometer> hidrometer;
        std::thread inputThread;
        std::thread imageThread;
        Image image;
};

#endif // SIMULATOR_H