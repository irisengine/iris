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
    float light_data[4];
    float time;
    char padding[12];
};

struct DirectionalLightConstantBuffer
{
    Matrix4 proj;
    Matrix4 view;
};

}
