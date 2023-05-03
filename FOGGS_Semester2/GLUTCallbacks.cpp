#pragma once
#include "GLUTCallbacks.h" // Includes
#include "GameManager.h"

// Namespace implementation
namespace GLUTCallbacks
{
	namespace
	{
		// Initialise to a null pointer before we do anything
		GameManager* gameManager = nullptr;
	}

	void Init(GameManager* gl)
	{
		gameManager = gl;
	}

	void Display()
	{
		if (gameManager != nullptr)
		{
			gameManager->Display();
		}
	}

	void Timer(int preferredRefresh)
	{
		if (gameManager != nullptr)
		{
			gameManager->Update();
		}
	}

	void Keyboard(unsigned char key, int x, int y)
	{
		if (gameManager != nullptr)
		{
			gameManager->Keyboard(key, x, y);
		}
	}
	void Motion(int x, int y)
	{
		if (gameManager != nullptr)
		{
			gameManager->Motion(x, y);
		}
	}
	void MouseWheel(int wheel, int direction, int x, int y)
	{
		if (gameManager != nullptr)
		{
			gameManager->MouseWheel(wheel, direction, x, y);
		}
	}
}