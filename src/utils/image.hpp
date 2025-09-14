#ifndef IMAGE_H
#define IMAGE_H

#include <cairo/cairo.h>
#include <math.h>
#include <string>

#define DEFAULT_WIDTH 400
#define DEFAULT_HEIGHT 400

class Image {
public:
    Image(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
    ~Image();
    void generate_image(int counter, float flowRate, float maxFlowRate, std::string name) const;

private:
    cairo_surface_t* surface;
    cairo_t* cr;
    int width;
    int height;
};

#endif // IMAGE_H
