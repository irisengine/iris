////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "physics/bullet/debug_draw.h"

#include <cstdint>
#include <vector>

#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionShapes/btTriangleCallback.h>
#include <btBulletDynamicsCommon.h>

#include "core/colour.h"
#include "core/error_handling.h"
#include "core/exception.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"
#include "graphics/vertex_data.h"
#include "physics/bullet/bullet_box_collision_shape.h"
#include "physics/bullet/bullet_capsule_collision_shape.h"
#include "physics/bullet/bullet_heightmap_collision_shape.h"
#include "physics/bullet/bullet_mesh_collision_shape.h"

namespace
{

/**
 * Bullet provides a callback class for debug drawing triangle meshes, however it is an internal class. We copy and
 * paste it here:
 * https://github.com/bulletphysics/bullet3/blob/06a212e66724f67f8abc8e927ac11facb063059a/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp#L1260-L1298
 */
class DebugDrawcallback : public btTriangleCallback, public btInternalTriangleIndexCallback
{
    btIDebugDraw *m_debugDrawer;
    btVector3 m_color;
    btTransform m_worldTrans;

  public:
    DebugDrawcallback(btIDebugDraw *debugDrawer, const btTransform &worldTrans, const btVector3 &color)
        : m_debugDrawer(debugDrawer)
        , m_color(color)
        , m_worldTrans(worldTrans)
    {
    }

    virtual void internalProcessTriangleIndex(btVector3 *triangle, int partId, int triangleIndex)
    {
        processTriangle(triangle, partId, triangleIndex);
    }

    virtual void processTriangle(btVector3 *triangle, int partId, int triangleIndex)
    {
        (void)partId;
        (void)triangleIndex;

        btVector3 wv0, wv1, wv2;
        wv0 = m_worldTrans * triangle[0];
        wv1 = m_worldTrans * triangle[1];
        wv2 = m_worldTrans * triangle[2];
        btVector3 center = (wv0 + wv1 + wv2) * btScalar(1. / 3.);

        if (m_debugDrawer->getDebugMode() & btIDebugDraw::DBG_DrawNormals)
        {
            btVector3 normal = (wv1 - wv0).cross(wv2 - wv0);
            normal.normalize();
            btVector3 normalColor(1, 1, 0);
            m_debugDrawer->drawLine(center, center + normal, normalColor);
        }
        m_debugDrawer->drawLine(wv0, wv1, m_color);
        m_debugDrawer->drawLine(wv1, wv2, m_color);
        m_debugDrawer->drawLine(wv2, wv0, m_color);
    }
};

}

namespace iris
{

DebugDraw::DebugDraw()
    : scene_(nullptr)
    , bullet_debug_draw_()
    , meshes_()
{
}

void DebugDraw::update()
{
    // if no scene has been set then we do not want debug drawing
    if (scene_ != nullptr)
    {
        for (auto &[body, entity] : bodies_)
        {
            expect(entity != nullptr, "corrupt debug state");

            entity->set_position(body->position());
            entity->set_orientation(body->orientation());
        }
    }
}

void DebugDraw::set_scene(Scene *scene)
{
    scene_ = scene;

    // add any previously registered objects to the scene
    for (auto &[body, entity] : bodies_)
    {
        entity = scene_->create_entity<SingleEntity>(
            nullptr,
            meshes_[body->collision_shape()].get(),
            Transform{body->position(), body->orientation(), {1.0f, 1.0f, 1.0f}},
            PrimitiveType::LINES);
    }
}

void DebugDraw::register_box_collision_shape(const BulletBoxCollisionShape *collision_shape)
{
    const auto half_size = collision_shape->half_size();
    bullet_debug_draw_.drawBox(
        {-half_size.x, -half_size.y, -half_size.z}, {half_size.x, half_size.y, half_size.z}, {0.0f, 1.0f, 0.0f});

    meshes_[collision_shape] = bullet_debug_draw_.flush();
}

void DebugDraw::register_capsule_collision_shape(const BulletCapsuleCollisionShape *collision_shape)
{
    bullet_debug_draw_.drawCapsule(
        collision_shape->width(),
        collision_shape->height(),
        1,
        btTransform{{{0.0f, 1.0f, 0.0f}, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {0.0f, 1.0f, 0.0f});

    meshes_[collision_shape] = bullet_debug_draw_.flush();
}

void DebugDraw::register_mesh_collision_shape(const BulletMeshCollisionShape *collision_shape)
{
    const auto *bullet_shape = static_cast<btTriangleMeshShape *>(collision_shape->handle());
    DebugDrawcallback callback{
        &bullet_debug_draw_, btTransform{{{0.0f, 1.0f, 0.0f}, 0.0f}, {0.0f, 0.0f, 0.0f}}, {0.0f, 1.0f, 0.0f}};
    bullet_shape->processAllTriangles(&callback, {-10000.0f, -10000.0f, -10000.0f}, {10000.0f, 10000.0f, 10000.0f});

    meshes_[collision_shape] = bullet_debug_draw_.flush();
}

void DebugDraw::register_height_map_collision_shape(const BulletHeightmapCollisionShape *collision_shape)
{
    const auto *bullet_shape = static_cast<btHeightfieldTerrainShape *>(collision_shape->handle());
    DebugDrawcallback callback{
        &bullet_debug_draw_, btTransform{{{0.0f, 1.0f, 0.0f}, 0.0f}, {0.0f, 0.0f, 0.0f}}, {0.0f, 1.0f, 0.0f}};
    bullet_shape->processAllTriangles(&callback, {-10000.0f, -10000.0f, -10000.0f}, {10000.0f, 10000.0f, 10000.0f});

    meshes_[collision_shape] = bullet_debug_draw_.flush();
}

void DebugDraw::register_rigid_body(const RigidBody *rigid_body)
{
    const auto &[iter, _] = bodies_.emplace(rigid_body, nullptr);

    if (scene_ != nullptr)
    {
        iter->second = scene_->create_entity<SingleEntity>(
            nullptr,
            meshes_[rigid_body->collision_shape()].get(),
            Transform{rigid_body->position(), rigid_body->orientation(), {1.0f, 1.0f, 1.0f}},
            PrimitiveType::LINES);
    }
}

void DebugDraw::deregister_rigid_body(const RigidBody *rigid_body)
{
    if (scene_ != nullptr)
    {
        scene_->remove(bodies_[rigid_body]);
        bodies_.erase(rigid_body);
    }
}

}
