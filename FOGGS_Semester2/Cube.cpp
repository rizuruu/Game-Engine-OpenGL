#include "Cube.h"
//Vertex Cube::indexedVertices[] = { 1, 1, 1,  -1, 1, 1,  // v0,v1,
//				-1,-1, 1,   1,-1, 1,   // v2,v3
//				1,-1,-1,   1, 1,-1,    // v4,v5
//				-1, 1,-1,   -1,-1,-1 }; // v6,v7
//
//Color Cube::indexedColors[] = { 1, 1, 1,   1, 1, 0,   // v0,v1,
//				1, 0, 0,   1, 0, 1,   // v2,v3
//				0, 0, 1,   0, 1, 1,   // v4,v5
//				0, 1, 0,   0, 0, 0 }; //v6,v7
//
//GLushort Cube::indices[] = { 0, 1, 2,  2, 3, 0,      // front
//				0, 3, 4,  4, 5, 0,      // right
//				0, 5, 6,  6, 1, 0,      // top
//				1, 6, 7,  7, 2, 1,      // left
//				7, 4, 3,  3, 2, 7,      // bottom
//				4, 7, 6,  6, 5, 4 };    // back

Cube::Cube(Mesh* mesh, float x, float y, float z)
{
	_rotation = 0.0f;
	_translation = 0.0f;
	_mesh = mesh;
}

Cube::~Cube()
{
}

void Cube::Draw()
{
	// std::cout << "drawing";
	glRotatef(_rotation, 1.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, _mesh->Vertices);
	glColorPointer(3, GL_FLOAT, 0, _mesh->Colors);

	glPushMatrix();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, _mesh->Indices);
	glPopMatrix();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Cube::Update()
{
	if (_rotation >= 360.0f)
		_rotation = 0.0f;

	_rotation += 1.1f;
}

void Cube::SetRotation(float rotation)
{
	glRotatef(_rotation, 1.0f, 0.0f, 1.0f);
}

void Cube::Translate(unsigned char key, float speed)
{
	if (key == 'd')
	{
		_translation += 1.0f;
		glTranslatef(_translation, 0.0f, 0.0f);
	}
	if (key == 'w')
	{
		_translation += 1.0f;
}
}