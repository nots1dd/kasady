#pragma once
#include <vector>
#include <GL/gl.h>
#include <imgui.h>

struct DocumentImage {
    GLuint texture;
    int width, height;
};

class DocumentViewer {
public:
    void loadImages(const char* pattern, int count);
    void render();

private:
    std::vector<DocumentImage> images;
    int current_page = 0;
};
