#ifndef HINAPE_PHYSICS_SYSTEM_H
#define HINAPE_PHYSICS_SYSTEM_H

#include "kernels/pbd/pbd_kernel.h"
#include "kernels/xpbd/xpbd_kernel.h"
#include "physics_object.h"

#include <vector>
#include <variant>
#include <map>

namespace HinaPE
{
class PhysicsSystem
{
public: // Singleton Pattern
    static PhysicsSystem &instance();
    static void destroy();

public:
    void _tick_(float dt);
    void _register_(unsigned int ID, std::shared_ptr<PhysicsObject> ptr);

public:
    PhysicsSystem(const PhysicsSystem &) = delete;
    PhysicsSystem &operator=(const PhysicsSystem &) = delete;
    PhysicsSystem(PhysicsSystem &&) = delete;
    PhysicsSystem &operator=(PhysicsSystem &&) = delete;
private: // disable any instantiation outside
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;

private:
    friend PBDKernel;
    friend XPBDKernel;
    std::variant<PBDKernel, XPBDKernel> kernel;
    std::map<unsigned int, std::shared_ptr<PhysicsObject>> physics_objects;
    std::map<unsigned int, std::shared_ptr<PhysicsObject>> erased_physics_objects;
    std::vector<Constraint> constraints;
};
}

#endif //HINAPE_PHYSICS_SYSTEM_H
