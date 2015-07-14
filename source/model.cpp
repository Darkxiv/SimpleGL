#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

// need to check value during processing
Model::Model(const char *fn) : vertices(), geometry(2, std::vector<std::vector<int>>())
{
	std::ifstream in;
	in.open(fn, std::ifstream::in);
	if (in.fail()) 
		return;
	
	std::string line;
	
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		
		char tmp;
		if (!line.compare(0, 2, "v ")) {
			iss >> tmp;
			glm::vec3 v;
			for (int i = 0; i < 3; i++) 
				iss >> v[i];

			vertices.push_back(v);
		}
		else if (!line.compare(0, 2, "f ")) {
			std::vector<int> f, t;
			int tmpv, idx, idt;
			iss >> tmp;
			while (iss >> idx >> tmp >> idt >> tmp >> tmpv) {
				idx--; // in wavefront obj all indices start at 1, not zero
				idt--;
				f.push_back(idx);
				t.push_back(idt);
			}
			geometry[VERTEX_LAYER].push_back(f);
			geometry[TEXCO_LAYER].push_back(t);
		}
		else if (!line.compare(0, 4, "vt  ")) {
			iss >> tmp >> tmp;
			glm::vec3 v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];

			uv.push_back(v);
		}
	}
	std::cout << "# v# " << vertices.size() << " f# " << geometry.size() << std::endl;
}

Model::~Model() { }

int Model::nverts() const
{
	return (int)vertices.size();
}

int Model::nfaces() const
{
	return (int)geometry[VERTEX_LAYER].size();
}

std::vector<std::vector<glm::vec3>> Model::get_face(int idx) const
{
	std::vector<int> idv = geometry[VERTEX_LAYER][idx];
	std::vector<int> idt = geometry[TEXCO_LAYER][idx];
	std::vector<glm::vec3> vert, texco;

	for (int i = 0; i < 3; i++) {
		vert.push_back(vertices[idv[i]]);
		texco.push_back(uv[idt[i]]);
	}

	return std::vector<std::vector<glm::vec3>>{vert, texco};
}