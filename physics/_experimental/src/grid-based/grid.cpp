#include "grid.h"

void Hina::Grid3::resize(const Hina::Base::Size3 &resolution, const Hina::mVector3 &grid_spacing, const Hina::mVector3 &origin)
{
	_opt.resolution = resolution;
	_opt.grid_spacing = grid_spacing;
	_opt.origin = origin;
	_opt.bounding_box = Hina::mBBox3(
			origin,
			origin + grid_spacing * Hina::mVector3(
					static_cast<real>(resolution.x),
					static_cast<real>(resolution.y),
					static_cast<real>(resolution.z)));
}
auto Hina::Grid3::cell_center_position() const -> Hina::Grid3::DataPositionFunc
{
	mVector3 h = _opt.grid_spacing;
	mVector3 o = _opt.origin;
	return [h, o](size_t i, size_t j, size_t k) { return o + h * mVector3(static_cast<real>(i) + 0.5, static_cast<real>(j) + 0.5, static_cast<real>(k) + 0.5); };
}
void Hina::Grid3::for_each_cell_index(const std::function<void(size_t, size_t, size_t)> &func) const
{
	for (size_t k = 0; k < _opt.resolution.z; ++k)
		for (size_t j = 0; j < _opt.resolution.y; ++j)
			for (size_t i = 0; i < _opt.resolution.x; ++i)
				func(i, j, k);
}
void Hina::Grid3::parallel_for_each_cell_index(const std::function<void(size_t, size_t, size_t)> &func) const
{
	Base::parallelFor((size_t) 0, _opt.resolution.x, (size_t) 0, _opt.resolution.y, (size_t) 0, _opt.resolution.z, [&func](size_t i, size_t j, size_t k) { func(i, j, k); }); // TODO: why another lambda
}


void Hina::ScalarGrid3::resize(const Hina::Base::Size3 &resolution, const Hina::mVector3 &grid_spacing, const Hina::mVector3 &origin, Hina::real initial_value)
{
	// update opt
	_opt.resolution = resolution;
	_opt.grid_spacing = grid_spacing;
	_opt.origin = origin;
	_opt.bounding_box = mBBox3(origin, origin + grid_spacing * mVector3(static_cast<real>(resolution.x), static_cast<real>(resolution.y), static_cast<real>(resolution.z)));

	// update data
	Grid3::resize(resolution, grid_spacing, origin);
	_data.resize(data_size(), initial_value);
}


void Hina::VectorGrid3::resize(const Hina::Base::Size3 &resolution, const Hina::mVector3 &grid_spacing, const Hina::mVector3 &origin, const Hina::mVector3 &initial_value)
{
	Grid3::resize(resolution, grid_spacing, origin);
	on_resize(resolution, grid_spacing, origin, initial_value);
}
void Hina::CollocatedVectorGrid3::on_resize(const Hina::Base::Size3 &resolution, const Hina::mVector3 &grid_spacing, const Hina::mVector3 &origin, const Hina::mVector3 &initial_value)
{
}
void Hina::FaceCenteredVectorGrid3::on_resize(const Hina::Base::Size3 &resolution, const Hina::mVector3 &grid_spacing, const Hina::mVector3 &origin, const Hina::mVector3 &initial_value)
{
}
auto Hina::FaceCenteredVectorGrid3::value_at_cell_center(size_t i, size_t j, size_t k) const -> Hina::mVector3
{
	return Constant::Half * mVector3(_u_data(i, j, k) + _u_data(i + 1, j, k),
									 _v_data(i, j, k) + _v_data(i, j + 1, k),
									 _w_data(i, j, k) + _w_data(i, j, k + 1));
}
auto Hina::FaceCenteredVectorGrid3::divergence_at_cell_center(size_t i, size_t j, size_t k) const -> Hina::real
{
	return (_u_data(i + 1, j, k) - _u_data(i, j, k)) / _opt.grid_spacing.x() +
		   (_v_data(i, j + 1, k) - _v_data(i, j, k)) / _opt.grid_spacing.y() +
		   (_w_data(i, j, k + 1) - _w_data(i, j, k)) / _opt.grid_spacing.z();
}
auto Hina::FaceCenteredVectorGrid3::curl_at_cell_center(size_t i, size_t j, size_t k) const -> Hina::mVector3
{
	const Base::Size3 &res = _opt.resolution;
	const mVector3 &gs = _opt.grid_spacing;

	mVector3 left = value_at_cell_center((i > 0) ? i - 1 : i, j, k);
	mVector3 right = value_at_cell_center((i + 1 < res.x) ? i + 1 : i, j, k);
	mVector3 down = value_at_cell_center(i, (j > 0) ? j - 1 : j, k);
	mVector3 up = value_at_cell_center(i, (j + 1 < res.y) ? j + 1 : j, k);
	mVector3 back = value_at_cell_center(i, j, (k > 0) ? k - 1 : k);
	mVector3 front = value_at_cell_center(i, j, (k + 1 < res.z) ? k + 1 : k);

	real Fx_ym = down.x();
	real Fx_yp = up.x();
	real Fx_zm = back.x();
	real Fx_zp = front.x();
	real Fy_xm = left.y();
	real Fy_xp = right.y();
	real Fy_zm = back.y();
	real Fy_zp = front.y();
	real Fz_xm = left.z();
	real Fz_xp = right.z();
	real Fz_ym = down.z();
	real Fz_yp = up.z();

	return {(Fz_yp - Fz_ym) / (2 * gs.y()) - (Fy_zp - Fy_zm) / (2 * gs.z()),
			(Fx_zp - Fx_zm) / (2 * gs.z()) - (Fz_xp - Fz_xm) / (2 * gs.x()),
			(Fy_xp - Fy_xm) / (2 * gs.x()) - (Fx_yp - Fx_ym) / (2 * gs.y())};
}
