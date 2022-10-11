#include <iostream>
#include "math_lib/bounding_box3.h"
#include "sph/sph_solver3.h"
#include "kernel/volume_particle_emitter3.h"
using namespace jet;

int main()
{
    BoundingBox3D domain(Vector3D(), Vector3D(1, 2, 1));

    auto solver = SphSolver3::builder().withTargetDensity(1000.0).withTargetSpacing(0.02).makeShared();

    solver->setPseudoViscosityCoefficient(0.0);

    auto emitter = VolumeParticleEmitter3::builder()
            //            .withImplicitSurface(surfaceSet)
            //            .withSpacing(targetSpacing)
            //            .withMaxRegion(sourceBound)
            .withIsOneShot(true).makeShared();

    solver->setEmitter(emitter);

    BoundingBox3D sourceBound(domain);
    sourceBound.expand(-0.02);

    return 0;
}
