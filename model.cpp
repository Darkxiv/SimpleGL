#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *fn) : vertices(), faces() 
{
	std::ifstream in;
	in.open(fn, std::ifstream::in);
	if (in.fail()) 
		return;
	
	std::string line;
	while (!in.eof()) 
	{
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		
		char tmp;
		if (!line.compare(0, 2, "v ")) 
		{
			iss >> tmp;
			Vec3f v;
			for (int i = 0; i < 3; i++) 
				iss >> v.raw[i];

			vertices.push_back(v);
		}
		else if (!line.compare(0, 2, "f ")) 
		{
			std::vector<int> f;
			int tmpv, idx;
			iss >> tmp;
			while (iss >> idx >> tmp >> tmpv >> tmp >> tmpv) 
			{
				idx--; // in wavefront obj all indices start at 1, not zero
				f.push_back(idx);
			}
			faces.push_back(f);
		}
	}
	std::cerr << "# v# " << vertices.size() << " f# " << faces.size() << std::endl;
}

Model::~Model() { }

int Model::nverts() 
{
	return (int)vertices.size();
}

int Model::nfaces() 
{
	return (int)faces.size();
}

std::vector<int> Model::face(int idx) 
{
	return faces[idx];
}

Vec3f Model::vert(int i) 
{
	return vertices[i];
}