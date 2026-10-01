#include "JoltGlobals.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <cstdarg>
#include <cstdio>
#include <thread>
#include <algorithm>

static JPH::TempAllocatorImpl* sTempAllocator = nullptr;
static JPH::JobSystemThreadPool* sJobSystem = nullptr;
static bool sInitialized = false;

static void TraceImpl(const char* format, ...)
{
	va_list list;
	va_start(list, format);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, list);
	va_end(list);
	printf("[Jolt] %s\n", buffer);
}

#ifdef JPH_ENABLE_ASSERTS
static bool AssertFailedImpl(const char* expression, const char* message, const char* file, JPH::uint line)
{
	printf("[Jolt] %s:%u: (%s) %s\n", file, line, expression, message != nullptr ? message : "");
	return true; // break
}
#endif

void JoltGlobals::Init()
{
	if (sInitialized) return;

	JPH::RegisterDefaultAllocator();
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	sTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

	int threads = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 1);
	sJobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, threads);

	sInitialized = true;
}

void JoltGlobals::Shutdown()
{
	if (!sInitialized) return;

	delete sJobSystem;
	sJobSystem = nullptr;
	delete sTempAllocator;
	sTempAllocator = nullptr;

	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	sInitialized = false;
}

JPH::TempAllocator* JoltGlobals::GetTempAllocator() { return sTempAllocator; }
JPH::JobSystem* JoltGlobals::GetJobSystem() { return sJobSystem; }
