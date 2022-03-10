#include "GL.hpp"
#include "Tools.hpp"
#ifndef __SERVER__
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#endif
#include <cmath>

double base_triangle_t::get_area(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3, double range) {
    double a = sqrt((v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[0] - v2[0]) * (v1[0] - v2[0])) + range;
    double b = sqrt((v2[1] - v3[1]) * (v2[1] - v3[1]) + (v2[0] - v3[0]) * (v2[0] - v3[0])) + range;
    double c = sqrt((v3[1] - v1[1]) * (v3[1] - v1[1]) + (v3[0] - v1[0]) * (v3[0] - v1[0])) + range;
    double s = (a + b + c) / 2.0;
    return sqrt(s * (s - a) * (s - b) * (s - c));
}

double base_triangle_t::get_area() { return get_area(vertices[0], vertices[1], vertices[2]); }

bool base_triangle_t::contains(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3, vertex_2d& vertex, double range) {
    return (
         get_area(v1, v2, vertex) +
         get_area(v2, v3, vertex) +
         get_area(v3, v1, vertex)
    ) <= get_area(v1, v2, v3, range);
}
bool base_triangle_t::contains(vertex_2d vertex) {
    return contains(vertices[0], vertices[1], vertices[2], vertex, 1.0);
}

vertex_2d& base_triangle_t::operator[](uint8_t id) { return vertices[limit(0_u8, id, 2_u8)]; }

vertex_2d& triangle_strip_t::operator[](size_t id) { return vertices[clamp(0_z, id, vertices.size())]; }

bool triangle_strip_t::contains(vertex_2d vertex, double range) {
    if(vertices.size() < 3) return false;

    for(size_t i = 0; i < vertices.size() - 2; i++)
        if(base_triangle_t::contains(vertices[i], vertices[i + 1], vertices[i + 2], vertex, range)) return true;
    
    return false;
}

double base_rect_t::get_area() {
    return base_triangle_t::get_area(vertices[0], vertices[1], vertices[2])
         + base_triangle_t::get_area(vertices[2], vertices[3], vertices[0]);
}

bool base_rect_t::contains(vertex_2d vertex) {
    return (
        base_triangle_t::get_area(vertices[0], vertices[1], vertex) +
        base_triangle_t::get_area(vertices[1], vertices[2], vertex) +
        base_triangle_t::get_area(vertices[2], vertices[3], vertex) +
        base_triangle_t::get_area(vertices[3], vertices[0], vertex)
    ) == get_area();
}

vertex_2d& base_rect_t::operator[](uint8_t id) { return vertices[limit(0_u8, id, 3_u8)]; }

#ifndef __SERVER__
void rect_t::render() {
    texture_t texture = { 0 };
    if(anim.frames.size() > 0) {
        if(anim.frames.size() == 1) texture = anim.frames[0];
        else {
            if(1000.0 / anim.frame_rate >= (sc::now() - anim.last_frame_time).count()) {
                anim.last_frame = limit(0, anim.last_frame, anim.frames.size() - 1);
            }
            texture = anim.frames[anim.last_frame];
        }
    }

    if(texture.textid) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.textid);
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);

        if(texture.textid) glTexCoord2d(0.0, 0.0); glVertex2d(vertices[0][0], vertices[0][1]);
        if(texture.textid) glTexCoord2d(1.0, 0.0); glVertex2d(vertices[1][0], vertices[1][1]);
        if(texture.textid) glTexCoord2d(1.0, 1.0); glVertex2d(vertices[2][0], vertices[2][1]);
        if(texture.textid) glTexCoord2d(0.0, 1.0); glVertex2d(vertices[3][0], vertices[3][1]);

    glEnd();

    if(texture.textid) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void rect_t::render(vertex_2d offset, double& rot) {
    glPushMatrix();

        glTranslated(offset.x, offset.y, 0.0);
        glRotated(rot, 0.0, 0.0, 1.0);
        render();

    glPopMatrix();
}
#endif

