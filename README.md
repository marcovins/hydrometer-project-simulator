# Simulador de Hidrômetro Residencial

Um simulador completo de hidrômetro em C++ com visualização gráfica em tempo real, cálculos hidráulicos precisos e arquitetura multi-thread.

## 🚀 Funcionalidades Principais

### � Simulação Hidráulica Realística
- **Cálculos baseados na equação de Darcy-Weisbach** para perda de carga
- **Detecção automática de regime** (laminar/turbulento) via número de Reynolds
- **Dimensões residenciais padrão**: 15mm diâmetro, rugosidade 0.00005m
- **Vazão máxima dinâmica** calculada automaticamente com base nas características do sistema

### 🎮 Interface Interativa
- **Controle em tempo real** da vazão usando setas do teclado (↑↓←→)
- **Saída controlada** com tecla ESC
- **Display organizado** com 4 linhas de informação sempre visíveis
- **Logs estruturados** com níveis (STARTUP, SHUTDOWN, RUNTIME, DEBUG)

### 📊 Visualização Avançada
- **Geração automática de imagens** a cada metro cúbico acumulado
- **Escala dinâmica** que se ajusta automaticamente à vazão máxima do sistema
- **Design realístico** com gradientes metálicos, displays digitais e ponteiro analógico
- **Formato PNG** com qualidade profissional usando Cairo Graphics

### 🔢 Sistema de Medição Consistente
- **Unidades padronizadas** em metros cúbicos (m³) em toda a aplicação
- **Conversões automáticas** entre L/min, m³/h e m³/s conforme necessário
- **Display digital** formatado como hidrômetros reais (%06d format)
- **Contador acumulativo** thread-safe com precisão de litros

### ⚡ Arquitetura Multi-Thread
- **Thread de entrada**: Captura comandos do usuário
- **Thread de medição**: Atualiza contador e cálculos hidráulicos
- **Thread de visualização**: Gera imagens periodicamente
- **Sincronização thread-safe** com `std::atomic` e smart pointers

## 🏗️ Arquitetura do Sistema

### Componentes Principais

| Componente | Responsabilidade | Funcionalidades |
|------------|------------------|-----------------|
| **Simulator** | Controlador principal | Gerenciamento de threads, entrada do usuário, geração de imagens |
| **Hidrometer** | Medidor digital | Contador thread-safe, cálculos de volume, status de ativação |
| **Pipe** | Sistema hidráulico | Equação Darcy-Weisbach, regime de fluxo, vazão máxima dinâmica |
| **Image** | Visualização | Escala dinâmica, design realístico, salvamento PNG |
| **Logger** | Sistema de logs | Níveis estruturados, conversões de unidade, output organizado |

### Fluxo de Dados

```
Entrada do Usuário → Simulator → Hidrometer → Pipe → Cálculos Hidráulicos
                         ↓
                    Image Generator ← Logger ← Medições em Tempo Real
```

## 📊 Diagramas de Classes

### Diagrama Simplificado
```plantuml
@startuml
!include docs/class_diagram.puml
@enduml
```

### Diagrama Detalhado
```plantuml
@startuml
!include docs/detailed_class_diagram.puml
@enduml
```

## ⏱️ Diagrama de Sequência
```plantuml
@startuml
!include docs/sequence_diagram.puml
@enduml
```

## 🔧 Tecnologias Utilizadas

### Core C++14
- **Threading**: `std::thread`, `std::atomic<bool>`, `std::atomic<int>`
- **Memory Management**: `std::unique_ptr`, RAII patterns
- **Synchronization**: Thread-safe operations, non-blocking input

### Bibliotecas Externas
- **Cairo Graphics**: Renderização vetorial PNG com gradientes e texto
- **termios**: Controle de terminal para entrada não-bloqueante
- **GNU Make**: Sistema de build com pkg-config integration

### Algoritmos Matemáticos
- **Darcy-Weisbach**: Perda de carga em tubulações circulares
- **Reynolds Number**: Determinação de regime (Re < 2300 laminar, Re > 4000 turbulento)
- **Colebrook-White**: Fator de atrito para regime turbulento
- **Convergência Iterativa**: Método numérico para vazão exata

## 🎯 Padrões de Projeto

### 1. **Composite Pattern**
- **Implementação**: Hidrometer composto por Pipe IN + Pipe OUT
- **Benefício**: Tratamento uniforme do sistema hidráulico

### 2. **Strategy Pattern**
- **Implementação**: Diferentes cálculos para regime laminar/turbulento
- **Benefício**: Flexibilidade nos algoritmos hidráulicos

### 3. **Observer Pattern**
- **Implementação**: Threads observam mudanças via atomic variables
- **Benefício**: Sincronização eficiente sem locks

### 4. **RAII Pattern**
- **Implementação**: Smart pointers e destructors automáticos
- **Benefício**: Gerenciamento seguro de recursos Cairo e threads

### 5. **MVC Pattern**
- **Implementação**: Simulator (Controller), Hidrometer (Model), Image (View)
- **Benefício**: Separação clara de responsabilidades

