
#include <Crib/App>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glad/glad_glx.h>
#include <string>


namespace Crib::Platform::X11
{

	namespace App
	{
		void open();
		void close();

		extern Display* display;
		extern Atom windowClosed;
		extern XContext windowClass;
	}

	class Window
	{
	public:
		Window(Crib::App::Window* owner, const Crib::App::Window::Options& opt);
		~Window();

		void proc(XEvent& event);
		void close();
		void setTitle(const std::string& title);

		Crib::App::Window* owner;

		::Window wnd;
		Colormap colorMap;
		GLXFBConfig pixelFormat;

		bool alreadyDeleted = false;

		int2 dims;
		int2 pos;
	};

	namespace GLX
	{
		GLXFBConfig choosePixelFormat(Display*);
	}

}
