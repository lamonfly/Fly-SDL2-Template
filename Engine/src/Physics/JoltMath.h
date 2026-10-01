#pragma once
#include <Jolt/Jolt.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// glm <-> Jolt. Quat ctor order differs: Jolt (x,y,z,w), glm (w,x,y,z)
namespace JoltMath
{
	inline JPH::Vec3 ToJolt(const glm::vec3& v) { return JPH::Vec3(v.x, v.y, v.z); }
	inline JPH::RVec3 ToJoltR(const glm::vec3& v) { return JPH::RVec3(v.x, v.y, v.z); }
	inline glm::vec3 ToGlm(JPH::Vec3Arg v) { return glm::vec3(v.GetX(), v.GetY(), v.GetZ()); }
	inline glm::vec3 ToGlmR(JPH::RVec3Arg v) { return glm::vec3(static_cast<float>(v.GetX()), static_cast<float>(v.GetY()), static_cast<float>(v.GetZ())); }

	inline JPH::Quat ToJolt(const glm::quat& q) { return JPH::Quat(q.x, q.y, q.z, q.w); }
	inline glm::quat ToGlm(JPH::QuatArg q) { return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ()); }
}
