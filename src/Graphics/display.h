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

	void Clear(float red, float green, float blue, float alpha);
	bool isClosed();

	virtual ~Display();

private:
	SDL_Window* m_Window;
	SDL_GLContext m_glContext;

	bool m_isClosed;

	void SwapBuffers();
};

#endif // DISPLAY_H
