#include "solver.h"

HinaPE::PBDClothSolver::PBDClothSolver() : _current_dt(0.2f)
{
	_data = std::make_shared<Data>();
	_data->_build_();
	_prepare();
}
void HinaPE::PBDClothSolver::step(real dt)
{
	_current_dt = dt;
	_external_force();
	_time_integration();
	_constraint_projection();
	_update_state();
}
void HinaPE::PBDClothSolver::_prepare()
{
	if (_opt.distance_constraint)
	{
		_constraints.emplace_back(std::make_shared<DistanceConstraint>(_data->_positions, _data->_inv_masses, _data->_init_edges));
	}
}
void HinaPE::PBDClothSolver::_external_force() const
{
	for (auto &_force: _data->_forces)
		_force += mVector3(0.f, _opt.gravity, 0.f);
}
void HinaPE::PBDClothSolver::_time_integration() const
{
	auto &positions = _data->_positions;
	auto &velocities = _data->_velocities;
	auto &forces = _data->_forces;
	auto &inv_masses = _data->_inv_masses;

	// update velocity
	for (auto i = 0; i < positions.size(); ++i)
	{
		if (inv_masses[i] == 0.f)
			continue;
		velocities[i] += _opt.damping * forces[i] * inv_masses[i] * _current_dt;
	}

	// update position
	for (auto i = 0; i < positions.size(); ++i)
		positions[i] += velocities[i] * _current_dt;

	// simple collision
	for (auto i = 0; i < positions.size(); ++i)
		if (positions[i].y() < -6.3)
		{
			positions[i].y() = -6.3;
			velocities[i].y() = 0.f;
		}
}
void HinaPE::PBDClothSolver::_constraint_projection()
{
	for (auto &c: _constraints)
		c->solve();
}
void HinaPE::PBDClothSolver::_update_state()
{
	auto &positions = _data->_positions;
	auto &pre_positions = _data->_pre_positions;
	auto &velocities = _data->_velocities;

	// update velocity
	for (auto i = 0; i < positions.size(); ++i)
	{
		velocities[i] = (positions[i] - pre_positions[i]) / _current_dt;
		pre_positions[i] = positions[i];
	}

	// clear forces
	auto &forces = _data->_forces;
	std::fill(forces.begin(), forces.end(), mVector3(0.f, 0.f, 0.f));
}
void HinaPE::PBDClothSolver::Data::_build_()
{
	_inv_masses.clear();
	_init_vertices.clear();
	_init_indices.clear();
	_init_edges.clear();
	_positions.clear();
	_pre_positions.clear();
	_velocities.clear();
	_forces.clear();

	// init geometry data.
	auto &_vertices = _init_vertices;
	auto &_indices = _init_indices;
	auto &_edges = _init_edges;

	_vertices.resize(_opt.cols * _opt.rows);
	_indices.resize((_opt.cols - 1) * (_opt.rows - 1) * 6);
	for (int j = 0; j < _opt.rows; ++j)
		for (int i = 0; i < _opt.cols; ++i)
			_vertices[j * _opt.cols + i] = mVector3(_opt.width * (static_cast<real>(i) / (static_cast<real>(_opt.cols) - 1.f)), _opt.height * (static_cast<real>(j) / (static_cast<real>(_opt.rows) - 1.f)), 0.f) - mVector3{_opt.width / 2.f, _opt.height / 2.f, 0};
	for (int j = 0; j < _opt.rows - 1; ++j)
	{
		for (int i = 0; i < _opt.cols - 1; ++i)
		{
			_indices[(j * (_opt.cols - 1) + i) * 6 + 0] = j * _opt.cols + i;
			_indices[(j * (_opt.cols - 1) + i) * 6 + 1] = j * _opt.cols + i + 1;
			_indices[(j * (_opt.cols - 1) + i) * 6 + 2] = (j + 1) * _opt.cols + i;
			_indices[(j * (_opt.cols - 1) + i) * 6 + 3] = (j + 1) * _opt.cols + i;
			_indices[(j * (_opt.cols - 1) + i) * 6 + 4] = j * _opt.cols + i + 1;
			_indices[(j * (_opt.cols - 1) + i) * 6 + 5] = (j + 1) * _opt.cols + i + 1;
		}
	}
	for (int i = 0; i < _indices.size(); i += 3)
	{
		std::pair<unsigned int, unsigned int> e1 = std::make_pair(_indices[i], _indices[i + 1]);
		std::pair<unsigned int, unsigned int> e2 = std::make_pair(_indices[i + 1], _indices[i + 2]);
		std::pair<unsigned int, unsigned int> e3 = std::make_pair(_indices[i + 2], _indices[i]);
		if (std::none_of(_edges.begin(), _edges.end(), [&e1](const std::pair<unsigned int, unsigned int> &e) { return (e.first == e1.first && e.second == e1.second) || (e.first == e1.second && e.second == e1.first); }))
			_edges.emplace_back(e1);
		if (std::none_of(_edges.begin(), _edges.end(), [&e2](const std::pair<unsigned int, unsigned int> &e) { return (e.first == e2.first && e.second == e2.second) || (e.first == e2.second && e.second == e2.first); }))
			_edges.emplace_back(e2);
		if (std::none_of(_edges.begin(), _edges.end(), [&e3](const std::pair<unsigned int, unsigned int> &e) { return (e.first == e3.first && e.second == e3.second) || (e.first == e3.second && e.second == e3.first); }))
			_edges.emplace_back(e3);
	}

	// init physics data

	_positions.resize(_vertices.size());
	_pre_positions.resize(_vertices.size());
	_velocities.resize(_vertices.size());
	_forces.resize(_vertices.size());
	_inv_masses.resize(_vertices.size());
	std::copy(_vertices.begin(), _vertices.end(), _positions.begin());
	_pre_positions = _positions;
	std::fill(_velocities.begin(), _velocities.end(), mVector3(0.f, 0.f, 0.f));
	std::fill(_forces.begin(), _forces.end(), mVector3(0.f, 0.f, 0.f));
	std::fill(_inv_masses.begin(), _inv_masses.end(), 1.f);
	_inv_masses[_opt.cols * (_opt.rows - 1)] = 0;
	_inv_masses[_opt.cols * _opt.rows - 1] = 0;
}