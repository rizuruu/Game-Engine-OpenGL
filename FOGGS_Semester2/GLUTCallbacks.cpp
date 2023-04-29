#pragma once
#include "GLUTCallbacks.h" // Includes
#include "GameManager.h"

// Namespace implementation
namespace GLUTCallbacks
{
	namespace
	{
		// Initialise to a null pointer before we do anything
		GameManager* helloGL = nullptr;
	}

	void Init(GameManager* gl)
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
			//helloGL->Update();
		}
	}

	void Keyboard(unsigned char key, int x, int y)
	{
		if (helloGL != nullptr)
		{
			helloGL->keyboard(key, x, y);
		}
	}
	void Motion(int x, int y)
	{
		if (helloGL != nullptr)
		{
			helloGL->Motion(x, y);
		}
	}
	void MouseWheel(int wheel, int direction, int x, int y)
	{
		if (helloGL != nullptr)
		{
			helloGL->MouseWheel(wheel, direction, x, y);
		}
	}
}