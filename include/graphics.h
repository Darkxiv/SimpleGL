#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "tgaimage.h"
#include "model.h"
#include <memory>

class Image
{
public:
	Image(int w, int h, int d);

	void draw_line(glm::ivec3 p0, glm::ivec3 p1, TGAColor color);
	void draw_triangle_wire(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color);
	void draw_triangle_flat(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color);
	void draw_triangle_texture(const std::vector<std::vector<glm::vec3>> &geom, float intensity, TGAImage &texture);

	void draw_model_wire(const Model &model, const TGAColor &color);
	void draw_model_texture(const Model &model, TGAImage &tex);

	void write_to_file(const std::string &fn);

	~Image();

private:
	TGAImage img;
	int width, height, depth;
	int *depth_buffer;

	void draw_line(int x0, int y0, int x1, int y1, TGAColor color);
	glm::ivec3 get_vert_cross_line_y(const glm::vec3 &p0, const glm::vec3 &p1, int y);
	glm::ivec3 get_uv_cross_line_y(const glm::vec3 &uv0, const glm::vec3 &uv1, const glm::vec3 &p0, const glm::vec3 &p1, int y);
};

#endif