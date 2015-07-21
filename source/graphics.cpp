#include "graphics.h"
#include "settings.h"
#include "tgaimage.h"
#include "model.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <stdio.h>

Canvas::Canvas(int w, int h, int d) : width(w), height(h), depth(d), img(w, h, TGAImage::RGB),
mode(MODE_WIRE), col(255, 255, 255, 255), texture_flags(EMPTY), render_flags(EMPTY), light_dir(0.0, -1.0, 1.0)
{
	if (w > 0 && h > 0)
		depth_buffer = new int[w * h]();
	else
		depth_buffer = NULL;
}

Canvas::~Canvas()
{
	if (depth_buffer) 
		delete[] depth_buffer;
}

void Canvas::draw_line(int x0, int y0, int x1, int y1, TGAColor color)
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

void Canvas::draw_line(glm::ivec3 p0, glm::ivec3 p1, TGAColor color)
{
	draw_line(p0.x, p0.y, p1.x, p1.y, color);
}

void Canvas::draw_triangle_wire(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color)
{
	draw_line(p0, p1, color);
	draw_line(p1, p2, color);
	draw_line(p2, p0, color);
}

glm::ivec3 Canvas::get_vert_cross_line_y(const glm::vec3 &p0, const glm::vec3 &p1, int y) {
	glm::ivec3 p;

	if (std::round(p0.y - p1.y) != 0) {
		p.x = std::round((p1.x - p0.x) * (float(y - p0.y) / (p1.y - p0.y)) + p0.x);
		p.z = std::round((p1.z - p0.z) * (float(y - p0.y) / (p1.y - p0.y)) + p0.z);
	}
	else {
		p = glm::ivec3(p0.x < p1.x ? p0 : p1);
	}
	p.y = y;

	return p;
}

// deprecated
void Canvas::draw_triangle_flat(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2, TGAColor color)
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

// TODO make interpolation general for all layers
void Canvas::draw_triangle_texture(Face &geom)
{
	bool texture_mapping = render_flags & TEXTURE_MAPPING;
	bool normal_mapping = render_flags & NORMAL_MAPPING;
	bool z_buffer = (render_flags & USE_Z_BUFFER) && depth_buffer;
	bool shadeless = render_flags & SHADELESS;

	glm::vec3 p0[] = { geom[CO_LAYER][0], geom[UV_LAYER][0] },
		p1[] = { geom[CO_LAYER][1], geom[UV_LAYER][1] },
		p2[] = { geom[CO_LAYER][2], geom[UV_LAYER][2] };
	glm::vec3 &n0 = geom[NOR_LAYER][0], &n1 = geom[NOR_LAYER][1], &n2 = geom[NOR_LAYER][2];
	float face_intensity = 1.0;

	if (!normal_mapping && !shadeless) {
		glm::vec3 facen = glm::normalize(glm::cross(geom[CO_LAYER][2] - geom[CO_LAYER][0], geom[CO_LAYER][1] - geom[CO_LAYER][0]));
		face_intensity = -glm::dot(light_dir, facen);
	}

	if (p0[0].y > p1[0].y) { std::swap(p0, p1); std::swap(n0, n1); }
	if (p0[0].y > p2[0].y) { std::swap(p0, p2); std::swap(n0, n2); }
	if (p1[0].y > p2[0].y) { std::swap(p1, p2); std::swap(n1, n2); }

	if (std::round(p2[0].y - p0[0].y) == 0) // don't care about degenerate triangle
		return;

	for (int y = std::round(p0[0].y); y <= std::round(p2[0].y); y++) {
		float alpha = float(y - p0[0].y) / (p2[0].y - p0[0].y);
		float betta = y < p1[0].y ? float(y - p0[0].y) / (p1[0].y - p0[0].y) :
				float(y - p1[0].y) / (p2[0].y - p1[0].y);

		if (y < p1[0].y ? std::round(p1[0].y - p0[0].y) == 0 : std::round(p2[0].y - p1[0].y) == 0)
			betta = 1.0;

		glm::vec3 fp[] = { (p2[0] - p0[0]) * alpha + p0[0], (p2[1] - p0[1]) * alpha + p0[1] };
		glm::vec3 lp[] = { y < p1[0].y ? (p1[0] - p0[0]) * betta + p0[0] : (p2[0] - p1[0]) * betta + p1[0],
			y < p1[0].y ? (p1[1] - p0[1]) * betta + p0[1] : (p2[1] - p1[1]) * betta + p1[1] };

		glm::vec3 fn, ln;
		if (normal_mapping) {
			fn = (n2 - n0) * alpha + n0;
			ln = y < p1[0].y ? (n1 - n0) * betta + n0 : (n2 - n1) * betta + n1;
		}

		if (fp[0].x > lp[0].x) {
			std::swap(fp, lp);
			if (normal_mapping)
				std::swap(fn, ln);
		}

		for (int x = std::round(fp[0].x); x <= std::round(lp[0].x); x++) {
			float gamma = lp[0].x == fp[0].x ? 0.0f : float(x - fp[0].x) / (lp[0].x - fp[0].x);
			float intensity = face_intensity;
			
			if (normal_mapping)
				intensity = glm::dot(glm::normalize((ln - fn) * gamma + fn), light_dir);

			if (intensity < 0.0)
				intensity = 0.0;

			if (!(x >= 0 && y >= 0 && x < width && y < height))
				continue;

			if (z_buffer) {
				int z = (lp[0].z - fp[0].z) * gamma + fp[0].z;
				if (z > depth_buffer[y * width + x])
					depth_buffer[y * width + x] = z;
				else
					continue;
			}

			if (texture_mapping && !shadeless) {
				glm::ivec3 uv_inter((lp[1] - fp[1]) * gamma + fp[1]);
				if (uv_inter.x < diffuse.get_width() && uv_inter.y < diffuse.get_height() && uv_inter.x >= 0 && uv_inter.y >= 0) {
					TGAColor color = diffuse.get(uv_inter.x, uv_inter.y);
					img.set(x, y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity, 255));
				}
				else {
					// depends on clamping mode
				}
			}
			else {
				img.set(x, y, TGAColor(col.r * intensity, col.g * intensity, col.b * intensity, 255));
			}
		}
	}
}

