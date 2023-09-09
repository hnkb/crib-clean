
#include <iostream>
#include <crib/app>


class myWindow : public crib::app::window
{
public:
	void on_size_changed(int2 dims) override
	{
		window::on_size_changed(dims);
		printf("size\n");
	}
	void on_position_changed(int2 pos) override
	{
		window::on_position_changed(pos);
		printf("move\n");
	}
	void on_key_char(const std::string& str) override
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
		set_options(opt);
	}

private:
	options opt;
};

int main()
{
	try
	{
		//crib::app::window win;
		myWindow win2;
		auto win = std::move(win2);
		return crib::app::run();
	}
	catch (std::exception& ex)
	{
		std::cout << "App finished with error:\n" << ex.what() << "\n";
	}

	return 0;
}
