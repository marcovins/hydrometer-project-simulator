#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <thread>
#include <memory>
#include <atomic>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "hidrometer.hpp"
#include "../utils/image.hpp"

#define IMAGE_PATH "medicoes_202311250013/"
#define MAX_SIM 5
enum Key {
    KEY_UP = 1000,
    KEY_DOWN = 1001,
    KEY_RIGHT = 1002,
    KEY_LEFT = 1003,
    KEY_ESC = 27
};

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
        int getKey() const;
        void updateFlow();
        void updateImage() const;
        void imageUpdateLoop() const;

        std::atomic<bool> running;
        std::unique_ptr<Hidrometer[]> hidrometer;
        std::thread inputThread;
        std::thread imageThread;
        std::atomic<int> atual;
        Image image;
};

#endif // SIMULATOR_H