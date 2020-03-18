#ifndef DISPLAY_H
#define DISPLAY_H

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <GL/glew.h>

class Display
{
public:
	Display(int width, int height, const std::string &title);

	void clear(float red, float green, float blue, float alpha);
	void update();
	bool isClosed();

	virtual ~Display();

private:
	SDL_Window* m_Window;
	SDL_GLContext m_glContext;

	bool m_isClosed;

	void swapBuffers();
	void pollEvents();
};

#endif // DISPLAY_H
