#!/bin/bash

echo "=========================================="
echo "🎨 GERADOR DE DIAGRAMAS - SIMULADOR HIDRÔMETRO"
echo "=========================================="

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

DOCS_DIR="docs"
OUTPUT_DIR="docs/images"

if [ ! -d "$OUTPUT_DIR" ]; then
    echo -e "${BLUE}📁 Criando diretório: $OUTPUT_DIR${NC}"
    mkdir -p "$OUTPUT_DIR"
fi

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

declare -a diagrams=(
    "class_diagram:Diagrama de Classes Detalhado - SHA v2.0\nPadrões: Composite, Strategy, Observer, RAII, MVC"
    "simple_class_diagram:Diagrama de Classes Simplificado - Visão Geral"
    "sequence_diagram:Diagrama de Sequência - Fluxo de Execução"
)

echo -e "${BLUE}🔧 Gerando diagramas...${NC}"

for diagram_info in "${diagrams[@]}"; do
    IFS=':' read -r diagram_file diagram_name <<< "$diagram_info"
    
    input_file="$DOCS_DIR/${diagram_file}.puml"
    output_pdf="$OUTPUT_DIR/${diagram_file}.pdf"
    output_png="$OUTPUT_DIR/${diagram_file}.png"
    output_svg="$OUTPUT_DIR/${diagram_file}.svg"

    if [ -f "$input_file" ]; then
        echo -e "${YELLOW}⚙️  Processando: $diagram_name${NC}"

        if plantuml -tpdf -o "../${OUTPUT_DIR}" "$input_file" 2>/dev/null; then
            echo -e "${GREEN}   ✅ PDF gerado: $output_pdf${NC}"
        else
            echo -e "${RED}   ❌ Erro ao gerar PDF para $diagram_file${NC}"
        fi

        if plantuml -tpng -o "../${OUTPUT_DIR}" "$input_file" 2>/dev/null; then
            echo -e "${GREEN}   ✅ PNG gerado: $output_png${NC}"
        else
            echo -e "${RED}   ❌ Erro ao gerar PNG para $diagram_file${NC}"
        fi

        if plantuml -tsvg -o "../${OUTPUT_DIR}" "$input_file" 2>/dev/null; then
            echo -e "${GREEN}   ✅ SVG gerado: $output_svg${NC}"
        else
            echo -e "${RED}   ❌ Erro ao gerar SVG para $diagram_file${NC}"
        fi
        
    else
        echo -e "${RED}❌ Arquivo não encontrado: $input_file${NC}"
    fi
done

echo ""
echo -e "${BLUE}📊 Resumo da geração:${NC}"
pdf_files=$(find "$OUTPUT_DIR" -name "*.pdf" 2>/dev/null | wc -l)
png_files=$(find "$OUTPUT_DIR" -name "*.png" 2>/dev/null | wc -l)
svg_files=$(find "$OUTPUT_DIR" -name "*.svg" 2>/dev/null | wc -l)

echo -e "${GREEN}   📈 Arquivos PDF gerados: $pdf_files${NC}"
echo -e "${GREEN}   🖼️  Arquivos PNG gerados: $png_files${NC}"
echo -e "${GREEN}   🎨 Arquivos SVG gerados: $svg_files${NC}"

total_files=$((pdf_files + png_files + svg_files))
if [ $total_files -gt 0 ]; then
    echo ""
    echo -e "${BLUE}📁 Arquivos gerados em $OUTPUT_DIR:${NC}"
    ls -la "$OUTPUT_DIR"
fi

echo ""
echo -e "${GREEN}✨ Geração de diagramas concluída!${NC}"
echo -e "${BLUE}💡 Para visualizar:${NC}"
echo "   - PDF: Visualizador de PDF"
echo "   - PNG: Para inclusão no README.md"
echo "   - SVG: Gráficos vetoriais escaláveis"

echo "=========================================="
