#include "Application.h"

int main(int argc, char **argv)
{
	try
	{
		Application app;

		app.initApp();
		app.getRoot()->startRendering();
		app.closeApp();
	}
	catch (const std::exception &ex)
	{
		std::cerr << "\n\n# Exception: " << ex.what() << "\n\n";
		system("pause");
	}

	return 0;
}
