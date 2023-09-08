
#include <crib/app>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>


namespace crib::platform::x11
{

	namespace app
	{
		void open();
		void close();

		extern Display* display;
		extern Atom     window_closed;
		extern XContext window_class;
	}

	class window
	{
	public:
		window(const crib::app::window::options& opt);
		~window();

		void proc(XEvent& event);
		void close();
		void set_title(const std::string& title);

	private:
		int      screen;
		::Window wnd;
		::GC     gc;

		bool already_deleted = false;
	};
    
}
