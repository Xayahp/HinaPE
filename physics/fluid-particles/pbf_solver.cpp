#include "pbf_solver.h"

void HinaPE::PBFSolver::init()
{
	_density_constraints = std::make_shared<DensityConstraints>(_data);
	_emit_particles();
	_opt.inited = true;
}

void HinaPE::PBFSolver::update(real dt) const
{
	// TODO: PBF core
}

void HinaPE::PBFSolver::_emit_particles() const
{
	_emitter->emit(&_data->_positions, &_data->_velocities);
	_data->_forces.resize(_data->_positions.size(), mVector3::Zero());
	_data->_densities.resize(_data->_positions.size(), 0);
	_data->_pressures.resize(_data->_positions.size(), 0);
	_data->_update_neighbor();
	_data->_update_density();
}

void HinaPE::PBFSolver::_accumulate_force() const
{
}

void HinaPE::PBFSolver::_time_integration() const
{
	auto &x = _data->_positions;
	auto &v = _data->_velocities;
	auto &f = _data->_forces;
	const auto &m = _data->_mass;
	const auto &dt = _opt.current_dt;

	// semi-euler integration
	Util::parallelFor(Constant::ZeroSize, _data->_positions.size(), [&](size_t i)
	{
		v[i] += dt * f[i] / m;
		x[i] += dt * v[i];
	});
}

void HinaPE::PBFSolver::_resolve_collision() const
{
	// collide with domain
	Util::parallelFor(Constant::ZeroSize, _data->_positions.size(), [&](size_t i)
	{
		_domain->resolve_collision(_data->_radius, _opt.restitution, &_data->_positions[i], &_data->_velocities[i]);
	});
}

void HinaPE::PBFSolver::INSPECT()
{
	ImGui::Text("Solver Inspector");
	ImGui::Text("Particles: %zu", _data->_positions.size());
	INSPECT_REAL(_opt.gravity[1], "g");
	ImGui::Separator();
}

void HinaPE::PBFSolver::VALID_CHECK() const
{
	if (_data == nullptr) throw std::runtime_error("PBFSolver::_data is nullptr");
	if (_domain == nullptr) throw std::runtime_error("PBFSolver::_domain is nullptr");
	if (_emitter == nullptr) throw std::runtime_error("PBFSolver::_emitter is nullptr");

	if (_data->_positions.size() != _data->_velocities.size() &&
		_data->_positions.size() != _data->_forces.size() &&
		_data->_positions.size() != _data->_densities.size() &&
		_data->_positions.size() != _data->_pressures.size())
		throw std::runtime_error("PBFSolver::_data size mismatch");
}


HinaPE::PBFSolver::Data::Data() { track(&_positions); }

void HinaPE::PBFSolver::Data::_update_neighbor()
{
	_neighbor_search->build(_positions);
	_neighbor_lists.resize(_positions.size());

	for (size_t i = 0; i < _positions.size(); ++i)
	{
		mVector3 origin = _positions[i];
		_neighbor_lists[i].clear();

		_neighbor_search->for_each_nearby_point(origin, [&](size_t j, const mVector3 &)
		{
			if (i != j)
				_neighbor_lists[i].push_back(j);
		});
	}
}

void HinaPE::PBFSolver::Data::_update_density()
{
	if (!_mass_inited)
		_update_mass(); // update mass to ensure the initial density is 1000

	auto &x = _positions;
	auto &d = _densities;
	const auto &m = _mass;

	Util::parallelFor(Constant::ZeroSize, _positions.size(), [&](size_t i)
	{
		real sum = 0;
		for (int j = 0; j < _neighbor_lists[i].size(); ++j)
		{
			real dist = (x[i] - x[_neighbor_lists[i][j]]).length();
			sum += (*poly6_kernel)(dist);
		}
		d[i] = m * sum; // rho(x) = m * sum(W(x - xj))
	});
}

