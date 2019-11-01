
#include <iostream>

#include "../Core/Core.h"
#include "../RendererOgl/WindowParams.h"

using namespace std;

int main()
{
	Core core;
	core.param1 = 1.0f;
	core.param2 = 2;

	Renderer::WindowParams params;
	params.width = 200;
	params.height = 100;
	params.bits = 32;

	cout << "Hello CMake." << endl;
	getchar();
	return 0;
}
