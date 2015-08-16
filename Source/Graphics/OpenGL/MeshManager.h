#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "GeometryGenerator.h"

enum class MeshType
{
	BOX,
	BILLBOARD,
	COUNT
};

enum class VertexType
{
	POS_UV,
	POS
};

typedef std::pair<MeshType, VertexType> MeshTypePair;

class MeshManager
{
	/**
	* Perfect hash for the MeshTypePair
	*/
	struct pair_hash
	{
		std::size_t operator()(const MeshTypePair& pair) const
		{
			return static_cast<std::size_t>(pair.first) * static_cast<std::size_t>(MeshType::COUNT) + static_cast<std::size_t>(pair.second);
		}
	};

public:
	Mesh load(MeshType meshType, VertexType vertexType);

	~MeshManager();
private:
	MeshData createMeshData(MeshType meshType);

	Mesh loadPosUVMesh(MeshType meshType, bool indexed);
	Mesh loadPosMesh(MeshType meshType, bool indexed);

	void setRenderMode(Mesh& mesh, MeshType meshType);

	bool isIndexed(MeshType meshType);
private:
	std::unordered_map<MeshTypePair, Mesh, pair_hash> m_meshes;
};