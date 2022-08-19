////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

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

Bone::Bone(const std::string &name, const std::string &parent, const Matrix4 &offset, const Matrix4 &transform)
    : name_(name)
    , parent_(parent)
    , offset_(offset)
    , transform_(transform)
    , is_manual_(false)
{
}

std::string Bone::name() const
{
    return name_;
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
