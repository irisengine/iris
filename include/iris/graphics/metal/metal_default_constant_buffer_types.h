////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/matrix4.h"

namespace iris
{

// this file Fontaine's various structs which map to MSL structs.

struct DefaultConstantBuffer
{
    Matrix4 projection;
    Matrix4 view;
    Matrix4 model;
    Matrix4 normal_matrix;
    Matrix4 bones[100];
    float camera[4];
    float light_position[4];
    float light_world_space[4];
    float light_attenuation[3];
    float time;
    char padding[48];
};

struct DirectionalLightConstantBuffer
{
    Matrix4 proj;
    Matrix4 view;
};

}
