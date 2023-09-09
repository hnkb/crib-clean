
#include <crib/app>
#include "../graphics/gl/context_gl.h"

using crib::app::window;


window::window(window&& other)
{
	*this = std::move(other);
}


void window::create_graphics_context(options opt)
{
	if (context)
	{
		delete context;
		context = nullptr;
	}

	if (!impl)
		return;

	if (opt.prefer_engine == engine::any || opt.prefer_engine == engine::opengl)
		context = new graphics::gl::context(*this);

	if (context)
	{
		opt.title = context->description;
		set_options(opt);
	}
}

void window::draw()
{
	if (context)
		context->draw();
}


void window::on_position_changed(int2 pos)
{}

void window::on_size_changed(int2 dims)
{
	if (context)
		context->on_resize(dims);
}
