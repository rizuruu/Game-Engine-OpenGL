#include "Player.h"
#include <GL\freeglut.h>
#include <iostream>

Player::Player()
{
	Head = new ModelLoader("bb8_head");
	Head->Transform.Scale = Vector3(0.4, 0.4, 0.4);
	Head->Transform.Position = Vector3(0, 2.65, 0.0);

	Body = new ModelLoader("bb8_body");
	Body->Transform.Scale = Vector3(0.4, 0.4, 0.4);
	Body->Transform.Position = Vector3(0, 1.2, 0.0);

	rotationSpeed = 3.0f; // Adjust to desired rotation speed
	movementSpeed = 0.8f; // Adjust to desired movement speed
	direction = Vector3(0, 0, -1); // Initially facing -Z direction
	GLfloat diff[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	Body->Material.Diffuse[0] = diff[0];
	Body->Material.Diffuse[1] = diff[1];
	Body->Material.Diffuse[2] = diff[2];
	Body->Material.Diffuse[3] = diff[3];
}

Player::~Player()
{
}

void Player::Update(unsigned char key) {
	std::cout << key << std::endl;
	// Rotate left or right based on input
	if (key == 'a') {
		Head->Transform.Rotation.y += rotationSpeed;
		Body->Transform.Rotation.y += rotationSpeed;
		direction.rotateY(-rotationSpeed);
	}
	if (key == 'd') {
		Head->Transform.Rotation.y -= rotationSpeed;
		Body->Transform.Rotation.y -= rotationSpeed;
		direction.rotateY(rotationSpeed);
	}

	// Move forward or backward based on input
	if (key == 'w') {
		Head->Transform.Position += direction * movementSpeed;
		Body->Transform.Position += direction * movementSpeed;
		Body->Transform.Rotation -= direction * rotationSpeed;
	}
	if (key == 's') {
		Head->Transform.Position -= direction * movementSpeed;
		Body->Transform.Position -= direction * movementSpeed;
		Body->Transform.Rotation += direction * rotationSpeed;
	}
}

void Player::Render()
{
	glPushMatrix();
	Body->render();
	glPopMatrix();

	glPushMatrix();
	Head->render();
	glPopMatrix();
}
float Player::lerp(float a, float b, float t) {
	return a * (1.0f - t) + b * t;
}

void Player::Translate(Vector3 position)
{
	// Use lerp to calculate the new rotation value
	float newRotation = lerp(Body->Transform.Rotation.x, Body->Transform.Rotation.x + position.x, 5.5f);

	// Assign the new rotation value to the body transform
	Body->Transform.Rotation.x = newRotation;
	Body->Transform.Position.z -= position.x;
	Head->Transform.Position.z -= position.x;
}

void Player::Rotate(Vector3 rotation)
{
	Body->Transform.Rotation.y += rotation.y;
	Head->Transform.Rotation.y += rotation.y;
}

Matrix3 Player::directionToRotationMatrix(const Vector3& direction) {
	Vector3 up(0, 1, 0);
	Vector3 zAxis = direction.normalize();
	Vector3 xAxis = up.cross(zAxis).normalize();
	Vector3 yAxis = zAxis.cross(xAxis);

	Matrix3 rotationMatrix;
	rotationMatrix.set(0, 0, xAxis.x);
	rotationMatrix.set(0, 1, xAxis.y);
	rotationMatrix.set(0, 2, xAxis.z);
	rotationMatrix.set(1, 0, yAxis.x);
	rotationMatrix.set(1, 1, yAxis.y);
	rotationMatrix.set(1, 2, yAxis.z);
	rotationMatrix.set(2, 0, zAxis.x);
	rotationMatrix.set(2, 1, zAxis.y);
	rotationMatrix.set(2, 2, zAxis.z);

	return rotationMatrix;
}