vertex_2d& line_strip_t::operator[](size_t id) { return vertices[clamp(0_z, id, vertices.size())]; }

vertex_2d line_strip_t::get_position_at(double d) {
    double offset = 0.0;
    for(auto i : iterate(vertices.size())) {
        if(i == vertices.size() - 1) return vertices[i];
        vertex_2d diff = vertices[i + 1] - vertices[i];
        double length = sqrt(diff.x * diff.x + diff.y * diff.y);
        if(length < d - offset) { offset += length; continue; }
        return diff * (d - offset) * (1.0 / length) + vertices[i];
    }
    return vertices[vertices.size() - 1];
}

#ifndef __SERVER__
void line_strip_t::render() {
    glBegin(GL_LINE_STRIP);

        for(auto&v : vertices) glVertex2d(v.x, v.y);

    glEnd();
}
#endif

double line_strip_t::distance(vertex_2d v) {
    double min = -1.0;

    for(size_t i = 1; i < vertices.size(); i++) {
        line_t l = { vertices[i - 1], vertices[i] };
        double d = l.distance(v);
        if(min == -1.0 || d < min) min = d;
    }

    return min;
}

double line_strip_t::length() {
    double d = 0.0;

    for(size_t i = 1; i < vertices.size(); i++) {
        line_t l = { vertices[i - 1], vertices[i] };
        d += l.length();
    }

    return d;
}

double line_t::distance(vertex_2d v) {
    vertex_2d ab = { v2.x - v1.x, v2.y - v1.y };
    vertex_2d av = { v .x - v1.x, v .y - v1.y };
    vertex_2d bv = { v .x - v2.x, v .y - v2.y };

    double ab_bv = ab.x * bv.x + ab.y * bv.y;
    double ab_av = ab.x * av.x + ab.y * av.y;
    
    if(ab_bv > 0.0) {
        double x = v.x - v2.x;
        double y = v.y - v2.y;
        return sqrt(x * x + y * y);
    }

    if(ab_av < 0.0) {
        double x = v.x - v1.x;
        double y = v.y - v1.y;
        return sqrt(x * x + y * y);
    }

    return abs(ab.x * av.y - ab.y * av.x) / sqrt(ab.x * ab.x + ab.y * ab.y);
}

double line_t::length() {
    double dx = v1.x - v2.x;
    double dy = v1.y - v2.y;
    return sqrt(dx * dx + dy * dy);
}

#ifndef __SERVER__
uint32_t render_circle(double radius, std::string fill_color, double stroke, std::string stroke_color) {
    uint32_t textureid = 0;
    std::string size = "\""_str + std::to_string((uint32_t)(radius * 2)) + '"';
    std::string r    = "\""_str + std::to_string((uint32_t)(radius    )) + '"';
    std::string svg_data =
          "<svg height="_str     + size
        + " width="              + size
        + "><circle cx="         + r
        + " cy="                 + r
        + " r="                  + r
        + " stroke=\""           + stroke_color           + '"'
        + " stroke-width=\""     + std::to_string(stroke) + '"'
        + " fill=\""             + fill_color             + '"'
        + "/></svg>"    ;
    SDL_RWops* mem = SDL_RWFromConstMem(svg_data.c_str(), (int)svg_data.length());
    if(!mem) return 0;

    SDL_Surface* surface = IMG_LoadSVG_RW(mem);

    SDL_RWclose(mem);
    if(!surface) return 0;

    int mode = GL_RGB;
    if(surface->format->BytesPerPixel == 4) {
        if(surface->format->Rmask == 0x000000ff)
            mode = GL_RGBA;
        else
            mode = GL_BGRA;
    } else {
        if(surface->format->Rmask == 0x000000ff)
            mode = GL_RGB;
        else
            mode = GL_BGR;
    }

    glGenTextures(1, &textureid);
    glBindTexture(GL_TEXTURE_2D, textureid);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(surface);
    
    return textureid;
}
#endif
