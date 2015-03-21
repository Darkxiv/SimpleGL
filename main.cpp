#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

Model *model = NULL;
const int width = 800;
const int height = 800;

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
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
		image.set(x0, y0, color);
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
			image.set(y, x, color);
		else
			image.set(x, y, color);

		e += s;
		if (e > dx)
		{
			y += sign ? 1 : -1;
			e -= dx * 2;
		}
	}
}

int main(int argc, char** argv) 
{
	model = new Model("african_head.obj");

	TGAImage image(width, height, TGAImage::RGB);
	for (int i = 0; i<model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j<3; j++) {
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.0f)*width / 2.0f;
			int y0 = (v0.y + 1.0f)*height / 2.0f;
			int x1 = (v1.x + 1.0f)*width / 2.0f;
			int y1 = (v1.y + 1.0f)*height / 2.0f;
			draw_line(x0, y0, x1, y1, image, green);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}
