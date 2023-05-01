#pragma once
#include "ModelLoader.h"
#include "Matrix3.h"

class Player
{
private:
public:
	Player();
	~Player();
	void Update(unsigned char key);
	void Render();

	float lerp(float a, float b, float t);

	void Translate(Vector3 position);
	void Rotate(Vector3 rotation);
	Matrix3 directionToRotationMatrix(const Vector3& direction);
	void Scale (Vector3 scale);

	ModelLoader* Head;
	ModelLoader* Body;

	float rotationSpeed;
	float movementSpeed;
	Vector3 direction;

	Vector3 rotateVectorAroundAxis(const Vector3& vec, const Vector3& axis, float angle) {
		float cosAngle = cos(angle);
		float sinAngle = sin(angle);
		Vector3 rotatedVec = vec * cosAngle + axis.cross(vec) * sinAngle + axis * axis.dot(vec) * (1 - cosAngle);
		return rotatedVec;
	}
};

