#include "renderer/app.h"
#include "api.h"

auto main() -> int
{
	std::make_shared<Kasumi::Renderer>("empty.txt")
			->load_api(std::make_shared<PCISPHExample>())
			->launch();
	return 0;
}