#include "GL.hpp"
#include "Tools.hpp"
#include <SDL/SDL_opengl.h>

vertex_2d vertex_2d::operator+(vertex_2d& rhs) { return vertex_2d { x + rhs.x , y + rhs.y }; }
vertex_2d vertex_2d::operator-(vertex_2d& rhs) { return vertex_2d { x - rhs.x , y - rhs.y }; }

vertex_2d& vertex_2d::operator+=(vertex_2d& rhs) { x += rhs.x; y += rhs.y; return *this; }
vertex_2d& vertex_2d::operator-=(vertex_2d& rhs) { x -= rhs.x; y -= rhs.y; return *this; }

double& vertex_2d::operator[](uint8_t id) { return !(id % 2) ? x : y; }

double base_triangle_t::get_area(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3) {
    double a = sqrt((v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[0] - v2[0]) * (v1[0] - v2[0]));
    double b = sqrt((v2[1] - v3[1]) * (v2[1] - v3[1]) + (v2[0] - v3[0]) * (v2[0] - v3[0]));
    double c = sqrt((v3[1] - v1[1]) * (v3[1] - v1[1]) + (v3[0] - v1[0]) * (v3[0] - v1[0]));
    double s = (a + b + c) / 2.0;
    return sqrt(s * (s - a) * (s - b) * (s - c));
}

double base_triangle_t::get_area() { return get_area(vertices[0], vertices[1], vertices[2]); }

bool base_triangle_t::contains(vertex_2d& v1, vertex_2d& v2, vertex_2d& v3, vertex_2d& vertex) {
    return (
         get_area(v1, v2, vertex) +
         get_area(v2, v3, vertex) +
         get_area(v3, v1, vertex)
    ) == get_area(v1, v2, v3);
}
bool base_triangle_t::contains(vertex_2d& vertex) {
    return contains(vertices[0], vertices[1], vertices[2], vertex);
}

vertex_2d& base_triangle_t::operator[](uint8_t id) { return vertices[limit(0_u8, id, 2_u8)]; }

bool triangle_strip_t::contains(vertex_2d& vertex) {
    if(vertices.size() < 3) return false;

    for(size_t i = 0; i < vertices.size() - 2; i++)
        if(base_triangle_t::contains(vertices[i], vertices[i + 1], vertices[i + 2], vertex)) return true;
    
    return false;
}

double base_rect_t::get_area() {
    return base_triangle_t::get_area(vertices[0], vertices[1], vertices[2])
         + base_triangle_t::get_area(vertices[2], vertices[3], vertices[0]);
}

bool base_rect_t::contains(vertex_2d& vertex) {
    return (
        base_triangle_t::get_area(vertices[0], vertices[1], vertex) +
        base_triangle_t::get_area(vertices[1], vertices[2], vertex) +
        base_triangle_t::get_area(vertices[2], vertices[3], vertex) +
        base_triangle_t::get_area(vertices[3], vertices[0], vertex)
    ) == get_area();
}

vertex_2d& base_rect_t::operator[](uint8_t id) { return vertices[limit(0_u8, id, 3_u8)]; }

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

        if(texture.textid) glTexCoord2d(0.0, 1.0); glVertex2d(vertices[0][0], vertices[0][1]);
        if(texture.textid) glTexCoord2d(1.0, 1.0); glVertex2d(vertices[1][0], vertices[1][1]);
        if(texture.textid) glTexCoord2d(1.0, 0.0); glVertex2d(vertices[2][0], vertices[2][1]);
        if(texture.textid) glTexCoord2d(0.0, 0.0); glVertex2d(vertices[3][0], vertices[3][1]);

    glEnd();

    if(texture.textid) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void rect_t::render(vertex_2d& offset, double& rot) {
    glPushMatrix();

        glTranslated(offset.x, offset.y, 0.0);
        glRotated(rot, 0.0, 0.0, 1.0);
        render();

    glPopMatrix();
}
