# 📊 Diagrama de Classes - Simulador de Hidrômetro

Baseado na análise completa do seu projeto, criei um conjunto abrangente de diagramas de classes que documenta toda a arquitetura do simulador.

## 🎯 **Resumo dos Diagramas Criados**

### 1. **Diagrama de Classes Básico** (`class_diagram.puml`)
- Visão simplificada das principais classes
- Relacionamentos fundamentais (composição, dependência)
- Foco na estrutura básica

### 2. **Diagrama de Classes Detalhado** (`detailed_class_diagram.puml`) 
- Todos os métodos e atributos de cada classe
- Anotações sobre padrões de projeto utilizados
- Detalhes técnicos (thread-safety, tipos de dados)
- Constantes e configurações

### 3. **Diagrama de Sequência** (`sequence_diagram.puml`)
- Fluxo completo de execução do programa
- Interação entre threads (InputThread, ImageThread, UpdateThread)
- Ciclo de vida dos objetos
- Sincronização e comunicação entre componentes

### 4. **Diagrama de Arquitetura** (`architecture_overview.puml`)
- Visão macro dos layers da aplicação
- Dependências de infraestrutura (Cairo, Threading, File System)
- Fluxo de dados entre componentes

## 🏗️ **Arquitetura Identificada**

### **Classes Principais:**

| Classe | Responsabilidade | Padrão |
|--------|------------------|---------|
| `Main` | Ponto de entrada e controle geral | **Controller** |
| `Simulator` | Coordenação de threads e orquestração | **Facade/Controller** |
| `Hidrometer` | Lógica de negócio do hidrômetro | **Entity** |
| `Pipe` | Cálculos hidráulicos (Darcy-Weisbach) | **Strategy** |
| `Image` | Geração visual com Cairo | **Utility** |

### **Padrões de Projeto Identificados:**

1. **🔗 Composite Pattern**
   - `Hidrometer` composto por 2 `Pipe` (IN/OUT)
   - Tratamento uniforme de componentes hidráulicos

2. **🎯 Strategy Pattern**
   - Diferentes estratégias de cálculo (laminar/turbulento)
   - Implementação flexível de algoritmos hidráulicos

3. **👁️ Observer Pattern**
   - Threads observam mudanças de estado via `std::atomic`
   - Sincronização eficiente entre componentes

4. **🛠️ RAII Pattern**
   - `std::unique_ptr` para gerenciamento automático
   - Destructors seguros para Cairo resources

5. **🎮 Controller Pattern**
   - `Simulator` coordena toda a operação
   - Separação clara de responsabilidades

### **Características Técnicas:**

#### **🧵 Multi-Threading**
- **3 threads concorrentes:**
  - `InputThread`: Geração de vazão variável
  - `ImageThread`: Atualização visual (3s)
  - `UpdateThread`: Medição do hidrômetro (100ms)
- **Thread-safety** com `std::atomic` operations

#### **🔬 Simulação Física**
- **Equação Darcy-Weisbach** para cálculo de perda de carga
- **Regime turbulento/laminar** com número de Reynolds
- **Convergência iterativa** para precisão numérica
- **Perda realística** de 10% entre entrada e saída

#### **🎨 Visualização Cairo**
- **Renderização vetorial** para qualidade profissional
- **Design realístico** com gradientes metálicos
- **Display digital** mostrando volume e vazão
- **Ponteiro analógico** proporcional à vazão

## 📋 **Como Usar os Diagramas**

### **Opção 1: Servidor Online PlantUML**
1. Acesse: http://www.plantuml.com/plantuml/uml/
2. Copie o conteúdo de qualquer arquivo `.puml`
3. Cole no editor online
4. Visualize e exporte (PNG/SVG)

### **Opção 2: Instalar PlantUML Localmente**
```bash
# Ubuntu/Debian
sudo apt install plantuml

# Gerar diagramas
./generate_diagrams.sh
```

### **Opção 3: VS Code Extension**
1. Instale a extensão "PlantUML"
2. Abra qualquer arquivo `.puml`
3. Use `Ctrl+Shift+P` → "PlantUML: Preview"

## 🎓 **Valor Acadêmico**

Este conjunto de diagramas demonstra:

✅ **Análise de Arquitetura** - Identificação clara de componentes e responsabilidades

✅ **Padrões de Projeto** - Aplicação prática de 5+ padrões reconhecidos

✅ **Programação Concorrente** - Threading seguro com atomic operations

✅ **Integração de Bibliotecas** - Cairo Graphics, STL, Cálculos matemáticos

✅ **Documentação Técnica** - UML completo com especificações detalhadas

---

## 📁 **Arquivos Criados:**

```
docs/
├── README.md                    # Esta documentação
├── class_diagram.puml          # Diagrama básico
├── detailed_class_diagram.puml # Diagrama completo
├── sequence_diagram.puml       # Fluxo de execução
├── architecture_overview.puml  # Visão arquitetural
└── images/                     # Diagramas gerados (após executar script)
    ├── *.png                   # Versões em PNG
    └── *.svg                   # Versões em SVG

generate_diagrams.sh            # Script de geração automática
```