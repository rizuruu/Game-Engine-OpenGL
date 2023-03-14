#pragma once

#include "Structures.h"

namespace MeshLoader
{
	Mesh* Load(char* path);
	Vertex* indexedVertices = nullptr;
	Color* indexedColors = nullptr;
	GLushort* indices = nullptr;

	int numVertices = 0;
	int numColors = 0;
	int numIndices = 0;

};