#include "particle_emitter.h"

HinaPE::PointParticleEmitter3::PointParticleEmitter3() : _rng(0) {}
void HinaPE::PointParticleEmitter3::emit(std::vector<mVector3> *positions, std::vector<mVector3> *velocities)
{
	if (!ParticleEmitter3::_opt.enable || ParticleEmitter3::_opt.remaining_particles <= 0)
		return;

	auto rdm = [&]() -> real
	{
		std::uniform_real_distribution<> d(0.0, 1.0);
		return d(_rng);
	};

	for (int i = 0; i < ParticleEmitter3::_opt.particles_at_once; ++i)
	{
		auto new_dir = Math::uniform_sample_cone(rdm(), rdm(), _direction, ParticleEmitter3::_opt.spread_angle);
		positions->push_back(_origin);
		velocities->push_back(ParticleEmitter3::_opt.speed * new_dir);
	}
	ParticleEmitter3::_opt.remaining_particles -= ParticleEmitter3::_opt.particles_at_once;
}