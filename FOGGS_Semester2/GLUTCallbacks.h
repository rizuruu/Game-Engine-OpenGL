#pragma once

class GameManager;

namespace GLUTCallbacks
{
	void Init(GameManager* gl);
	void Display();
	void Timer(int preferredRefresh);
	void Keyboard(unsigned char key, int x, int y);
	void Motion(int x, int y);
}