## 🚀 Como Compilar e Executar

### Pré-requisitos
```bash
# Ubuntu/Debian
sudo apt install build-essential libcairo2-dev pkg-config

# CentOS/RHEL
sudo yum install gcc-c++ cairo-devel pkgconfig

# macOS
brew install cairo pkg-config
```

### Compilação
```bash
# Clone o repositório
git clone https://github.com/marcovins/hydrometer-project-simulator.git
cd hydrometer-project-simulator

# Compile o projeto
make clean && make

# Execute o simulador
./hidrometer_simulator
```

### Uso do Simulador
1. **Iniciar**: Execute `./hidrometer_simulator`
2. **Controlar vazão**: Use ↑↓←→ para ajustar a vazão
3. **Monitorar**: Observe as 4 linhas de status em tempo real
4. **Ver imagens**: Imagens são geradas automaticamente em `medicoes_202311250013/`
5. **Sair**: Pressione ESC para finalizar

## 📈 Funcionalidades Detalhadas

### ✅ Sistema Hidráulico
- [x] Vazão máxima calculada dinamicamente (17+ m³/h típico)
- [x] Perda de carga realística (~10% entre entrada e saída)
- [x] Convergência numérica com precisão de 0.001%
- [x] Validação de limites físicos e sanitários

### ✅ Interface e Controle
- [x] Entrada não-bloqueante via termios
- [x] Ajuste incremental de vazão (chunks de 2% da vazão máxima)
- [x] Display organizado com contadores em m³
- [x] Logs estruturados com conversão de unidades

### ✅ Visualização Dinâmica
- [x] Escala automática baseada na vazão máxima real
- [x] Marcações principais e secundárias proporcionais
- [x] Ponteiro analógico com normalização precisa
- [x] Display digital com formato %06d (ex: "000001 L")

### ✅ Geração de Imagens
- [x] Trigger automático a cada 1000L (1m³) acumulado
- [x] Resolução 400x400 com qualidade profissional
- [x] Gradientes metálicos e efeitos realísticos
- [x] Numeração sequencial (01.png, 02.png, ...)

### ✅ Thread Safety
- [x] Atomic counters para volume acumulado
- [x] Atomic flags para controle de execução
- [x] Smart pointers para gerenciamento de memória
- [x] Sincronização sem deadlocks

## 📊 Especificações Técnicas

### Parâmetros Hidráulicos
- **Diâmetro**: 15mm (padrão residencial)
- **Comprimento**: 0.15m por tubulação
- **Rugosidade**: 0.00005m (PVC/metal)
- **Vazão máxima**: ~17 m³/h (calculada dinamicamente)

### Performance
- **Threads**: 3 threads concorrentes
- **Frequência de atualização**: 50ms por ciclo
- **Precisão de medição**: 1 litro
- **Geração de imagens**: A cada 1000L acumulados

### Formatos de Saída
- **Imagens**: PNG 400x400 pixels
- **Logs**: STDOUT com timestamps implícitos
- **Medições**: Metros cúbicos (m³) padronizado
- **Numeração**: Formato digital %06d

## � Documentação e Diagramas

### Visualizar Diagramas PlantUML

Os diagramas de arquitetura estão em formato PlantUML na pasta `docs/`:

- `architecture_overview.puml` - Visão geral da arquitetura
- `class_diagram.puml` - Diagrama de classes simplificado  
- `detailed_class_diagram.puml` - Diagrama de classes detalhado
- `sequence_diagram.puml` - Diagrama de sequência

#### Métodos para Visualização

1. **VS Code**: Instale a extensão "PlantUML" 
2. **Online**: [PlantUML Online Server](http://www.plantuml.com/plantuml/uml/)
3. **CLI**: Instale plantuml localmente

```bash
# Ubuntu/Debian
sudo apt install plantuml

# Gerar imagens dos diagramas
plantuml docs/*.puml

# Gerar em SVG (vetorial)
plantuml -tsvg docs/*.puml
```

### Documentação Adicional

- `docs/CHANGELOG.md` - Histórico de melhorias e funcionalidades
- `docs/README.md` - Documentação técnica detalhada
- `docs/images/` - Diagramas exportados em PNG/SVG

---

## 🤝 Contribuição

### Estrutura de Branches
- `main` - Versão estável
- `develop` - Desenvolvimento ativo
- `feature/*` - Novas funcionalidades  

### Padrões de Código
- C++14 standard
- RAII patterns obrigatórios
- Thread-safety com atomic operations
- Documentação inline para métodos públicos

---

## 📄 Licença

Este projeto está licenciado sob a MIT License - veja o arquivo [LICENSE](LICENSE) para detalhes.

---

## 👥 Autores

- **Marco Vinicius** - Desenvolvimento principal - [@marcovins](https://github.com/marcovins)

---

## 🙏 Agradecimentos

- Biblioteca Cairo Graphics pela renderização profissional
- Comunidade C++ pelas melhores práticas
- Documentação PlantUML pela clareza dos diagramas