void Canvas::draw_model(const Model &model)
{
	switch (mode)
	{
	case Canvas::MODE_WIRE:
		draw_model_wire(model);
		break;
	case Canvas::MODE_FLAT:
		draw_model_texture(model);
		break;
	default:
		printf("Error: incorrect render mode.");
		break;
	}
}

void Canvas::draw_model_wire(const Model &model)
{
	int max = model.nfaces();
	for (int i = 0; i < model.nfaces(); i++) {
		Face face = model.get_face(i);
		for (int j = 0; j < 3; j++) {
			glm::vec3 v0 = face[CO_LAYER][j];
			glm::vec3 v1 = face[CO_LAYER][(j + 1) % 3];
			int x0 = (v0.x + 1.0f) * width / 2.0f;
			int y0 = (v0.y + 1.0f) * height / 2.0f;
			int x1 = (v1.x + 1.0f) * width / 2.0f;
			int y1 = (v1.y + 1.0f) * height / 2.0f;
			draw_line(x0, y0, x1, y1, col);
		}
	}
}

void Canvas::draw_model_texture(const Model &model)
{
	for (int i = 0; i < model.nfaces(); i++) {
		Face face = model.get_face(i);
		auto world_v = face[CO_LAYER];
		
		for (int j = 0; j < 3; j++) {
			auto v = face[CO_LAYER][j];
			face[CO_LAYER][j] = glm::vec3((v.x + 1.0f) * width / 2.0f, (v.y + 1.0f) * height / 2.0f, (v.z + 1.0f) * depth / 2.0f);

			if (render_flags & TEXTURE_MAPPING) {
				if (texture_flags & DIFFUSE_TEXTURE) {
					auto uvp = face[UV_LAYER][j];
					face[UV_LAYER][j] = glm::vec3(uvp.x * diffuse.get_width(), uvp.y * diffuse.get_height(), 0.0f);
				}
			}
		}

		draw_triangle_texture(face);
	}
}

void Canvas::write_to_file(const std::string &fn)
{
	img.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	img.write_tga_file(fn.c_str());
	img.flip_vertically();
}


void Canvas::bind_diffuse_texture(const TGAImage &texture)
{
	diffuse = texture;
	texture_flags |= DIFFUSE_TEXTURE;
}

void Canvas::set_mode(const Render_mode &draw_mode)
{
	mode = draw_mode;
}

void Canvas::set_color(const TGAColor &color)
{
	col = color;
}

void Canvas::set_light(const glm::vec3 &light_direction)
{
	light_dir = glm::normalize(light_direction);
}

void Canvas::set_render_settings(unsigned int flags)
{
	render_flags = flags;
}

void Canvas::set_shadeless(const bool flag)
{
	if (flag)
		render_flags |= SHADELESS;
	else
		render_flags &= ~SHADELESS;
}

void Canvas::set_normal_mapping(const bool flag)
{
	if (flag)
		render_flags |= NORMAL_MAPPING;
	else
		render_flags &= ~NORMAL_MAPPING;
}

void Canvas::set_texture_mapping(const bool flag)
{
	if (flag)
		render_flags |= TEXTURE_MAPPING;
	else
		render_flags &= ~TEXTURE_MAPPING;
}

void Canvas::set_depth_buffer(const bool flag)
{
	if (flag)
		render_flags |= USE_Z_BUFFER;
	else
		render_flags &= ~USE_Z_BUFFER;
}
