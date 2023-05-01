#pragma once
#include "ModelLoader.h"

class Player
{
private:
public:
	Player();
	~Player();
	void Update();
	void Render();

	float lerp(float a, float b, float t);

	void Move();

	ModelLoader* Head;
	ModelLoader* Body;

};

