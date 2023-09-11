
#include <iostream>
#include <Crib/App>
#include <cmath>

void startFont();

float2 offset = { 0.f, 0.f };
float scale = .25f;
std::string text;
float scaleX = 1.f;


class MyWindow : public Crib::App::Window
{
public:
	MyWindow()
		: Window(Options {
			"HelloTriangle",
			{400, 300},
			{800, 800}
    })
	{}
	void onSizeChanged(int2 dims) override
	{
		Window::onSizeChanged(dims);
		printf("size\n");

		opt.size = dims;
	}
	void onPositionChanged(int2 pos) override
	{
		Window::onPositionChanged(pos);
		printf("move\n");
	}
	void onKeyChar(const std::string& str) override
	{
		if (str[0] == 27)  // Escape
		{
			if (opt.title.empty())
				close();
			else
				opt.title.clear();
		}
		else if (str[0] == 8)  // Backspace
		{
			// remove all UTF-8 continuation bytes
			while (opt.title.size() && (opt.title.back() & 0xc0) == 0x80)
				opt.title.pop_back();
			if (opt.title.size())
				opt.title.pop_back();
		}
		else
		{
			opt.title += str;
		}
		//setOptions(opt);
		text = opt.title;
		draw();
	}
	void onMouseEvent(const Crib::App::MouseEvent& ev) override
	{
		using Crib::App::MouseEvent;

		if (ev.type == MouseEvent::Type::ButtonDown)
		{
			if (dragStartPos.x == -1)
			{
				dragStartPos = ev.pos;
				dragStartOffset = offset;
			}
		}
		else if (ev.type == MouseEvent::Type::ButtonUp)
		{
			dragStartPos = { -1 };
			this->draw();
		}
		else if (ev.type == MouseEvent::Type::Move && dragStartPos.x != -1)
		{
			offset = toFloat2(ev.pos - dragStartPos) / 400.f / scale;
			offset.y *= -1.f;
			offset += dragStartOffset;
			draw();
		}
		else if (ev.type == MouseEvent::Type::Wheel)
		{
			// first, I convert from pixel space to range -1...1
			auto screen = toFloat2(ev.pos) / toFloat2(opt.size) * 2.f - 1.f;
			screen.y *= -1;

			// this is the reverse of the transformation in my shader:
			// screen = (world + offset) * scale

			// I have two values for scale (before and after).
			// I want the screen to point to the same world coordinates, but it does not.
			// So, I have to add the difference between worldBefore and worldAfter to my offset.
			//
			// Therefore:
			//     world_before = screen / scale_before - offset
			//  -  world_after  = screen / scale_after  - offset
			// --------------------------------------------------
			//        diff       = (screen / scale_before) - (screen / scale_after)

			float2 scale_before = { scale / scaleX, scale };

			scale *= powf(1.25f, ev.wheel);

			float2 scale_after = { scale / scaleX, scale };

			offset -= screen / scale_before - screen / scale_after;

			draw();
		}
	}

private:
	Options opt;
	int2 dragStartPos = { -1 };
	float2 dragStartOffset = {};
};

int main()
{
	try
	{
		//startFont();
		//return 0;

		//Crib::App::Window win;
		MyWindow win2;
		auto win = std::move(win2);
		return Crib::App::run();
	}
	catch (std::exception& ex)
	{
		std::cout << "App finished with error:\n" << ex.what() << "\n";
	}

	return 0;
}