void HinaPE::PBFSolver::Data::_update_mass()
{
	// TODO: to be rewritten
	_mass = 1.0;

	real max_number_density = 0;
	for (int i = 0; i < _positions.size(); ++i)
	{
		real sum = 0;
		const auto &point = _positions[i];
		for (const auto &neighbor_point_id: _neighbor_lists[i])
		{
			auto dist = (point - _positions[neighbor_point_id]).length();
			sum += (*poly6_kernel)(dist);
		}
		max_number_density = std::max(max_number_density, sum);
	}

	if (max_number_density > 0)
		_mass = std::max((target_density / max_number_density), HinaPE::Constant::Zero);

	_mass_inited = true;
}

void HinaPE::PBFSolver::Data::INSPECT()
{
	PoseBase::INSPECT();
	if (_inst_id >= 0 && _inst_id < _densities.size())
	{
		ImGui::Text("Inst: %d", _inst_id);
		ImGui::Text("Force: {%.3f, %.3f, %.3f}", _forces[_inst_id].x(), _forces[_inst_id].y(), _forces[_inst_id].z());
		ImGui::Text("Density: %.3f", _densities[_inst_id]);
		ImGui::Text("Pressure: %.3f", _pressures[_inst_id]);
		ImGui::Text("Lambda: %.3f", _lambdas[_inst_id]);
		ImGui::Text("Neighbors: %zu", _neighbor_lists[_inst_id].size());
		ImGui::Text("dp: {%.3f, %.3f, %.3f}", _delta_p[_inst_id].x(), _delta_p[_inst_id].y(), _delta_p[_inst_id].z());
	}
}

void HinaPE::PBFSolver::DensityConstraints::solve() const
{
	// Note:
	// "i" is the index of the current particle,
	// "j" is the index of the neighbor particle

	const size_t size = _data->_positions.size();
	const real d0 = _data->target_density;
	const real eps = 1e-6;

	const auto &x = _data->_positions;
	const auto &d = _data->_densities;
	const auto &nl = _data->_neighbor_lists;
	const auto &kernel = _data->poly6_kernel;


	// First, we compute all lambdas
	auto &lambdas = _data->_lambdas;
	lambdas.resize(size, 0); // initialize lambdas to zero
	Util::parallelFor(Constant::ZeroSize, size, [&lambdas, &x, &d, &nl, &kernel, d0, eps](size_t i)
	{
		real d_i = d[i];
		real C_i = d_i / d0 - 1;

		if (C_i > 0) // if density is bigger than water density, do constraints projection
		{
			real sum_grad_C_i_squared = 0;
			mVector3 grad_C_i = mVector3::Zero();

			for (const auto j: nl[i])
			{
				const auto p_i = x[i];
				const auto p_j = x[j];
				const mVector3 grad_C_j = -(*kernel).gradient(p_i - p_j);

				// Equation (8)
				sum_grad_C_i_squared += grad_C_j.length_squared();
				grad_C_i -= grad_C_j;
			}

			sum_grad_C_i_squared += grad_C_i.length_squared();

			// Equation (11): compute lambda
			real lambda = -C_i / (sum_grad_C_i_squared + eps);
			lambdas[i] = lambda; // thread safe write
		}
	});


	// Second, we compute all correction delta p
	auto &dp = _data->_delta_p;
	dp.resize(size, mVector3::Zero()); // initialize delta p to zero vector

	Util::parallelFor(Constant::ZeroSize, size, [&dp, &lambdas, &x, &nl, &kernel](size_t i)
	{
		const auto &lambda_i = lambdas[i];

		// Equation (12): compute delta p
		mVector3 delta_p_i = mVector3::Zero();
		for (const auto j: nl[i])
		{
			const auto &lambda_j = lambdas[j];
			const auto p_i = x[i];
			const auto p_j = x[j];
			const mVector3 grad_C_j = -(*kernel).gradient(p_i - p_j);
			delta_p_i -= (lambda_i + lambda_j) * grad_C_j;
		}
		dp[i] = delta_p_i; // thread safe write
	});

	// Finally, apply delta p to all particles
	auto &x_to_write = _data->_positions;
	Util::parallelFor(Constant::ZeroSize, size, [&x_to_write, &dp](size_t i)
	{
		x_to_write[i] += dp[i];
	});
}
