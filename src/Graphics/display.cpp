#include "display.h"

Display::Display(int width, int height, const std::string &title)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	m_Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	m_glContext = SDL_GL_CreateContext(m_Window);

	GLenum status = glewInit();

	if(status != GLEW_OK)
		std::cerr << "Glew failed to initialize!" << std::endl;

	m_isClosed = false;

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

bool Display::isClosed()
{
	return m_isClosed;
}

void Display::clear(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::update()
{
	swapBuffers();
	pollEvents();
}

void Display::swapBuffers()
{
	SDL_GL_SwapWindow(m_Window);
}

void Display::pollEvents()
{
	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		switch( e.type )
		{
			case SDL_KEYUP:
				break;

			case SDL_KEYDOWN:
				//switch( e.key.keysym.sym )
				// case SDLK_W:
				break;

			case SDL_MOUSEMOTION:
				//e.motion.yrel
				//e.motion.xrel
				break;

			case SDL_WINDOWEVENT:
				//switch (e.window.event)
				//{
				//	case SDL_WINDOWEVENT_RESIZED:
				//		break;
				//}
				break;

			case SDL_QUIT:
				m_isClosed = true;
				break;
		}
	}
}

Display::~Display()
{
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}
