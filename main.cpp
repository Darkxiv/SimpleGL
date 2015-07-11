#include "tgaimage.h"
#include "settings.h"
#include "model.h"
#include "graphics.h"

#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

const int width = 200;
const int height = 200;

int main(int argc, char** argv) 
{
	Image ima(width, height);

#if DRAW == DRAW_MODEL
	Model model("african_head.obj");
	ima.draw_model(model, red);

#elif DRAW == DRAW_TEST
	std::vector<std::vector<glm::vec2> > triangles{ {glm::vec2(10.0f, 70.0f), glm::vec2(50.0f, 60.0f), glm::vec2(70.0f, 80.0f)},
										{glm::vec2(180.0f, 50.0f), glm::vec2(150.0f, 1.0f), glm::vec2(70.0f, 180.0f)},
										{glm::vec2(180.0f, 150.0f), glm::vec2(120.0f, 160.0f), glm::vec2(130.0f, 180.0f)}
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
