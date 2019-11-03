
#include <iostream>
#include <memory>

#include "../Core/Game.h"

#include "DefaultState.h"

int main(int argc, char* argv[])
{
	Game g;

	std::unique_ptr<DefaultState> state = std::make_unique<DefaultState>();
	g.SetState(state.get());
	return g.Execute();
}