#include <iostream>
#include "Graphics/display.h"

#include <chrono>
#include <thread>

int main()
{
	Display display(1280, 720, "Test Window");

	while(!display.isClosed())
	{
		display.clear(1.0f, 0.0f, 0.0f, 1.0f);

		display.update();
	}

	return 0;
}
