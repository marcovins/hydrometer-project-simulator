#!/bin/bash

# Script para gerar diagramas de classes do simulador de hidrômetro

echo "=========================================="
echo "🎨 GERADOR DE DIAGRAMAS - SIMULADOR HIDRÔMETRO"
echo "=========================================="

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Diretórios
DOCS_DIR="docs"
OUTPUT_DIR="docs/images"

# Criar diretório de saída se não existir
if [ ! -d "$OUTPUT_DIR" ]; then
    echo -e "${BLUE}📁 Criando diretório: $OUTPUT_DIR${NC}"
    mkdir -p "$OUTPUT_DIR"
fi

# Verificar se PlantUML está instalado
if ! command -v plantuml &> /dev/null; then
    echo -e "${RED}❌ PlantUML não encontrado!${NC}"
    echo -e "${YELLOW}💡 Para instalar:${NC}"
    echo "   Ubuntu/Debian: sudo apt install plantuml"
    echo "   macOS: brew install plantuml"
    echo "   Windows: choco install plantuml"
    echo ""
    echo -e "${BLUE}🌐 Ou use o servidor online: http://www.plantuml.com/plantuml/uml/${NC}"
    exit 1
fi

echo -e "${GREEN}✅ PlantUML encontrado!${NC}"

# Lista de diagramas para gerar
declare -a diagrams=(
    "class_diagram:Diagrama de Classes Básico"
    "detailed_class_diagram:Diagrama de Classes Detalhado"  
    "sequence_diagram:Diagrama de Sequência"
)

echo -e "${BLUE}🔧 Gerando diagramas...${NC}"

# Gerar cada diagrama
for diagram_info in "${diagrams[@]}"; do
    IFS=':' read -r diagram_file diagram_name <<< "$diagram_info"
    
    input_file="$DOCS_DIR/${diagram_file}.puml"
    output_png="$OUTPUT_DIR/${diagram_file}.png"
    output_svg="$OUTPUT_DIR/${diagram_file}.svg"
    
    if [ -f "$input_file" ]; then
        echo -e "${YELLOW}⚙️  Processando: $diagram_name${NC}"
        
        # Gerar PNG
        if plantuml -tpng -o "../${OUTPUT_DIR}" "$input_file" 2>/dev/null; then
            echo -e "${GREEN}   ✅ PNG gerado: $output_png${NC}"
        else
            echo -e "${RED}   ❌ Erro ao gerar PNG para $diagram_file${NC}"
        fi
        
        # Gerar SVG
        if plantuml -tsvg -o "../${OUTPUT_DIR}" "$input_file" 2>/dev/null; then
            echo -e "${GREEN}   ✅ SVG gerado: $output_svg${NC}"
        else
            echo -e "${RED}   ❌ Erro ao gerar SVG para $diagram_file${NC}"
        fi
        
    else
        echo -e "${RED}❌ Arquivo não encontrado: $input_file${NC}"
    fi
done

# Verificar resultados
echo ""
echo -e "${BLUE}📊 Resumo da geração:${NC}"
png_files=$(find "$OUTPUT_DIR" -name "*.png" 2>/dev/null | wc -l)
svg_files=$(find "$OUTPUT_DIR" -name "*.svg" 2>/dev/null | wc -l)

echo -e "${GREEN}   📈 Arquivos PNG gerados: $png_files${NC}"
echo -e "${GREEN}   🎨 Arquivos SVG gerados: $svg_files${NC}"

# Listar arquivos gerados
if [ $png_files -gt 0 ] || [ $svg_files -gt 0 ]; then
    echo ""
    echo -e "${BLUE}📁 Arquivos gerados em $OUTPUT_DIR:${NC}"
    ls -la "$OUTPUT_DIR"
fi

echo ""
echo -e "${GREEN}✨ Geração de diagramas concluída!${NC}"
echo -e "${BLUE}💡 Para visualizar:${NC}"
echo "   - Abra os arquivos PNG em qualquer visualizador de imagens"
echo "   - Abra os arquivos SVG em navegadores ou editores vetoriais"
echo "   - Use 'eog docs/images/' para visualizar no Linux"

echo "=========================================="
