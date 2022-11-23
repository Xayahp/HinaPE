#ifndef HINAPE_FLUID_PARTICLE_EMITTER3_H_
#define HINAPE_FLUID_PARTICLE_EMITTER3_H_

#include "animation/animation.h"
#include "solver/particle/particle_system_data3.h"

namespace HinaPE::Fluid
{

//!
//! \brief Abstract base class for 3-D particle emitter.
//!
class ParticleEmitter3
{
public:
    //!
    //! \brief Callback function type for update calls.
    //!
    //! This type of callback function will take the emitter pointer, current
    //! time, and time interval in seconds.
    //!
    using OnBeginUpdateCallback = std::function<void (ParticleEmitter3 *, double, double)>;

    //! Default constructor.
    ParticleEmitter3();

    //! Destructor.
    virtual ~ParticleEmitter3();

    //! Updates the emitter state from \p currentTimeInSeconds to the following
    //! time-step.
    void update(double currentTimeInSeconds, double timeIntervalInSeconds);

    //! Returns the target particle system to emit.
    auto target() const -> const ParticleSystemData3Ptr &;

    //! Sets the target particle system to emit.
    void setTarget(const ParticleSystemData3Ptr &particles);

    //! Returns true if the emitter is enabled.
    auto isEnabled() const -> bool;

    //! Sets true/false to enable/disable the emitter.
    void setIsEnabled(bool enabled);

    //!
    //! \brief      Sets the callback function to be called when
    //!             ParticleEmitter3::update function is invoked.
    //!
    //! The callback function takes current simulation time in seconds unit. Use
    //! this callback to track any motion or state changes related to this
    //! emitter.
    //!
    //! \param[in]  callback The callback function.
    //!
    void setOnBeginUpdateCallback(const OnBeginUpdateCallback &callback);

protected:
    //! Called when ParticleEmitter3::setTarget is executed.
    virtual void onSetTarget(const ParticleSystemData3Ptr &particles);

    //! Called when ParticleEmitter3::update is executed.
    virtual void onUpdate(double currentTimeInSeconds, double timeIntervalInSeconds) = 0;

private:
    bool _isEnabled = true;
    ParticleSystemData3Ptr _particles;
    OnBeginUpdateCallback _onBeginUpdateCallback;
};

//! Shared pointer for the ParticleEmitter3 type.
using ParticleEmitter3Ptr = std::shared_ptr<ParticleEmitter3>;

}  // namespace HinaPE::Fluid

#endif  // HINAPE_FLUID_PARTICLE_EMITTER3_H_
