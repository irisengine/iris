#include "graphics/bone.h"

#include <cstddef>
#include <limits>
#include <string>
#include <vector>

#include "core/matrix4.h"
#include "core/transform.h"
#include "graphics/weight.h"

namespace iris
{

Bone::Bone(
    const std::string &name,
    const std::string &parent,
    const std::vector<Weight> &weights,
    const Matrix4 &offset,
    const Matrix4 &transform)
    : name_(name)
    , parent_(parent)
    , weights_(weights)
    , offset_(offset)
    , transform_(transform)
    , is_manual_(false)
{
}

std::string Bone::name() const
{
    return name_;
}

const std::vector<Weight> &Bone::weights() const
{
    return weights_;
}

const Matrix4 &Bone::offset() const
{
    return offset_;
}

std::string Bone::parent() const
{
    return parent_;
}

const Matrix4 &Bone::transform() const
{
    return transform_;
}

void Bone::set_transform(const Matrix4 &transform)
{
    transform_ = transform;
}

bool Bone::is_manual() const
{
    return is_manual_;
}

void Bone::set_manual(bool is_manual)
{
    is_manual_ = is_manual;
}

}
