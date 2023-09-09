
#include <crib/app>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glad/glad_glx.h>
#include <string>


namespace crib::platform::x11
{

	namespace app
	{
		void open();
		void close();

		extern Display* display;
		extern Atom window_closed;
		extern XContext window_class;
	}

	class window
	{
	public:
		window(crib::app::window* owner, const crib::app::window::options& opt);
		~window();

		void proc(XEvent& event);
		void close();
		void set_title(const std::string& title);

		crib::app::window* owner;

		::Window wnd;
		Colormap color_map;
		GLXFBConfig pixel_format;

		bool already_deleted = false;

		int2 dims;
		int2 pos;
	};

	namespace glx
	{
		GLXFBConfig choose_pixel_format(Display*);
	}

}
