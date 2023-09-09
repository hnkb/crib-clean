
#include <Crib/Platform/Win>
#include <objbase.h>

using namespace Crib::Platform::Win;


InitializeCOM::InitializeCOM() : InitializeCOM(COINIT_MULTITHREADED)
{}

InitializeCOM::InitializeCOM(const DWORD concurrencyModel)
{
	// If CoInitializeEx fails, but only because COM has already been initialized with a
	// different concurrency model, we don't want to throw an exception, because COM is
	// available. The only difference is, in that case we don't want to uninitialize in
	// destructor.

	const auto errorCode = CoInitializeEx(nullptr, concurrencyModel);

	mustShutdown = SUCCEEDED(errorCode);

	if (!mustShutdown && errorCode != RPC_E_CHANGED_MODE)
		throw Error(errorCode);  //, "initializing COM");
}

InitializeCOM::~InitializeCOM()
{
	if (mustShutdown)
		CoUninitialize();
}
