#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

// GL program
class Shader
{
public:
	// Null on compile or link failure, log printed
	static std::shared_ptr<Shader> FromSource(const char* vertexSource, const char* fragmentSource);

	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	void Bind() const;
	GLuint Id() const { return mProgram; }

	void SetMat4(const char* name, const glm::mat4& value);
	void SetMat3(const char* name, const glm::mat3& value);
	void SetVec3(const char* name, const glm::vec3& value);
	void SetVec4(const char* name, const glm::vec4& value);
	void SetFloat(const char* name, float value);
	void SetInt(const char* name, int value);

private:
	explicit Shader(GLuint program) : mProgram(program) {}
	GLint Location(const char* name);

	GLuint mProgram = 0;
	std::unordered_map<std::string, GLint> mLocations;
};
