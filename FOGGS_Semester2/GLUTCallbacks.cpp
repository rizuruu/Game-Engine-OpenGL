// Includes
#include "GLUTCallbacks.h"
#include "HelloGL.h"

// Namespace implementation
namespace GLUTCallbacks
{
	namespace
	{
		// Initialise to a null pointer before we do anything
		HelloGL* helloGL = nullptr;
	}

	void Init(HelloGL* gl)
	{
		helloGL = gl;
	}

	void Display()
	{
		if (helloGL != nullptr)
		{
			helloGL->Display();
		}
	}

	void Timer(int preferredRefresh)
	{
		if (helloGL != nullptr)
		{
			helloGL->Update();
			glutTimerFunc(preferredRefresh, GLUTCallbacks::Timer, preferredRefresh);
		}
	}

	void Keyboard(unsigned char key, int x, int y)
	{
		if (helloGL != nullptr)
		{
			helloGL->Keyboard(key, x, y);
		}
	}
}
