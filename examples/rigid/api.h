#ifndef HINAPE_API_H
#define HINAPE_API_H

#include "renderer/app.h"
#include "rigid/reactphysics3d/reactphysics3d.h"

class Api : public Kasumi::Api
{
public:
	void prepare() final;
	void step(float dt) final;
	void ui_sidebar() final;
	void sync();

private:
	reactphysics3d::PhysicsCommon common;
	reactphysics3d::PhysicsWorld *world;

private:
	std::vector<std::pair<Kasumi::SceneObjectPtr, reactphysics3d::RigidBody *>> _objs;
};


#endif //HINAPE_API_H
