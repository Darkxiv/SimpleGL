#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <glm/glm.hpp>

typedef std::vector<std::vector<glm::vec3>> Face;

class Model {
public:
	enum { VERTEX_LAYER, TEXCO_LAYER };

	Model(const char *fn);
	~Model();
	int nverts() const;
	int nfaces() const;
	std::vector<std::vector<glm::vec3>> get_face(int i) const;

private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> uv;
	std::vector<std::vector<std::vector<int>>> geometry;

};

#endif