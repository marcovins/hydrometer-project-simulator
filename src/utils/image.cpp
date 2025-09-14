#include "image.hpp"
#include <string>
#include <iostream>
#include <iomanip>

Image::Image(int width, int height) {
    this->width = width;
    this->height = height;
    this->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, this->width, this->height);
    this->cr = cairo_create(this->surface);
}

Image::~Image() {
    if (this->cr) {
        cairo_destroy(this->cr);
        this->cr = nullptr;
    }
    if (this->surface) {
        cairo_surface_destroy(this->surface);
        this->surface = nullptr;
    }
}

void Image::generate_image(int counter, float flowRate, float maxFlowRate, std::string name) const {
    // Log apenas a cada 5 atualizações para não poluir o output
    static int callCount = 0;
    callCount++;

    // Calcula escala dinâmica baseada na vazão máxima
    float maxFlowRate_m3h = maxFlowRate * 3600.0f; // Converte para m³/h
    // Arredonda para cima para o próximo múltiplo de 5 para uma escala limpa
    int scaleMax = ((int)(maxFlowRate_m3h + 4) / 5) * 5;
    if (scaleMax < 10) scaleMax = 10; // Mínimo de 10 m³/h para legibilidade
    
    // Calcula incrementos da escala
    int majorStep = scaleMax / 5; // 5 marcações principais
    int minorStep = majorStep / 2; // Marcações menores entre as principais

    // Fundo gradiente (simulando ambiente)
    cairo_pattern_t *gradient = cairo_pattern_create_radial(this->width/2, this->height/2, 0, 
                                                           this->width/2, this->height/2, this->width);
    cairo_pattern_add_color_stop_rgb(gradient, 0, 0.95, 0.95, 0.98);  // Centro claro
    cairo_pattern_add_color_stop_rgb(gradient, 1, 0.85, 0.85, 0.90);  // Bordas mais escuras
    cairo_set_source(this->cr, gradient);
    cairo_paint(this->cr);
    cairo_pattern_destroy(gradient);

    double centerX = this->width/2;
    double centerY = this->height/2;
    double mainRadius = 160;
    
    // Corpo metálico externo (anel exterior)
    cairo_pattern_t *metalGradient = cairo_pattern_create_linear(centerX-mainRadius, centerY-mainRadius, 
                                                                centerX+mainRadius, centerY+mainRadius);
    cairo_pattern_add_color_stop_rgb(metalGradient, 0, 0.8, 0.8, 0.85);   // Metal claro
    cairo_pattern_add_color_stop_rgb(metalGradient, 0.5, 0.6, 0.6, 0.65); // Metal médio
    cairo_pattern_add_color_stop_rgb(metalGradient, 1, 0.4, 0.4, 0.45);   // Metal escuro
    cairo_set_source(this->cr, metalGradient);
    cairo_arc(this->cr, centerX, centerY, mainRadius, 0, 2*M_PI);
    cairo_fill(this->cr);
    cairo_pattern_destroy(metalGradient);

    // Anel interno metálico
    cairo_set_source_rgb(this->cr, 0.3, 0.3, 0.35);
    cairo_arc(this->cr, centerX, centerY, mainRadius-5, 0, 2*M_PI);
    cairo_set_line_width(this->cr, 3);
    cairo_stroke(this->cr);

    // Fundo do mostrador (branco com leve gradiente)
    cairo_pattern_t *dialGradient = cairo_pattern_create_radial(centerX, centerY, 0, centerX, centerY, 145);
    cairo_pattern_add_color_stop_rgb(dialGradient, 0, 1.0, 1.0, 1.0);     // Centro branco
    cairo_pattern_add_color_stop_rgb(dialGradient, 1, 0.92, 0.92, 0.95);  // Bordas levemente azuladas
    cairo_set_source(this->cr, dialGradient);
    cairo_arc(this->cr, centerX, centerY, 145, 0, 2*M_PI);
    cairo_fill(this->cr);
    cairo_pattern_destroy(dialGradient);

    // Borda do mostrador
    cairo_set_source_rgb(this->cr, 0.2, 0.2, 0.25);
    cairo_arc(this->cr, centerX, centerY, 145, 0, 2*M_PI);
    cairo_set_line_width(this->cr, 2);
    cairo_stroke(this->cr);

    // Escala principal com números (dinâmica baseada na vazão máxima)
    cairo_set_source_rgb(this->cr, 0.1, 0.1, 0.15);
    cairo_set_line_width(this->cr, 3);
    cairo_select_font_face(this->cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(this->cr, 12);
    
    // Marcações principais cobrindo 270° (de -90° a +180°) - escala dinâmica
    for (int i = 0; i <= scaleMax; i += majorStep) {
        double angle = -M_PI/2 + (i * M_PI * 1.5 / scaleMax); // Distribui ao longo de 270°
        
        // Marcações principais
        double x1 = centerX + 135 * cos(angle);
        double y1 = centerY + 135 * sin(angle);
        double x2 = centerX + 120 * cos(angle);
        double y2 = centerY + 120 * sin(angle);
        
        cairo_move_to(this->cr, x1, y1);
        cairo_line_to(this->cr, x2, y2);
        cairo_stroke(this->cr);
        
        // Números na escala dinâmica
        std::string numText = std::to_string(i);
        cairo_text_extents_t extents;
        cairo_text_extents(this->cr, numText.c_str(), &extents);
        double textX = centerX + 110 * cos(angle) - extents.width/2;
        double textY = centerY + 110 * sin(angle) + extents.height/2;
        cairo_move_to(this->cr, textX, textY);
        cairo_show_text(this->cr, numText.c_str());
    }
    
    // Marcações secundárias (incrementos menores)
    cairo_set_line_width(this->cr, 1.5);
    for (int i = minorStep; i < scaleMax; i += majorStep) {
        double angle = -M_PI/2 + (i * M_PI * 1.5 / scaleMax);
        double x1 = centerX + 135 * cos(angle);
        double y1 = centerY + 135 * sin(angle);
        double x2 = centerX + 127 * cos(angle);
        double y2 = centerY + 127 * sin(angle);
        
        cairo_move_to(this->cr, x1, y1);
        cairo_line_to(this->cr, x2, y2);
        cairo_stroke(this->cr);
    }

    // Ponteiro principal (baseado na vazão)
    // Converte a vazão para m³/h para normalização proporcional
    float flowRate_m3h = flowRate * 3600.0f; // m³/s para m³/h
    // Normaliza com base na escala dinâmica
    double normalizedFlow = flowRate_m3h / scaleMax; // Escala dinâmica
    if (normalizedFlow > 1.0) normalizedFlow = 1.0; // Limita ao máximo
    if (normalizedFlow < 0.0) normalizedFlow = 0.0; // Limita ao mínimo
    double pointerAngle = -M_PI/2 + (normalizedFlow * M_PI * 1.5); // 270° de rotação
    
    // Sombra do ponteiro
    cairo_set_source_rgba(this->cr, 0, 0, 0, 0.3);
    cairo_set_line_width(this->cr, 6);
    cairo_move_to(this->cr, centerX+2, centerY+2);
    cairo_line_to(this->cr, centerX+2 + 90*cos(pointerAngle), centerY+2 + 90*sin(pointerAngle));
    cairo_stroke(this->cr);
    
    // Ponteiro principal (formato de agulha)
    cairo_set_source_rgb(this->cr, 0.8, 0.1, 0.1); // Vermelho escuro
    cairo_set_line_width(this->cr, 4);
    
    // Desenha ponteiro como triângulo
    double tipX = centerX + 85 * cos(pointerAngle);
    double tipY = centerY + 85 * sin(pointerAngle);
    double baseAngle1 = pointerAngle + M_PI/2;
    double baseAngle2 = pointerAngle - M_PI/2;
    double baseX1 = centerX + 8 * cos(baseAngle1);
    double baseY1 = centerY + 8 * sin(baseAngle1);
    double baseX2 = centerX + 8 * cos(baseAngle2);
    double baseY2 = centerY + 8 * sin(baseAngle2);
    
    cairo_move_to(this->cr, tipX, tipY);
    cairo_line_to(this->cr, baseX1, baseY1);
    cairo_line_to(this->cr, centerX, centerY);
    cairo_line_to(this->cr, baseX2, baseY2);
    cairo_close_path(this->cr);
    cairo_fill(this->cr);
    
    // Contorno do ponteiro
    cairo_set_source_rgb(this->cr, 0.5, 0.05, 0.05);
    cairo_set_line_width(this->cr, 1);
    cairo_move_to(this->cr, tipX, tipY);
    cairo_line_to(this->cr, baseX1, baseY1);
    cairo_line_to(this->cr, centerX, centerY);
    cairo_line_to(this->cr, baseX2, baseY2);
    cairo_close_path(this->cr);
    cairo_stroke(this->cr);

    // Centro do ponteiro (parafuso)
    cairo_pattern_t *centerGradient = cairo_pattern_create_radial(centerX-2, centerY-2, 0, centerX, centerY, 12);
    cairo_pattern_add_color_stop_rgb(centerGradient, 0, 0.9, 0.9, 0.95);  // Brilho
    cairo_pattern_add_color_stop_rgb(centerGradient, 1, 0.4, 0.4, 0.45);  // Sombra
    cairo_set_source(this->cr, centerGradient);
    cairo_arc(this->cr, centerX, centerY, 12, 0, 2*M_PI);
    cairo_fill(this->cr);
    cairo_pattern_destroy(centerGradient);
    
    // Parafuso central (detalhes)
    cairo_set_source_rgb(this->cr, 0.2, 0.2, 0.25);
    cairo_arc(this->cr, centerX, centerY, 8, 0, 2*M_PI);
    cairo_set_line_width(this->cr, 1);
    cairo_stroke(this->cr);
    
    // Cruz do parafuso
    cairo_move_to(this->cr, centerX-4, centerY);
    cairo_line_to(this->cr, centerX+4, centerY);
    cairo_move_to(this->cr, centerX, centerY-4);
    cairo_line_to(this->cr, centerX, centerY+4);
    cairo_stroke(this->cr);

    // Display digital no canto inferior
    cairo_pattern_t *displayBg = cairo_pattern_create_linear(centerX-100, this->height-100, centerX+100, this->height-60);
    cairo_pattern_add_color_stop_rgb(displayBg, 0, 0.05, 0.05, 0.05);  // Preto
    cairo_pattern_add_color_stop_rgb(displayBg, 1, 0.15, 0.15, 0.15);  // Cinza escuro
    cairo_set_source(this->cr, displayBg);
    cairo_rectangle(this->cr, centerX-100, this->height-100, 200, 40);
    cairo_fill(this->cr);
    cairo_pattern_destroy(displayBg);
    
    // Contorno do display
    cairo_set_source_rgb(this->cr, 0.4, 0.4, 0.4);
    cairo_rectangle(this->cr, centerX-100, this->height-100, 200, 40);
    cairo_set_line_width(this->cr, 2);
    cairo_stroke(this->cr);
    
    // Texto do display (volume acumulado)
    cairo_set_source_rgb(this->cr, 0.0, 0.8, 0.0); // Verde LED
    cairo_select_font_face(this->cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(this->cr, 14);
    
    char volumeDisplay[25];
    // Formato decimal com 6 dígitos e zeros à esquerda
    snprintf(volumeDisplay, sizeof(volumeDisplay), "%06d m³", counter);
    cairo_text_extents_t displayExtents;
    cairo_text_extents(this->cr, volumeDisplay, &displayExtents);
    cairo_move_to(this->cr, centerX - displayExtents.width/2, this->height - 85);
    cairo_show_text(this->cr, volumeDisplay);
    
    // Vazão atual no display
    cairo_set_source_rgb(this->cr, 0.0, 0.6, 1.0); // Azul LED
    cairo_set_font_size(this->cr, 12);
    
    char flowDisplay[25];
    // Usa a variável flowRate_m3h já declarada anteriormente
    snprintf(flowDisplay, sizeof(flowDisplay), "%.4f m³/h", flowRate_m3h);
    cairo_text_extents_t flowExtents;
    cairo_text_extents(this->cr, flowDisplay, &flowExtents);
    cairo_move_to(this->cr, centerX - flowExtents.width/2, this->height - 65);
    cairo_show_text(this->cr, flowDisplay);
    
    // Informações da marca/modelo
    cairo_set_source_rgb(this->cr, 0.3, 0.3, 0.3);
    cairo_select_font_face(this->cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(this->cr, 12);
    
    std::string brandText = "HYDROTECH PRO";
    cairo_text_extents_t brandExtents;
    cairo_text_extents(this->cr, brandText.c_str(), &brandExtents);
    cairo_move_to(this->cr, centerX - brandExtents.width/2, centerY + 50);
    cairo_show_text(this->cr, brandText.c_str());
    
    std::string modelText = "Modelo HT-2024";
    cairo_text_extents_t modelExtents;
    cairo_text_extents(this->cr, modelText.c_str(), &modelExtents);
    cairo_move_to(this->cr, centerX - modelExtents.width/2, centerY + 70);
    cairo_show_text(this->cr, modelText.c_str());
    
    // Unidade de medida
    cairo_set_source_rgb(this->cr, 0.4, 0.4, 0.4);
    cairo_set_font_size(this->cr, 10);
    std::string unitText = "m³/h";
    cairo_text_extents_t unitExtents;
    cairo_text_extents(this->cr, unitText.c_str(), &unitExtents);
    cairo_move_to(this->cr, centerX - unitExtents.width/2, centerY - 60);
    cairo_show_text(this->cr, unitText.c_str());
    
    // Indicador de vazão atual no mostrador
    cairo_set_source_rgb(this->cr, 0.0, 0.4, 0.8);
    cairo_set_font_size(this->cr, 12);
    char flowIndicator[30];
    // Usa a variável flowRate_m3h já declarada anteriormente
    snprintf(flowIndicator, sizeof(flowIndicator), "%.4f m³/h", flowRate_m3h);
    cairo_text_extents_t flowIndicatorExtents;
    cairo_text_extents(this->cr, flowIndicator, &flowIndicatorExtents);
    cairo_move_to(this->cr, centerX - flowIndicatorExtents.width/2, centerY + 25);
    cairo_show_text(this->cr, flowIndicator);
    
    // Título do equipamento
    cairo_set_font_size(this->cr, 14);
    cairo_set_source_rgb(this->cr, 0.2, 0.2, 0.4);
    const char* title = "HIDRÔMETRO RESIDENCIAL";
    cairo_text_extents_t titleExtents;
    cairo_text_extents(this->cr, title, &titleExtents);
    cairo_move_to(this->cr, centerX - titleExtents.width/2, 30);
    cairo_show_text(this->cr, title);

    // Salva a imagem
    cairo_surface_write_to_png(this->surface, name.c_str());
}
