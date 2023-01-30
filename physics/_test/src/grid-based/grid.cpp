#include "grid.h"

void Hina::Grid3::resize(const Hina::Base::Size3 &resolution, const Hina::Base::mVector3 &grid_spacing, const Hina::Base::mVector3 &origin)
{
	_opt.resolution = resolution;
	_opt.grid_spacing = grid_spacing;
	_opt.origin = origin;
	_opt.bounding_box = Hina::Base::mBBox3(
			origin,
			origin + grid_spacing * Hina::Base::mVector3(
					static_cast<Base::real>(resolution.x),
					static_cast<Base::real>(resolution.y),
					static_cast<Base::real>(resolution.z)));
}
void Hina::ScalarGrid3::resize(const Hina::Base::Size3 &resolution, const Hina::Base::mVector3 &grid_spacing, const Hina::Base::mVector3 &origin, Hina::Base::real initial_value)
{
	Grid3::resize(resolution, grid_spacing, origin);
	_data.resize(size(), initial_value);
}
void Hina::VectorGrid3::resize(const Hina::Base::Size3 &resolution, const Hina::Base::mVector3 &grid_spacing, const Hina::Base::mVector3 &origin, const Hina::Base::mVector3 &initial_value)
{
	Grid3::resize(resolution, grid_spacing, origin);
	on_resize(resolution, grid_spacing, origin, initial_value);
}
void Hina::CollocatedVectorGrid3::on_resize(const Hina::Base::Size3 &resolution, const Hina::Base::mVector3 &grid_spacing, const Hina::Base::mVector3 &origin, const Hina::Base::mVector3 &initial_value)
{
}
void Hina::FaceCenteredVectorGrid3::on_resize(const Hina::Base::Size3 &resolution, const Hina::Base::mVector3 &grid_spacing, const Hina::Base::mVector3 &origin, const Hina::Base::mVector3 &initial_value)
{
}
