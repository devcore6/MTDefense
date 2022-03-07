#include <cxxgui/cxxgui.hpp>
#include "../Src/Game/Game.hpp"
#include "../Src/Engine/Tools.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

struct vertex_2d {
	double x = 0;
	double y = 0;
};

struct line_strip_t {
	std::vector<vertex_2d> vertices;
	void render(float r, float g, float b, float a) {
		glColor4f(r, g, b, a);
		glBegin(GL_LINE_STRIP);
			for(auto& vertex : vertices) glVertex2d(vertex.x, vertex.y);
		glEnd();
	}
};

struct triangle_strip_t {
	std::vector<vertex_2d> vertices;
	void render(float r, float g, float b, float a) {
		glColor4f(r, g, b, a);
		glBegin(GL_TRIANGLE_STRIP);
			for(auto& vertex : vertices) glVertex2d(vertex.x, vertex.y);
		glEnd();
	}
};

using namespace cxxgui;

enum {
	MODE_NONE = 0_u8,
	MODE_ADD_ROUTE,
	MODE_ADD_WATER,
	MODE_ADD_CLIP
};

enum {
	MAP_BEGINNER = 0,
	MAP_INTERMEDIATE,
	MAP_ADVANCED,
	MAP_EXPERT,
	MAP_IMPOSSIBLE
};

uint8_t difficulty = (uint8_t)MAP_BEGINNER;
uint8_t current_mode = MODE_NONE;
std::vector<vertex_2d> cur_vertices;

std::vector<line_strip_t> routes;
std::vector<triangle_strip_t> water;
std::vector<triangle_strip_t> clipping_areas;

bool update_function(window_t* w, void* data) {
	for(auto& route : routes) route.render(0.0f, 0.0f, 0.0f, 1.0f);
	for(auto& water_area : water) water_area.render(0.2f, 0.3f, 0.8f, 0.75f);
	for(auto& clip : clipping_areas) clip.render(0.8f, 0.2f, 0.2f, 0.75f);
	int x = 0, y = 0;
	SDL_GetMouseState(&x, &y);
	switch(current_mode) {
		case MODE_ADD_ROUTE: {
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			glBegin(GL_LINE_STRIP);
				for(auto& vertex : cur_vertices) glVertex2d(vertex.x, vertex.y);
				glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
				glVertex2i(x, y);
			glEnd();
			break;
		}
		case MODE_ADD_WATER: {
			glColor4f(0.2f, 0.3f, 0.8f, 0.75f);
			glBegin(GL_TRIANGLE_STRIP);
				for(auto& vertex : cur_vertices) glVertex2d(vertex.x, vertex.y);
				glColor4f(0.2f, 0.3f, 0.8f, 0.375f);
				glVertex2i(x, y);
			glEnd();
			break;
		}
		case MODE_ADD_CLIP: {
			glColor4f(0.8f, 0.2f, 0.2f, 0.75f);
			glBegin(GL_TRIANGLE_STRIP);
				for(auto& vertex : cur_vertices) glVertex2d(vertex.x, vertex.y);
				glColor4f(0.8f, 0.2f, 0.2f, 0.375f);
				glVertex2i(x, y);
			glEnd();
			break;
		}
		case MODE_NONE:
		default: break;
	}
	return true;
}

