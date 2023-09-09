
#pragma once

#include <Crib/Graphics>
#include <Crib/App>

#if defined(_WIN32)
#	include <Crib/Platform/Win>
#	define PLATFORM_GL_CONTEXT HGLRC
#	define PLATFORM_OWNER_TYPE HWND
#elif defined(__unix__)
#	include <glad/glad_glx.h>
#	define PLATFORM_GL_CONTEXT GLXContext
#	define PLATFORM_OWNER_TYPE crib::platform::x11::window&

namespace crib::platform::x11
{
	class window;
}
#endif


namespace crib::graphics::gl
{

	class context : public graphics::context
	{
	public:
		context(const app::window&);
		virtual ~context();

		virtual void draw() override;
		virtual void on_resize(int2 dims) override;

	private:
		void draw_platform_independent();
		void read_device_name(int swapInterval);

		PLATFORM_GL_CONTEXT ctx = nullptr;
		PLATFORM_OWNER_TYPE owner;
	};

}
