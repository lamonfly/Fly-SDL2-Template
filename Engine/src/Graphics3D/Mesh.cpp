#include "Mesh.h"

#include <cstddef>

Mesh::Mesh(const MeshData& data)
{
	mIndexCount = static_cast<uint32_t>(data.Indices.size());

	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);
	glGenBuffers(1, &mEbo);

	glBindVertexArray(mVao);

	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, data.Vertices.size() * sizeof(Vertex), data.Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.Indices.size() * sizeof(uint32_t), data.Indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, UV));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
	if (mEbo) glDeleteBuffers(1, &mEbo);
	if (mVbo) glDeleteBuffers(1, &mVbo);
	if (mVao) glDeleteVertexArrays(1, &mVao);
}

void Mesh::Draw() const
{
	glBindVertexArray(mVao);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
