#include <iostream>
#include "Graphics/display.h"

#include <chrono>
#include <thread>

int main()
{
	Display display(1280, 720, "Test Window");

	display.Clear(1.0f, 0.0f, 0.0f, 0.0f);

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	return 0;

	std::cout << "Hello World!" << std::endl;
}
