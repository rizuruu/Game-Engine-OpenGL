#include "Player.h"
#include <GL\freeglut.h>

Player::Player()
{
	Head = new ModelLoader("bb8_head");
	Head->Transform.Scale = Vector3(0.4, 0.4, 0.4);
	Head->Transform.Position = Vector3(0, 2.65, 0.0);

	Body = new ModelLoader("bb8_body");
	Body->Transform.Scale = Vector3(0.4, 0.4, 0.4);
	Body->Transform.Position = Vector3(0, 1.2, 0.0);
}

Player::~Player()
{
}

void Player::Update()
{
}

void Player::Render()
{
	glPushMatrix();
	Body->Transform.Rotation.x += 1.0f;
	Body->render();
	glPopMatrix();

	glPushMatrix();
	Head->render();
	glPopMatrix();
}
float Player::lerp(float a, float b, float t) {
	return a * (1.0f - t) + b * t;
}

void Player::Move()
{
	// Use lerp to calculate the new rotation value
	float newRotation = lerp(Body->Transform.Rotation.x, Body->Transform.Rotation.x - 3.0f, 1.5f);

	// Assign the new rotation value to the body transform
	Body->Transform.Rotation.x = newRotation;
	Body->Transform.Position.z -= 0.3f;
	Head->Transform.Position.z -= 0.3f;
}
