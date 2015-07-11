#include "graphics.h"
#include "settings.h"
#include "tgaimage.h"
#include "model.h"
#include <glm/glm.hpp>
#include <algorithm>

Image::Image(int w, int h) : width(w), height(h), img(w, h, TGAImage::RGB)
{

}

void Image::draw_line(int x0, int y0, int x1, int y1, TGAColor color)
{
	bool revers = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		revers = true;
	}

	if (x1 < x0)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	if (!(x1 - x0)) {
		img.set(x0, y0, color);
		return;
	}

	int dx = std::abs(x1 - x0);
	int dy = std::abs(y1 - y0);
	int s = dy * 2;
	int e = 0.0;
	bool sign = y1 > y0;
	int y = y0;
	for (int x = x0; x <= x1; x++)
	{
		if (revers)
			img.set(y, x, color);
		else
			img.set(x, y, color);

		e += s;
		if (e > dx)
		{
			y += sign ? 1 : -1;
			e -= dx * 2;
		}
	}
}

void Image::draw_line(glm::vec2 p0, glm::vec2 p1, TGAColor color)
{
	draw_line(p0.x, p0.y, p1.x, p1.y, color);
}

void Image::draw_triangle_wire(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, TGAColor color)
{
	draw_line(p0, p1, color);
	draw_line(p1, p2, color);
	draw_line(p2, p0, color);
}

int Image::cross_line_y(const glm::vec2 &p0, const glm::vec2 &p1, int y) {
	int x;
	if (std::round(p0.y - p1.y) != 0)
		x = std::round((p0.x - p1.x) / (p0.y - p1.y) * (y - p0.y) + p0.x);
	else
		x = std::round(std::min(p0.x, p1.x));
	return x;
}

void Image::draw_triangle_flat(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, TGAColor color)
{
	if (p0.y > p1.y) std::swap(p0, p1);
	if (p0.y > p2.y) std::swap(p0, p2);
	if (p1.y > p2.y) std::swap(p1, p2);

	for (int y = p0.y; y <= p2.y; y++) {
		int x1 = cross_line_y(p0, p2, y);
		int x2 = y < p1.y ? cross_line_y(p0, p1, y) : cross_line_y(p1, p2, y);
		
		if (x1 > x2) std::swap(x1, x2);
		for (int x = x1; x <= x2; x++)
			img.set(x, y, color);
	}

	draw_triangle_wire(p0, p1, p2, color);
}

void Image::draw_model_wire(const Model &model, const TGAColor &color)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<int> face = model.face(i);
		for (int j = 0; j < 3; j++) {
			glm::vec3 v0 = model.vert(face[j]);
			glm::vec3 v1 = model.vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.0f)*width / 2.0f;
			int y0 = (v0.y + 1.0f)*height / 2.0f;
			int x1 = (v1.x + 1.0f)*width / 2.0f;
			int y1 = (v1.y + 1.0f)*height / 2.0f;
			draw_line(x0, y0, x1, y1, color);
		}
	}
}

void Image::draw_model(const Model &model, const TGAColor &color)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<int> face = model.face(i);
		std::vector<glm::vec2> points;
		for (int j = 0; j < 3; j++) {
			glm::vec3 v = model.vert(face[j]);
			int x = (v.x + 1.0f)*width / 2.0f;
			int y = (v.y + 1.0f)*height / 2.0f;
			points.push_back(glm::vec2(x, y));
		}

		draw_triangle_flat(points[0], points[1], points[2], TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}
}

void Image::write_to_file(const std::string &fn)
{
	img.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	img.write_tga_file(fn.c_str());
	img.flip_vertically();
}
