#include "solver.h"

auto main() -> int
{
	auto renderer = std::make_shared<Kasumi::Renderer3D>();
	auto solver = std::make_shared<HinaPE::RigidSolver>();


	auto sphere1 = std::make_shared<Kasumi::SphereObject>();
	solver->add(sphere1, HinaPE::RigidType::Static);
	auto sphere2 = std::make_shared<Kasumi::SphereObject>();
	sphere2->POSE.position = {0, 5, 0};
	solver->add(sphere2);

	renderer->_init = [&](const Kasumi::Scene3DPtr &scene)
	{
		scene->add(sphere1);
		scene->add(sphere2);
	};
	renderer->_step = [&](real dt)
	{
		solver->update(dt);
	};

	renderer->launch();
	return 0;
}