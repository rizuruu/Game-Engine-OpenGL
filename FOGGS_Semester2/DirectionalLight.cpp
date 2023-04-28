#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : color{ 1.0f, 1.0f, 1.0f , 1.0f },
direction{ 0.0f, 0.0f, -1.0f },
ambient{ 0.2f, 0.2f, 0.2f, 1.0f },
specular{ 1.0f, 1.0f, 1.0f, 1.0f },
intensity{ 1212.0f } {}

void DirectionalLight::addLight() {
	if (!glIsEnabled(GL_LIGHT0))
		return;

	// Calculate position of light source from direction vector
	GLfloat position[4] = { -direction[0], -direction[1], -direction[2], 0.0f };

	// Set light properties
	GLfloat diffuse[4] = { color[0] * intensity, color[1] * intensity, color[2] * intensity, color[3] };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Add the light
	glEnable(GL_LIGHT0);
}

void DirectionalLight::draw()
{
	if (!glIsEnabled(GL_LIGHT0))
		return;

	// Draw a small sphere to represent the light source
	glTranslatef(0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor4fv(color);
	glutSolidSphere(0.2, 100, 100);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void DirectionalLight::disable()
{
	glDisable(GL_LIGHT0);
}

void DirectionalLight::enable()
{
	glEnable(GL_LIGHT0);
}

void DirectionalLight::setIntensity(GLfloat intensity)
{
	this->intensity = intensity;
}
