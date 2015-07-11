#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <glm/glm.hpp>

class Model {
public:
	Model(const char *fn);
	~Model();
	int nverts() const;
	int nfaces() const;
	glm::vec3 vert(int i) const;
	std::vector<int> face(int idx) const;
private:
	std::vector<glm::vec3> vertices;
	std::vector<std::vector<int> > faces;
};

#endif