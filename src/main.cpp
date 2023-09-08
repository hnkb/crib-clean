
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
