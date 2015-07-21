#include "tgaimage.h"
#include "settings.h"
#include "model.h"
#include "graphics.h"

#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor gray = TGAColor(200, 200, 200, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const int width = 2400;
const int height = 2400;
const int depth = 2400;

int main(int argc, char** argv) 
{
	Canvas ima(width, height, depth);

#if DRAW == DRAW_MODEL
	ima.set_color(TGAColor(255, 0, 0, 255));
	ima.set_mode(Canvas::MODE_FLAT);
	ima.set_render_settings(NORMAL_MAPPING | TEXTURE_MAPPING | USE_Z_BUFFER);

	Model model("african_head.obj");
	TGAImage tex;
	if (tex.read_tga_file("african_head_diffuse.tga"))
		printf("Texture loaded\n");
	else {
		printf("Error with texture loading\n");
		return 1;
	}
	tex.flip_vertically();
	ima.bind_diffuse_texture(tex);

	ima.set_light(glm::vec3(0, -1.0, 1.0));
	ima.draw_model(model);

#elif DRAW == DRAW_TEST
	std::vector<std::vector<glm::vec3> > triangles{ { glm::vec3(10.0f, 70.0f, 0.0f), glm::vec3(50.0f, 60.0f, 0.0f), glm::vec3(70.0f, 80.0f, 0.0f) },
		{ glm::vec3(180.0f, 50.0f, 0.0f), glm::vec3(150.0f, 1.0f, 0.0f), glm::vec3(70.0f, 180.0f, 0.0f) },
		{ glm::vec3(180.0f, 150.0f, 0.0f), glm::vec3(120.0f, 160.0f, 0.0f), glm::vec3(130.0f, 180.0f, 0.0f) }
	};

	int i = 0;
	std::vector<TGAColor> colors{ red, white, green };
	for (auto vec: triangles) {
		ima.draw_triangle_flat(vec[0], vec[1], vec[2], colors[i % colors.size()]);
		i++;
	}
#else
	printf("Nothing to draw\n");
	return 1;

#endif

	ima.write_to_file("output.tga");
	return 0;
}
