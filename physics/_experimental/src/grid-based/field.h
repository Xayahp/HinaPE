#ifndef HINAPE_FIELD_H
#define HINAPE_FIELD_H

#include "base/base.h"

namespace Hina
{
class Field3 {};
class ScalarField3 : public Field3
{
public:
	virtual auto sample(const mVector3 &x) const -> real = 0;
	virtual auto sampler() const -> std::function<real(const mVector3 &)> = 0;
	virtual auto gradient(const mVector3 &x) const -> mVector3 = 0;
	virtual auto laplacian(const mVector3 &x) const -> real = 0;
};
class ConstantScalarField3 final : public ScalarField3
{
public:
	auto sample(const mVector3 &x) const -> real final { return _value; }
	auto gradient(const mVector3 &x) const -> mVector3 final { return mVector3::Zero(); }
	auto laplacian(const mVector3 &x) const -> real final { return Constant::Zero; }
	auto sampler() const -> std::function<real(const mVector3 &)> final { return [this](const mVector3 &) { return _value; }; }
private:
	real _value = Constant::Zero;
};
class VectorField3 : public Field3
{
public:
	virtual auto sample(const mVector3 &x) const -> mVector3 = 0;
	virtual auto sampler() const -> std::function<mVector3(const mVector3 &)> = 0;
	virtual auto divergence(const mVector3 &x) const -> real = 0;
	virtual auto curl(const mVector3 &x) const -> mVector3 = 0;
};
class ConstantVectorField3 final : public VectorField3
{
public:
	auto sample(const mVector3 &x) const -> mVector3 final { return _value; }
	auto divergence(const mVector3 &x) const -> real final { return Constant::Zero; }
	auto curl(const mVector3 &x) const -> mVector3 final { return mVector3::Zero(); }
	auto sampler() const -> std::function<mVector3(const mVector3 &)> final { return [this](const mVector3 &) -> mVector3 { return _value; }; }
private:
	mVector3 _value = mVector3::Zero();
};
using Field3Ptr = std::shared_ptr<Field3>;
using ScalarField3Ptr = std::shared_ptr<ScalarField3>;
using VectorField3Ptr = std::shared_ptr<VectorField3>;
using ConstantScalarField3Ptr = std::shared_ptr<ConstantScalarField3>;
using ConstantVectorField3Ptr = std::shared_ptr<ConstantVectorField3>;
}

#endif //HINAPE_FIELD_H