class contentview: public view {
private:
	text* difficulties[5] = {
		new text("Map difficulty: Beginner"),
		new text("Map difficulty: Intermediate"),
		new text("Map difficulty: Advanced"),
		new text("Map difficulty: Expert"),
		new text("Map difficulty: Impossible")
	};

public:
	contentview(std::string& path) {
		body = new hstack {
			(new image(path.c_str()))
				->on_click([](view*, float x, float y, void*) { if(current_mode != MODE_NONE) cur_vertices.push_back({ x, y - 32 }); }),
			(new vstack {
				(new text("Add enemy route"))
					->font(headline)
					->padding(16, 0)
					->hover_foreground_color(color::accent_color)
					->on_click([](view*, float, float, void*) {
						switch(current_mode) {
							case MODE_ADD_ROUTE: {
								routes.push_back({ cur_vertices });
								break;
							}
							case MODE_ADD_WATER: {
								water.push_back({ cur_vertices });
								break;
							}
							case MODE_ADD_CLIP: {
								clipping_areas.push_back({ cur_vertices });
								break;
							}
							case MODE_NONE:
							default: break;
						}
						current_mode = MODE_ADD_ROUTE;
						cur_vertices.clear();
					}),
				(new text("Add new water area"))
					->font(headline)
					->padding(16, 0)
					->hover_foreground_color(color::accent_color)
					->on_click([](view*, float, float, void*) {
						switch(current_mode) {
							case MODE_ADD_ROUTE: {
								routes.push_back({ cur_vertices });
								break;
							}
							case MODE_ADD_WATER: {
								water.push_back({ cur_vertices });
								break;
							}
							case MODE_ADD_CLIP: {
								clipping_areas.push_back({ cur_vertices });
								break;
							}
							case MODE_NONE:
							default: break;
						}
						current_mode = MODE_ADD_WATER;
						cur_vertices.clear();
					}),
				(new text("Add new clipping area"))
					->font(headline)
					->padding(16, 0)
					->hover_foreground_color(color::accent_color)
					->on_click([](view*, float, float, void*) {
						switch(current_mode) {
							case MODE_ADD_ROUTE: {
								routes.push_back({ cur_vertices });
								break;
							}
							case MODE_ADD_WATER: {
								water.push_back({ cur_vertices });
								break;
							}
							case MODE_ADD_CLIP: {
								clipping_areas.push_back({ cur_vertices });
								break;
							}
							case MODE_NONE:
							default: break;
						}
						current_mode = MODE_ADD_CLIP;
						cur_vertices.clear();
					}),
				// todo: difficulty selector
				(new text("Export"))
					->font(title)
					->padding(16, 0)
					->hover_foreground_color(color::accent_color)
					->on_click([path](view*, float, float, void*) {
						std::ofstream output;
						output.open("maptool-output.bin", std::ios::out | std::ios::binary);
						if(!output.is_open()) {
							std::cerr << "Fatal error: Couldn't open output file!" << std::endl;
							exit(EXIT_FAILURE);
						}

						output << difficulty;

						const auto _loop = []<typename T>(std::vector<T>& vec, std::ofstream& output) {
							uint32_t size = (uint32_t)vec.size();
							output.write((const char*)&size, sizeof(uint32_t));

							for(auto& x : vec) {
								size = (uint32_t)x.vertices.size();
								output.write((const char*)&size, sizeof(uint32_t));
								for(auto& vertex : x.vertices) {
									output.write((const char*)&vertex.x, sizeof(double));
									output.write((const char*)&vertex.y, sizeof(double));
								}
							}
						};

						_loop(routes, output);
						_loop(water, output);
						_loop(clipping_areas, output);

						std::ifstream image;
						image.open(path, std::ios::in | std::ios::binary);
						if(!image.is_open()) {
							std::cerr << "Fatal error: Couldn't load map image!" << std::endl;
							output.close();
							std::filesystem::remove("maptool-output.bin");
							exit(EXIT_FAILURE);
						}

						output << image.rdbuf();

						std::cout << "Map saved successfully!" << std::endl;
						output.close();
						exit(EXIT_SUCCESS);
					})
			})
			->frame(300, 300, 300, 0, 0, 0, alignment_t::center)
		};
	}
};

int main(int argc, const char** argv) {
	std::string path = "";
	if(argc == 1) {
		std::cout << "Enter path to map image or drag and drop image onto this window: ";
		std::getline(std::cin, path);
		erase_all(path, "\""_str);
	} else {
		path = argv[1];
	}

	app test;
	test.title = GAME_NAME " MapTool";
	test.body = new contentview(path);
	test.post_loop = update_function;
	test.main();

    return 0;
}
