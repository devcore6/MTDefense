export module renderer.base.renderer;

import renderer.color_format.color_format;

import renderer.base.command_buffer;
import renderer.base.viewport;

import renderer.vertex;

import tools.types;

import <cstdint>;
import <vector>;

export class renderer {
protected:
    // C++ Doesn't allow templated virtual functions, which means I need some kind of wrapper
    // to keep the code clean. Luckily, this wrapper will only ever be used within the renderer
    // code and I won't ever have to deal with it again. Ideally I will find a better way of
    // doing this in the future cause I don't like this one bit.
    struct command_buffer_wrapper {
        const size_t               v_size;
        const size_t               v_count;
        const command_mode         mode;
        const color_format         format;
        std::vector<double>        positions;
        std::vector<unsigned char> colors;

        template<size_t vertex_size,
                 size_t vertex_count,
                 color_format col_format>
        command_buffer_wrapper(copy_t<command_buffer<vertex_size,
                                                     vertex_count,
                                                     col_format>> buffer) : v_size(vertex_size),
                                                                            v_count(vertex_count),
                                                                            mode(buffer.mode),
                                                                            positions(v_size * v_count),
                                                                            colors(col_format.size * v_count),
                                                                            format(col_format) {
            for(auto& vertex : buffer.vertices) {
                for(size_t i = 0; i <                           v_size; i++) positions.push_back(vertex.pos[i]);
                for(size_t i = 0; i < color_format_traits[format].size; i++) colors.push_back(vertex.colors[i]);
            }
        }
    };

public:
    virtual void clear() noexcept = 0;
    virtual void set_clear_color(float r, float g, float b, float a) = 0;
    virtual void set_viewport(viewport) = 0;

    virtual void send_command(command_buffer_wrapper) = 0;
};
