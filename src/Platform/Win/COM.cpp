
#include <Crib/Platform/Win>
#include <objbase.h>

using namespace crib::platform::win;


initialize_com::initialize_com() : initialize_com(COINIT_MULTITHREADED)
{}

initialize_com::initialize_com(const DWORD concurrency_model)
{
	// If CoInitializeEx fails, but only because COM has already been initialized with a
	// different concurrency model, we don't want to throw an exception, because COM is
	// available. The only difference is, in that case we don't want to uninitialize in
	// destructor.

	const auto error_code = CoInitializeEx(nullptr, concurrency_model);

	must_shutdown = SUCCEEDED(error_code);

	if (!must_shutdown && error_code != RPC_E_CHANGED_MODE)
		throw error(error_code);  //, "initializing COM");
}

initialize_com::~initialize_com()
{
	if (must_shutdown)
		CoUninitialize();
}
