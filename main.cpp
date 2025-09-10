#include "modules/simulator.hpp"
#include <iostream>

using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << "[INFO] SIMULADOR DE HIDRÓMETRO INICIANDO" << endl;
    cout << "========================================" << endl;
    
    cout << "[INFO] Criando instância do simulador..." << endl;
    Simulator simulator;
    
    cout << "[INFO] Iniciando simulação..." << endl;
    simulator.run();

    // Aguarda um pouco para a simulação inicializar
    this_thread::sleep_for(chrono::milliseconds(500));
    
    cout << "\n[INFO] ====== STATUS INICIAL ======" << endl;
    cout << "[INFO] Hidrómetro status: " << (simulator.getHidrometer()->getStatus() ? "Active" : "Inactive") << endl;
    cout << "[INFO] Contador inicial: " << simulator.getCounter() << " L" << endl;
    cout << "[INFO] Simulação rodando: " << (simulator.isRunning() ? "Sim" : "Não") << endl;
    cout << "[INFO] ==============================" << endl;
    
    cout << "\n[INFO] Monitorando simulação (pressione 'q' + Enter para sair)..." << endl;
    while(simulator.isRunning() && simulator.getCounter() < 200) {
        
        this_thread::sleep_for(chrono::seconds(2));
        
        // Verifica se há input disponível (não bloqueante)
        cin.sync();
        if (cin.rdbuf()->in_avail() > 0) {
            char input;
            cin >> input;
            if (input == 'q' || input == 'Q') {
                cout << "[INFO] Saída solicitada pelo usuário" << endl;
                break;
            }
        }

    }
    
    cout << "\n[INFO] ====== STATUS FINAL ======" << endl;
    cout << "[INFO] Contador final: " << simulator.getCounter() << " L" << endl;
    cout << "[INFO] =============================" << endl;
    
    cout << "\n[INFO] Parando simulação..." << endl;
    simulator.stop();
    
    cout << "[INFO] Simulação finalizada com sucesso!" << endl;
    cout << "[INFO] A imagem 'hidrometro.png' foi atualizada dinamicamente durante a execução!" << endl;
    cout << "========================================" << endl;

    cout << "Hidrometer status: " << (simulator.getHidrometer()->getStatus() ? "Active" : "Inactive") << endl;

    return 0;
}