#ifndef HINAPE_CLOTH_SOLVER_H
#define HINAPE_CLOTH_SOLVER_H

#include "common.h"
#include "constraints.h"

namespace HinaPE
{
class PBDClothSolver : public CopyDisable
{
public:
	void step(real dt);

public:
	struct Opt
	{
		float time_step = 0.02f;
		float gravity = -9.8f;
		float damping = 0.99f;

		bool distance_constraint = true;
		float distance_constraint_stiffness = 0.9f;
	} _opt;
	PBDClothSolver();

protected:
	void _prepare();
	void _external_force();
	void _time_integration();
	void _constraint_projection();
	void _update_state();

public:
	struct Data;
	std::shared_ptr<Data> _data;
	std::vector<Constraint> _constraints;
	real _current_dt;
};

struct PBDClothSolver::Data : public CopyDisable
{
public:
	struct Opt
	{
		real width = 2.0f;
		real height = 2.0f;
		int rows = 30;
		int cols = 30;
	} _opt;
	void _sync_opt();

	// init infos
	std::vector<real> _inv_masses;
	std::vector<mVector3> _init_vertices;
	std::vector<unsigned int> _init_indices;
	std::vector<std::pair<unsigned int, unsigned int>> _init_edges;

	// update infos
	std::vector<mVector3> _positions;
	std::vector<mVector3> _pre_positions;
	std::vector<mVector3> _velocities;
	std::vector<mVector3> _forces;
};

using PBDClothSolverPtr = std::shared_ptr<PBDClothSolver>;
using PBDClothSolverDataPtr = std::shared_ptr<PBDClothSolver::Data>;
} // namespace HinaPE

#endif //HINAPE_CLOTH_SOLVER_H
