#include "simulator.hpp"
#include "../utils/logger.hpp"

    int Simulator::getKey() const {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Desativa modo canônico e eco
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        if (ch == 27) { // ESC ou sequência de escape
            if (getchar() == 91) { // [
                ch = getchar();
                switch (ch) {
                    case 'A': return KEY_UP;    // seta cima
                    case 'B': return KEY_DOWN;  // seta baixo
                    case 'C': return KEY_RIGHT; // seta direita
                    case 'D': return KEY_LEFT;  // seta esquerda
                    default: return 0;
                }
            } else {
                return KEY_ESC; // ESC simples
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restaura configurações
        return ch;
    }

    void Simulator::updateFlow(){
        int input;
        float maxFlow = this->hidrometer->getPipeIN()->getMaxFlow();
        float chunks = maxFlow / 50.0f;
        float currentFlow = this->hidrometer->getPipeIN()->getFlowRate();


        tcflush(STDIN_FILENO, TCIFLUSH);

        int iteration = 0;
        while (this->running.load()) {
            iteration++;
            input = this->getKey();
            switch (input) {
                case KEY_UP:
                case KEY_RIGHT:
                    currentFlow = this->hidrometer->getPipeIN()->getFlowRate();
                    this->hidrometer->getPipeIN()->setFlowRate(currentFlow + chunks);
                    tcflush(STDIN_FILENO, TCIFLUSH);
                    break;

                case KEY_DOWN: // seta baixo
                case KEY_LEFT: // seta esquerda
                    currentFlow = this->hidrometer->getPipeIN()->getFlowRate();
                    this->hidrometer->getPipeIN()->setFlowRate(currentFlow - chunks);
                    tcflush(STDIN_FILENO, TCIFLUSH); // Limpa buffer
                    break;

                case KEY_ESC: // ESC
                    Logger::log(LogLevel::SHUTDOWN, "[INFO] Saída solicitada pelo usuário");
                    this->running.store(false);
                    tcflush(STDIN_FILENO, TCIFLUSH);
                    break;
                default:
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    break;
            }
        }
        Logger::log(LogLevel::SHUTDOWN, "[DEBUG] Simulator::updateFlow - Thread de geração de vazão finalizada após " + std::to_string(iteration) + " iterações");
    }

    Simulator::Simulator() {
        this->running.store(false);
        
        // Hidrômetro residencial padrão com dimensões realísticas:
        // Diâmetro: 15mm (0.015m) - padrão residencial
        // Comprimento: 0.15m - tamanho típico de medidor residencial
        // Rugosidade: 0.00005m - PVC/metal padrão residencial
        this->hidrometer = std::make_unique<Hidrometer>(0.015f, 0.15f, 0.00005f, 0.015f, 0.15f, 0.00005f);
    }

    Simulator::~Simulator() {
        stop();
    }

    Hidrometer* Simulator::getHidrometer() const { return this->hidrometer.get(); }
    Pipe* Simulator::getPipeIN() const { return this->hidrometer->getPipeIN(); }
    Pipe* Simulator::getPipeOUT() const { return this->hidrometer->getPipeOUT(); }
    int Simulator::getCounter() const { return this->hidrometer->getCounter(); }
    bool Simulator::getHidrometerStatus() const { return this->hidrometer->getStatus(); }
    bool Simulator::isRunning() const { return this->running.load(); }

    void Simulator::run() {
        this->running.store(true);
        this->hidrometer->activate();
        this->inputThread = std::thread(&Simulator::updateFlow, this);
        this->imageThread = std::thread(&Simulator::imageUpdateLoop, this);
    }

    void Simulator::stop() {

        running.store(false);
        this->hidrometer->deactivate();
        
        if (inputThread.joinable()) {
            inputThread.join();
        }
        if (imageThread.joinable()) {
            imageThread.join();
        }

        // Restaura configurações do terminal
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= (ICANON | ECHO | ISIG); // Restaura modo normal
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }

    void Simulator::imageUpdateLoop() const {
        int updateCount = 0;
        
        int imageCounter = 1; // contador para nome do arquivo
        int nextImageThreshold = 1; // Próximo marco para gerar imagem (em litros = 1m³)

        while (this->running.load()) {
            updateCount++;
            int currentCounter = this->getCounter();
            
            try {
                // Verifica se atingiu o próximo metro cúbico (1000L)
                if (currentCounter >= nextImageThreshold) {
                    int counter = this->getCounter();
                    float flowRate = this->getPipeIN()->getFlowRate();

                    // Gerar nome do arquivo com zero à esquerda (01.jpeg, 02.jpeg, ..., 99.jpeg)
                    std::ostringstream filename;
                    filename << std::setw(2) << std::setfill('0') << imageCounter << ".jpeg";
                    std::string fileNameStr = IMAGE_PATH + filename.str(); // concatena com o caminho

                    Logger::log(LogLevel::DEBUG, "[DEBUG] Simulator::imageUpdateLoop - Update #" + 
                            std::to_string(updateCount) + " - Counter: " + std::to_string(counter) + 
                            "L (" + std::to_string(counter/1000.0) + "m³), Flow: " + std::to_string(flowRate) + "m³/s - File: " + fileNameStr);
                    
                    // Passa o caminho completo do arquivo como terceiro argumento
                    float maxFlowRate = this->getPipeIN()->getMaxFlow();
                    this->image.generate_image(counter, flowRate, maxFlowRate, fileNameStr);

                    Logger::log(LogLevel::DEBUG, "[DEBUG] Simulator::imageUpdateLoop - Imagem gerada com sucesso para update #" + 
                            std::to_string(updateCount) + " -> " + fileNameStr + " (Marco: " + std::to_string(nextImageThreshold/1000.0) + "m³)");

                    // Atualiza para o próximo marco de 1m³ (1000L)
                    nextImageThreshold += 1;
                    
                    // Atualiza contador de arquivo e volta para 1 após 99
                    imageCounter++;
                    if (imageCounter > 99) imageCounter = 1;
                } else {
                    // Aguarda um pouco antes de verificar novamente
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

            } catch (const std::exception& e) {
                Logger::log(LogLevel::DEBUG, "[ERROR] Simulator::imageUpdateLoop - Erro na geração de imagem: " + std::string(e.what()));
            } catch (...) {
                Logger::log(LogLevel::DEBUG, "[ERROR] Simulator::imageUpdateLoop - Erro desconhecido na geração de imagem");
            }
        }
    }