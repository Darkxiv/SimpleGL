#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

// need to check value during processing
Model::Model(const char *fn) : vertices(), geometry(LAYERS_COUNT, std::vector<std::vector<int>>())
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
			std::vector<int> v, t, n;
			int idn, idv, idt;
			iss >> tmp;
			while (iss >> idv >> tmp >> idt >> tmp >> idn) {
				idv--; // in wavefront obj all indices start at 1, not zero
				v.push_back(idv);
				idt--;
				t.push_back(idt);
				idn--;
				n.push_back(idn);
			}
			geometry[CO_LAYER].push_back(v);
			geometry[UV_LAYER].push_back(t);
			geometry[NOR_LAYER].push_back(n);
		}
		else if (!line.compare(0, 4, "vt  ")) {
			iss >> tmp >> tmp;
			glm::vec3 v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];

			uv.push_back(v);
		}
		else if (!line.compare(0, 4, "vn  ")) {
			iss >> tmp >> tmp;
			glm::vec3 v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];

			normals.push_back(v);
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
	return (int)geometry[CO_LAYER].size();
}

int Model::nnormals() const
{
	return (int)geometry[NOR_LAYER].size();
}

int Model::nuv() const
{
	return (int)geometry[UV_LAYER].size();
}

Face Model::get_face(int idx) const
{
	std::vector<int> idv = geometry[CO_LAYER][idx];
	std::vector<int> idt = geometry[UV_LAYER][idx];
	std::vector<int> idn = geometry[NOR_LAYER][idx];
	std::vector<glm::vec3> vert, texco, nors;

	for (int i = 0; i < 3; i++) {
		vert.push_back(vertices[idv[i]]);
		texco.push_back(uv[idt[i]]);
		nors.push_back(normals[idn[i]]);
	}

	return Face{ vert, texco, nors };
}

Point Model::point_from_face(const Face &face, int index)
{
	Point p(LAYERS_COUNT, glm::ivec3());

	if (index < 0 || index >= LAYERS_COUNT)
		return p;

	for (int i = 0; i < LAYERS_COUNT; i++)
		p[i] = face[i][index];

	return p;
}
