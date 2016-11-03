#ifndef  __MODULEMESHES_H__
#define __MODULEMESHES_H__

#include "Module.h"
#include <cstdint>
#include <string>

class aiNode;
class aiScene;

struct Mesh
{
	//Vertices
	uint id_vertices = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	//Indices
	uint id_indices = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	//UVs
	uint id_uvs = 0;
	uint num_uvs = 0;
	float* uvs = nullptr;

	//Normals
	float* normals = nullptr;

	//Vertex colors
	float* colors = nullptr;

	std::string file_path;
};

class ModuleMeshes : public Module
{
public:
	ModuleMeshes(const char* name, bool start_enabled = true);
	~ModuleMeshes();

	bool Init(Data& config);
	bool CleanUp();

	bool Load(const char* path, const char* base_path);

	uint LoadTexture(const char* path);

private:

	void LoadNode(aiNode* node,const aiScene* scene,GameObject* parent, const char* base_path);
};

#endif // __MODULEMESHES_H__
