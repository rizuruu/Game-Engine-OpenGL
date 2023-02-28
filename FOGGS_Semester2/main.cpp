#pragma once
#include "HelloGL.h"

int main(int argc, char* argv[])
{
	HelloGL* game = new HelloGL(argc, argv); //Creates an instance of our game  

	return 0; //Assumes a successful exit if our game exits
}