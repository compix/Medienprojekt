#include "MeshManager.h"
#include "GeometryGenerator.h"

Mesh MeshManager::load(MeshType meshType, VertexType vertexType)
{
	auto pair = std::make_pair(meshType, vertexType);
	if (m_meshes.count(pair))
		return m_meshes[pair];

	switch (vertexType)
	{
	case VertexType::POS_UV: 
		m_meshes[pair] = loadPosUVMesh(meshType, isIndexed(meshType));
		break;
	case VertexType::POS:
		m_meshes[pair] = loadPosMesh(meshType, isIndexed(meshType));
		break;
	default:
		assert(false);
	}

	setRenderMode(m_meshes[pair], meshType);

	return m_meshes[pair];
}

MeshData MeshManager::createMeshData(MeshType meshType)
{
	switch (meshType)
	{
	case MeshType::BOX:
		return GeometryGenerator::createBox(1.f, 1.f, 1.f);
	case MeshType::BILLBOARD:
		return GeometryGenerator::createBillboard();
	default:
		assert(false);
	}

	return MeshData();
}

Mesh MeshManager::loadPosUVMesh(MeshType meshType, bool indexed)
{
	MeshData data = createMeshData(meshType);
	Mesh mesh;

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	std::vector<PosUVVertex> vertices(data.vertices.size());
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].position = data.vertices[i].position;
		vertices[i].uv = data.vertices[i].uv;
	}

	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PosUVVertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PosUVVertex), 0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PosUVVertex), (void*)offsetof(PosUVVertex, uv));

	mesh.vertexCount = data.vertices.size();

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (indexed)
	{
		glGenBuffers(1, &mesh.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(GLuint), &data.indices[0], GL_STATIC_DRAW);
		mesh.indexCount = data.indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind
	}

	return mesh;
}

Mesh MeshManager::loadPosMesh(MeshType meshType, bool indexed)
{
	MeshData data = createMeshData(meshType);
	Mesh mesh;

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	std::vector<PosVertex> vertices(data.vertices.size());
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].position = data.vertices[i].position;
	}

	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PosVertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PosVertex), 0);

	mesh.vertexCount = data.vertices.size();
	
	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (indexed)
	{
		glGenBuffers(1, &mesh.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(GLuint), &data.indices[0], GL_STATIC_DRAW);
		mesh.indexCount = data.indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind
	}

	return mesh;
}

void MeshManager::setRenderMode(Mesh& mesh, MeshType meshType)
{
	switch (meshType)
	{
	case MeshType::BOX:
		mesh.renderMode = RenderMode::TRIANGLES_INDEXED;
		break;
	case MeshType::BILLBOARD:
		mesh.renderMode = RenderMode::TRIANGLE_STRIP;
		break;
	default: 
		assert(false);
	}
}

bool MeshManager::isIndexed(MeshType meshType)
{
	switch (meshType)
	{
	case MeshType::BOX:
		return true;
	case MeshType::BILLBOARD:
		return false;
	default:
		assert(false);
	}

	return false;
}