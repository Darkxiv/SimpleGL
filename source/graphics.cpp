#include "graphics.h"
#include "settings.h"
#include "tgaimage.h"
#include "model.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <stdio.h>

Image::Image(int w, int h, int d) : width(w), height(h), depth(d), img(w, h, TGAImage::RGB)
{
	if (w > 0 && h > 0)
		depth_buffer = new int[w * h]();
	else
		depth_buffer = NULL;
}

Image::~Image()
{
	if (depth_buffer) 
		delete[] depth_buffer;
}

void Image::draw_line(int x0, int y0, int x1, int y1, TGAColor color)
{
	bool revers = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		revers = true;
	}

	if (x1 < x0) {
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
	for (int x = x0; x <= x1; x++) {
		if (revers)
			img.set(y, x, color);
		else
			img.set(x, y, color);

		e += s;
		if (e > dx) {
			y += sign ? 1 : -1;
			e -= dx * 2;
		}
	}
}

void Image::draw_line(glm::ivec3 p0, glm::ivec3 p1, TGAColor color)
{
	draw_line(p0.x, p0.y, p1.x, p1.y, color);
}

void Image::draw_triangle_wire(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color)
{
	draw_line(p0, p1, color);
	draw_line(p1, p2, color);
	draw_line(p2, p0, color);
}

glm::ivec3 Image::get_vert_cross_line_y(const glm::vec3 &p0, const glm::vec3 &p1, int y) {
	glm::ivec3 p;

	p.y = y;
	if (std::round(p0.y - p1.y) != 0) {
		p.x = std::round((p1.x - p0.x) * (float(y - p0.y) / (p1.y - p0.y)) + p0.x);
		p.z = std::round((p1.z - p0.z) * (float(y - p0.y) / (p1.y - p0.y)) + p0.z);
	}
	else {
		if (p0.x < p1.x) {
			p.x = p0.x;
			p.z = p0.z;
		}
		else {
			p.x = p1.x;
			p.z = p1.z;
		}
	}

	return p;
}

glm::ivec3 Image::get_uv_cross_line_y(const glm::vec3 &uv0, const glm::vec3 &uv1, const glm::vec3 &p0, const glm::vec3 &p1, int y)
{
	glm::vec3 p;

	if (std::round(p0.y - p1.y) != 0) {
		p.x = std::round((uv1.x - uv0.x) * (float(y - p0.y) / (p1.y - p0.y)) + uv0.x);
		p.y = std::round((uv1.y - uv0.y) * (float(y - p0.y) / (p1.y - p0.y)) + uv0.y);
	}
	else {
		p.x = p0.x < p1.x ? uv0.x : uv1.x;
		p.y = p0.x < p1.x ? uv0.y : uv1.y;
	}

	return p;
}

void Image::draw_triangle_flat(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color)
{
	if (p0.y > p1.y) std::swap(p0, p1);
	if (p0.y > p2.y) std::swap(p0, p2);
	if (p1.y > p2.y) std::swap(p1, p2);

	if (std::round(p2.y - p0.y) == 0) {
		int minx = p0.x < p1.x ? p0.x < p2.x ? p0.x : p2.x : p1.x < p2.x ? p1.x : p2.x;
		int maxx = p0.x > p1.x ? p0.x > p2.x ? p0.x : p2.x : p1.x > p2.x ? p1.x : p2.x;
		for (int x = minx; x < maxx; x++)
			img.set(x, p0.y, color);
		return;
	}

	for (int y = p0.y; y <= p2.y; y++) {
		glm::ivec3 fp = get_vert_cross_line_y(p0, p2, y);
		glm::ivec3 lp = y < p1.y ? get_vert_cross_line_y(p0, p1, y) : get_vert_cross_line_y(p1, p2, y);
		
		if (fp.x > lp.x)
			std::swap(fp, lp);

		for (int x = fp.x; x <= lp.x; x++) {
			int z = 1;
			if (lp.x != fp.x)
				z = float(x - fp.x) / (lp.x - fp.x) * (lp.z - fp.z) + fp.z;
			if (x >= 0 && y >= 0 && x < width && y < height && depth_buffer && z > depth_buffer[y * width + x]) {
				depth_buffer[y * width + x] = z;
				img.set(x, y, color);
			}
		}
	}
}

