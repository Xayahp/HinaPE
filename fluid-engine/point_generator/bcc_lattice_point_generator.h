#ifndef HINAPE_FLUID_ENGINE_BCC_LATTICE_POINT_GENERATOR_H_
#define HINAPE_FLUID_ENGINE_BCC_LATTICE_POINT_GENERATOR_H_

#include "point_generator3.h"

namespace HinaPE::FluidEngine
{

//!
//! \brief Body-centered lattice points generator.
//!
//! \see http://en.wikipedia.org/wiki/Cubic_crystal_system
//!      http://mathworld.wolfram.com/CubicClosePacking.html
//!
class BccLatticePointGenerator final : public PointGenerator3
{
public:
    //!
    //! \brief Invokes \p callback function for each BCC-lattice points inside
    //! \p boundingBox.
    //!
    //! This function iterates every BCC-lattice points inside \p boundingBox
    //! where \p spacing is the size of the unit cell of BCC structure.
    //!
    void forEachPoint(const BoundingBox3D &boundingBox, double spacing, const std::function<bool(const Vector3D &)> &callback) const override;
};

//! Shared pointer type for the BccLatticePointGenerator.
typedef std::shared_ptr<BccLatticePointGenerator> BccLatticePointGeneratorPtr;

}  // namespace HinaPE::FluidEngine

#endif  // HINAPE_FLUID_ENGINE_BCC_LATTICE_POINT_GENERATOR_H_