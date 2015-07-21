#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <glm/glm.hpp>

typedef std::vector<std::vector<glm::vec3>> Face;
typedef std::vector<glm::vec3> Point;

enum { CO_LAYER, UV_LAYER, NOR_LAYER, LAYERS_COUNT };

class Model {
public:
	Model(const char *fn);
	~Model();
	int nverts() const;
	int nfaces() const;
	int nnormals() const;
	int nuv() const;
	Face get_face(int i) const;
	static Point point_from_face(const Face &face, int index);
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> uv;
	std::vector<std::vector<std::vector<int>>> geometry;

};

#endif