#pragma once
#include <glad/glad.h>
#include <cstdint>
#include "Vertex.h"

// VAO + VBO + EBO
class Mesh
{
public:
	explicit Mesh(const MeshData& data);
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void Draw() const;
	uint32_t IndexCount() const { return mIndexCount; }

private:
	GLuint mVao = 0;
	GLuint mVbo = 0;
	GLuint mEbo = 0;
	uint32_t mIndexCount = 0;
};
