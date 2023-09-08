
#pragma once

#include <crib/graphics>
#include <crib/app>

#if defined(_WIN32)
#	include <crib/platform/win>
#	define PLATFORM_GL_CONTEXT HGLRC
#elif defined(__unix__)
#	include <glad/glad_glx.h>
#	define PLATFORM_GL_CONTEXT GLXContext
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
		const app::window& owner;
	};

}
