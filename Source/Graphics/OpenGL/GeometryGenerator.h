#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

struct Vertex
{
	Vertex(){}
	Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec3& t, const glm::vec3& uv)
		: position(p), normal(n), tangentU(t), uv(uv){}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: position(px, py, pz), normal(nx, ny, nz),
		tangentU(tx, ty, tz), uv(u, v){}

	Vertex(const glm::vec3& pos) : position(pos) {}
	Vertex(float px, float py, float pz) : position(px, py, pz) {}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangentU;
	glm::vec2 uv;
};

struct PosUVVertex
{
	glm::vec3 position;
	glm::vec2 uv;
};

struct PosVertex
{
	glm::vec3 position;
};

enum class RenderMode
{
	TRIANGLES_INDEXED,
	TRIANGLE_STRIP
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

class GeometryGenerator
{
public:
	static MeshData createBox(float width, float height, float depth);

	static MeshData createBillboard();
};