#include "doc_viewer.h"
#include <GL/gl.h>
#include <cstdio>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <vector>
#include <memory>
#include <iostream>

void DocumentViewer::loadImages(const char* pdf_filename, int page_count) {
    std::unique_ptr<poppler::document> doc(poppler::document::load_from_file(pdf_filename));
    if (!doc) {
        fprintf(stderr, "Failed to load PDF document: %s\n", pdf_filename);
        return;
    }

    if (doc->is_locked()) {
        fprintf(stderr, "PDF document is encrypted: %s\n", pdf_filename);
        return;
    }

    int num_pages = doc->pages();
    if (page_count > num_pages) {
        page_count = num_pages;
    }

    poppler::page_renderer renderer;
    renderer.set_render_hint(poppler::page_renderer::antialiasing, true);
    renderer.set_render_hint(poppler::page_renderer::text_antialiasing, true);

    // Loop through the requested pages and render them
    for (int i = 0; i < page_count; ++i) {
        std::unique_ptr<poppler::page> page(doc->create_page(i));
        if (!page) {
            fprintf(stderr, "Failed to create page %d\n", i + 1);
            continue;
        }

        // Render the page into an image
        poppler::image img = renderer.render_page(page.get(), 800, 600);
        if (!img.is_valid()) {
            fprintf(stderr, "Failed to render page %d\n", i + 1);
            continue;
        }

        // Create OpenGL texture from the rendered image
        int width = img.width();
        int height = img.height();
        std::vector<unsigned char> buffer(img.bytes_per_row() * height);
        memcpy(buffer.data(), img.data(), buffer.size());

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        // Store the texture for rendering
        images.push_back({texture, width, height});
    }
}


void DocumentViewer::render() {
    if (images.empty()) {
        ImGui::Text("No images loaded!");
        return;
    }

    static float zoom = 1.0f;  // Zoom level
    ImGui::Begin("Document Viewer");

    // Display page and navigation controls
    ImGui::Text("Page %d/%d", current_page + 1, (int)images.size());
    if (ImGui::Button("Previous") && current_page > 0) current_page--;
    ImGui::SameLine();
    if (ImGui::Button("Next") && current_page < (int)images.size() - 1) current_page++;

    // Get the size of the ImGui window
    ImVec2 viewer_size = ImGui::GetContentRegionAvail(); // Available space in the viewer
    float viewer_width = viewer_size.x;
    float viewer_height = viewer_size.y;

    // Compute the "fit" zoom level
    auto& img = images[current_page];
    float fit_zoom_x = viewer_width / img.width;
    float fit_zoom_y = viewer_height / img.height;
    float fit_zoom = std::min(fit_zoom_x, fit_zoom_y); // Ensure the image fits both dimensions

    // Zoom controls
    ImGui::Text("Zoom: %.2fx", zoom);
    if (ImGui::Button("Zoom In")) zoom *= 1.1f;
    ImGui::SameLine();
    if (ImGui::Button("Zoom Out")) zoom *= 0.9f;
    ImGui::SameLine();
    if (ImGui::Button("Fit")) zoom = fit_zoom;

    // Render the current page
    float display_width = img.width * zoom;
    float display_height = img.height * zoom;
    ImGui::Image((ImTextureID)img.texture, ImVec2(display_width, display_height));

    ImGui::End();
}

