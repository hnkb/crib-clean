
#include <iostream>
#include <crib/app>


int main()
{
	try
	{
		crib::app::window win;
		return crib::app::run();
	}
	catch (std::exception& ex)
	{
		std::cout << "App finished with error:\n" << ex.what() << "\n";
	}

	return 0;
}
