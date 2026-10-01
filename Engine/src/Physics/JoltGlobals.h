#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystem.h>

// Process-wide Jolt state
class JoltGlobals
{
public:
	static void Init();
	static void Shutdown();

	static JPH::TempAllocator* GetTempAllocator();
	static JPH::JobSystem* GetJobSystem();
};