void Image::draw_triangle_texture(const std::vector<std::vector<glm::vec3>> &geom, float intensity, TGAImage &tex)
{
	glm::ivec3 p0 = geom[Model::VERTEX_LAYER][0], p1 = geom[Model::VERTEX_LAYER][1], p2 = geom[Model::VERTEX_LAYER][2];
	glm::ivec3 uv0 = geom[Model::TEXCO_LAYER][0], uv1 = geom[Model::TEXCO_LAYER][1], uv2 = geom[Model::TEXCO_LAYER][2];

	if (p0.y > p1.y) { std::swap(p0, p1); std::swap(uv0, uv1); }
	if (p0.y > p2.y) { std::swap(p0, p2); std::swap(uv0, uv2); }
	if (p1.y > p2.y) { std::swap(p1, p2); std::swap(uv1, uv2); }

	if (std::round(p2.y - p0.y) == 0) // don't care about degenerate triangle
		return;

	for (int y = p0.y; y <= p2.y; y++) {
		glm::ivec3 fp = get_vert_cross_line_y(p0, p2, y);
		glm::ivec3 lp = y < p1.y ? get_vert_cross_line_y(p0, p1, y) : get_vert_cross_line_y(p1, p2, y);
		glm::ivec3 fuv = get_uv_cross_line_y(uv0, uv2, p0, p2, y);
		glm::ivec3 luv = y < p1.y ? get_uv_cross_line_y(uv0, uv1, p0, p1, y) : get_uv_cross_line_y(uv1, uv2, p1, p2, y);

		if (fp.x > lp.x) {
			std::swap(fp, lp); 
			std::swap(fuv, luv); 
		}

		for (int x = fp.x; x <= lp.x; x++) {
			int z = 1;
			glm::ivec3 uv_inter = fuv;
			
			if (lp.x != fp.x) {
				z = (lp.z - fp.z) * (float(x - fp.x) / (lp.x - fp.x)) + fp.z;
				uv_inter = glm::vec3(luv - fuv) * (float(x - fp.x) / (lp.x - fp.x)) + glm::vec3(fuv);
			}

			if (x >= 0 && y >= 0 && x < width && y < height && depth_buffer && z > depth_buffer[y * width + x] &&
				uv_inter.x < tex.get_width() && uv_inter.y < tex.get_height() && uv_inter.x >= 0 && uv_inter.y >= 0) {
				depth_buffer[y * width + x] = z;
				TGAColor color = tex.get(uv_inter.x, uv_inter.y);
				img.set(x, y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity, 255));
			}
		}
	}
}

void Image::draw_model_wire(const Model &model, const TGAColor &color)
{
	int max = model.nfaces();
	for (int i = 0; i < model.nfaces(); i++) {
		Face face = model.get_face(i);
		for (int j = 0; j < 3; j++) {
			glm::vec3 v0 = face[Model::VERTEX_LAYER][j];
			glm::vec3 v1 = face[Model::VERTEX_LAYER][(j + 1) % 3];
			int x0 = (v0.x + 1.0f)*width / 2.0f;
			int y0 = (v0.y + 1.0f)*height / 2.0f;
			int x1 = (v1.x + 1.0f)*width / 2.0f;
			int y1 = (v1.y + 1.0f)*height / 2.0f;
			draw_line(x0, y0, x1, y1, color);
		}
	}
}

void Image::draw_model_texture(const Model &model, TGAImage &tex)
{
	glm::vec3 light_dir(0, 0, -1.0);
	light_dir = glm::normalize(light_dir);

	for (int i = 0; i < model.nfaces(); i++) {
		Face face = model.get_face(i);
		auto world_v = face[Model::VERTEX_LAYER];
		
		for (int j = 0; j < 3; j++) {
			auto v = face[Model::VERTEX_LAYER][j];
			auto uvp = face[Model::TEXCO_LAYER][j];
			face[Model::VERTEX_LAYER][j] = glm::vec3((v.x + 1.0f) * width / 2.0f, (v.y + 1.0f) * height / 2.0f, (v.z + 1.0f) * depth / 2.0f);
			face[Model::TEXCO_LAYER][j] = glm::vec3(uvp.x * tex.get_width(), uvp.y * tex.get_height(), 0.0f);
		}

		glm::vec3 n = glm::cross(world_v[2] - world_v[0], world_v[1] - world_v[0]);
		n = glm::normalize(n);

		float intensity = glm::dot(light_dir, n);
		if (intensity < 0)
			intensity = 0;

		//draw_triangle_flat(face[Model::VERTEX_LAYER][0], face[Model::VERTEX_LAYER][1], face[Model::VERTEX_LAYER][2], TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		draw_triangle_texture(face, intensity, tex);
	}
}

void Image::write_to_file(const std::string &fn)
{
	img.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	img.write_tga_file(fn.c_str());
	img.flip_vertically();
}
