#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "tgaimage.h"
#include "model.h"
#include <memory>

class Canvas
{
public:
	enum Render_mode { MODE_WIRE, MODE_FLAT };

	Canvas(int w, int h, int d);

	void draw_line(glm::ivec3 p0, glm::ivec3 p1, TGAColor color);
	void draw_triangle_wire(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color);
	void draw_triangle_flat(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color);
	void draw_triangle_texture(Face &geom);

	void draw_model(const Model &model);
	void bind_diffuse_texture(const TGAImage &texture);

	void set_mode(const Render_mode &draw_mode);
	void set_color(const TGAColor &color);
	void set_light(const glm::vec3 &light_direction);

	void set_render_settings(unsigned int flags);
	void set_shadeless(const bool flag);
	void set_normal_mapping(const bool flag);
	void set_texture_mapping(const bool flag);
	void set_depth_buffer(const bool flag); // TODO rename

	void write_to_file(const std::string &fn);
	~Canvas();

private:
	// general
	TGAImage img;
	TGAColor col;
	Render_mode mode;
	unsigned int render_flags;

	// texture
	TGAImage diffuse;
	unsigned int texture_flags;

	// light
	glm::vec3 light_dir;

	// other
	int width, height, depth;
	int *depth_buffer;

	void draw_model_wire(const Model &model);
	void draw_model_texture(const Model &model);

	void draw_line(int x0, int y0, int x1, int y1, TGAColor color);
	glm::ivec3 get_vert_cross_line_y(const glm::vec3 &p0, const glm::vec3 &p1, int y);
};

#endif