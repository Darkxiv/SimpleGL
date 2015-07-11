#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "tgaimage.h"
#include "model.h"

class Image
{
public:
	Image(int w, int h);

	void draw_line(glm::vec2 p0, glm::vec2 p1, TGAColor color);
	void draw_triangle_wire(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, TGAColor color);
	void draw_triangle_flat(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, TGAColor color);

	void draw_model_wire(const Model &model, const TGAColor &color);
	void draw_model(const Model &model, const TGAColor &color);

	void write_to_file(const std::string &fn);

private:
	TGAImage img;
	int width;
	int height;

	void draw_line(int x0, int y0, int x1, int y1, TGAColor color);
	int cross_line_y(const glm::vec2 &p0, const glm::vec2 &p1, int y);
};

#endif