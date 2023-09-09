
#include <Crib/App>
#include "../Graphics/OpenGL/Context.h"

using namespace Crib;


App::Window::Window(Window&& other)
{
	*this = std::move(other);
}


void App::Window::createGraphicsContext(Options opt)
{
	if (context)
	{
		delete context;
		context = nullptr;
	}

	if (!impl)
		return;

	if (opt.preferEngine == Engine::any || opt.preferEngine == Engine::openGL)
		context = new Graphics::OpenGL::Context(*this);

	if (context)
	{
		opt.title = context->description;
		setOptions(opt);
	}
}

void App::Window::draw()
{
	if (context)
		context->draw();
}


void App::Window::onPositionChanged(int2 pos)
{}

void App::Window::onSizeChanged(int2 dims)
{
	// try
	//{
	if (context)
		context->onResize(dims);
	//}
	// catch (Graphics::base::context_invalid e)
	//{
	//	createGraphicsContext({});
	//}
}
