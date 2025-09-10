# Diagramas de Classes - Simulador de Hidrômetro

Este documento apresenta os diagramas de classes e arquitetura do simulador de hidrômetro desenvolvido em C++.

## 📋 Índice

1. [Visão Geral da Arquitetura](#visão-geral-da-arquitetura)
2. [Diagrama de Classes Simplificado](#diagrama-de-classes-simplificado)
3. [Diagrama de Classes Detalhado](#diagrama-de-classes-detalhado)
4. [Diagrama de Sequência](#diagrama-de-sequência)
5. [Padrões de Projeto Utilizados](#padrões-de-projeto-utilizados)
6. [Tecnologias e Bibliotecas](#tecnologias-e-bibliotecas)

## 🏗️ Visão Geral da Arquitetura

O simulador de hidrômetro é uma aplicação multi-thread que simula o comportamento real de um medidor de água, incluindo:

- **Cálculos hidráulicos** baseados na equação de Darcy-Weisbach
- **Simulação em tempo real** com threads concorrentes
- **Visualização gráfica** utilizando a biblioteca Cairo
- **Monitoramento contínuo** do volume acumulado e vazão

### Componentes Principais

| Componente | Responsabilidade | Tecnologia |
|------------|------------------|------------|
| **Main** | Ponto de entrada e controle geral | C++ STL |
| **Simulator** | Coordenação e gerenciamento de threads | std::thread, std::atomic |
| **Hidrometer** | Lógica do hidrômetro e medição | std::atomic, smart pointers |
| **Pipe** | Cálculos hidráulicos e fluxo | Equações matemáticas |
| **Image** | Geração de visualização | Cairo Graphics |

## 📊 Diagrama de Classes Simplificado

O diagrama básico mostra as relações fundamentais entre as classes:

```plantuml
@startuml
!include docs/class_diagram.puml
@enduml
```

### Principais Relacionamentos

- **Simulator** *compõe* um **Hidrometer** e uma **Image**
- **Hidrometer** *compõe* dois **Pipe** (entrada e saída)
- **Main** *cria e gerencia* o **Simulator**

## 🔍 Diagrama de Classes Detalhado

O diagrama detalhado inclui todos os métodos, atributos e anotações sobre padrões:

```plantuml
@startuml
!include docs/detailed_class_diagram.puml
@enduml
```

### Características Técnicas

#### Thread Safety
- **Atomic Variables**: `std::atomic<bool>`, `std::atomic<int>` 
- **Smart Pointers**: `std::unique_ptr` para gerenciamento automático
- **Thread Management**: Threads dedicadas para cada operação

#### Cálculos Hidráulicos
- **Equação Darcy-Weisbach**: Perda de carga em tubulações
- **Regime Turbulento/Laminar**: Detecção automática baseada no número de Reynolds
- **Convergência Iterativa**: Método numérico para encontrar vazão exata

## ⏱️ Diagrama de Sequência

O fluxo de execução mostra como as classes interagem durante a simulação:

```plantuml
@startuml
!include docs/sequence_diagram.puml
@enduml
```

### Fases de Execução

1. **Inicialização**: Criação de objetos e configuração inicial
2. **Ativação**: Start das threads e início da simulação
3. **Execução**: Loops concorrentes de geração, medição e visualização
4. **Monitoramento**: Verificação contínua de status pelo usuário
5. **Finalização**: Parada controlada e limpeza de recursos

## 🎯 Padrões de Projeto Utilizados

### 1. **Composite Pattern**
- **Contexto**: Hidrometer composto por 2 Pipes
- **Benefício**: Tratamento uniforme de componentes hidráulicos

### 2. **Strategy Pattern** 
- **Contexto**: Diferentes estratégias de cálculo hidráulico (laminar/turbulento)
- **Benefício**: Flexibilidade para diferentes regimes de fluxo

### 3. **Observer Pattern**
- **Contexto**: Threads observam mudanças de estado atomicamente
- **Benefício**: Sincronização eficiente entre componentes

### 4. **RAII (Resource Acquisition Is Initialization)**
- **Contexto**: Smart pointers e destructors automáticos
- **Benefício**: Gerenciamento seguro de memória e recursos

### 5. **Controller Pattern**
- **Contexto**: Simulator coordena todas as operações
- **Benefício**: Separação clara de responsabilidades

## 🛠️ Tecnologias e Bibliotecas

### Core C++
- **C++14 Standard**: Features modernas como auto, lambdas, smart pointers
- **STL Threading**: `std::thread`, `std::atomic`, `std::chrono`
- **Memory Management**: `std::unique_ptr`, RAII patterns

### Bibliotecas Externas
- **Cairo Graphics**: Renderização 2D vetorial para PNG
- **pkg-config**: Integração automática de dependências
- **GNU Make**: Sistema de build multiplataforma

### Cálculos Matemáticos
- **Darcy-Weisbach Equation**: Cálculo de perda de carga
- **Reynolds Number**: Determinação de regime de fluxo
- **Colebrook-White**: Fator de atrito para regime turbulento

## 📈 Funcionalidades Implementadas

### ✅ Simulação Física
- [x] Cálculo de vazão máxima baseado em ΔP
- [x] Perda de carga realística (10%)
- [x] Convergência numérica iterativa
- [x] Validação de limites físicos

### ✅ Concorrência
- [x] Thread para geração de vazão variável
- [x] Thread para atualização do hidrômetro
- [x] Thread para geração de imagens
- [x] Sincronização thread-safe

### ✅ Visualização
- [x] Geração dinâmica de imagens PNG
- [x] Design realístico de hidrômetro
- [x] Display digital com volume e vazão
- [x] Ponteiro analógico proporcional

### ✅ Monitoramento
- [x] Logs detalhados de depuração
- [x] Status em tempo real
- [x] Controle interativo (quit com 'q')
- [x] Limite automático de volume

---

## 🚀 Como Usar os Diagramas

Para visualizar os diagramas PlantUML:

1. **Online**: Acesse [PlantUML Online Server](http://www.plantuml.com/plantuml/uml/)
2. **VS Code**: Instale a extensão "PlantUML"
3. **CLI**: Instale plantuml e execute `plantuml diagram.puml`

### Comandos para Gerar Imagens

```bash
# Instalar PlantUML (Ubuntu/Debian)
sudo apt install plantuml

# Gerar diagramas em PNG
plantuml docs/*.puml

# Gerar diagramas em SVG
plantuml -tsvg docs/*.puml
```

---

*Diagrama gerado automaticamente baseado na análise do código fonte do projeto*
