#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <vector>

static GLuint Compile(GLenum type, const char* source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
	if (!ok)
	{
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		std::vector<char> log(static_cast<size_t>(len) + 1);
		glGetShaderInfoLog(shader, len, nullptr, log.data());
		printf("Shader: %s compile failed:\n%s\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment", log.data());
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

std::shared_ptr<Shader> Shader::FromSource(const char* vertexSource, const char* fragmentSource)
{
	GLuint vs = Compile(GL_VERTEX_SHADER, vertexSource);
	if (!vs) return nullptr;
	GLuint fs = Compile(GL_FRAGMENT_SHADER, fragmentSource);
	if (!fs)
	{
		glDeleteShader(vs);
		return nullptr;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);

	GLint ok = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &ok);
	if (!ok)
	{
		GLint len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		std::vector<char> log(static_cast<size_t>(len) + 1);
		glGetProgramInfoLog(program, len, nullptr, log.data());
		printf("Shader: link failed:\n%s\n", log.data());
		glDeleteProgram(program);
		return nullptr;
	}

	return std::shared_ptr<Shader>(new Shader(program));
}

Shader::~Shader()
{
	if (mProgram) glDeleteProgram(mProgram);
}

void Shader::Bind() const
{
	glUseProgram(mProgram);
}

GLint Shader::Location(const char* name)
{
	auto it = mLocations.find(name);
	if (it != mLocations.end()) return it->second;
	GLint loc = glGetUniformLocation(mProgram, name);
	mLocations.emplace(name, loc);
	return loc;
}

void Shader::SetMat4(const char* name, const glm::mat4& value) { glUniformMatrix4fv(Location(name), 1, GL_FALSE, glm::value_ptr(value)); }
void Shader::SetMat3(const char* name, const glm::mat3& value) { glUniformMatrix3fv(Location(name), 1, GL_FALSE, glm::value_ptr(value)); }
void Shader::SetVec3(const char* name, const glm::vec3& value) { glUniform3fv(Location(name), 1, glm::value_ptr(value)); }
void Shader::SetVec4(const char* name, const glm::vec4& value) { glUniform4fv(Location(name), 1, glm::value_ptr(value)); }
void Shader::SetFloat(const char* name, float value) { glUniform1f(Location(name), value); }
void Shader::SetInt(const char* name, int value) { glUniform1i(Location(name), value); }
