
#include <crib/app>
#include "../graphics/gl/context_gl.h"

using crib::app::window;


window::window(window&& other)
{
	*this = std::move(other);
}

window& window::operator=(window&& other)
{
	if (this != &other)
	{
		impl = other.impl;
		context = other.context;
		other.impl = nullptr;
		other.context = nullptr;
	}
	return *this;
}


void window::create_graphics_context(options opt)
{
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
