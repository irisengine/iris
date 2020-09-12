#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <type_traits>

#include "core/quaternion.h"
#include "core/vector3.h"
#include "physics/character_controller.h"
#include "physics/rigid_body.h"

namespace iris
{

// forward declare opaque type of storing physics state
// this will be defined in the PhysicsSystem implementation
struct PhysicsState;

// must also forward declare a deleter for PhysicsState, otherwise we will be
// returning a unique_ptr of incomplete type
struct PhysicsStateDeleter
{
    void operator()(PhysicsState *state);
};

/**
 * Class for handling physics simulations.
 */
class PhysicsSystem
{
    public:

        // helper traits
        template<class T>
        using is_character_controller = std::enable_if_t<std::is_base_of_v<CharacterController, T>>;

        template<class T>
        using is_rigid_body = std::enable_if_t<std::is_base_of_v<RigidBody, T>>;

        /**
         * Create a new physics system.
         */
        PhysicsSystem();

        /** Declared in cpp file as implementation is an incomplete file. */
        ~PhysicsSystem();

        // disabled
        PhysicsSystem(const PhysicsSystem&) = delete;
        PhysicsSystem& operator=(const PhysicsSystem&) = delete;

        /**
         * Step the physics system by the supplied time.
         *
         * @param time_step
         *   The amount of time to simulate.
         */
        void step(std::chrono::milliseconds time_step);

        /**
         * Create a RigidBody and add it to the simulation. Uses perfect
         * forwarding to pass along all arguments.
         *
         * @param args
         *   Arguments for RigidBody.
         *
         * @returns
         *   A pointer to the newly created RigidBody.
         */
        template<class T, class ...Args, typename=is_rigid_body<T>>
        RigidBody* create_rigid_body(Args &&...args)
        {
            auto element = std::make_unique<T>(std::forward<Args>(args)...);
            return add(std::move(element));
        }

        /**
         * Create a CharacterController and add it to the simulation. Uses perfect
         * forwarding to pass along all arguments.
         *
         * @param args
         *   Arguments for CharacterController.
         *
         * @returns
         *   A pointer to the newly created CharacterController.
         */
        template<class T, class ...Args, typename=is_character_controller<T>>
        CharacterController* create_character_controller(Args &&...args)
        {
            auto element = std::make_unique<T>(std::forward<Args>(args)...);
            return add(std::move(element));
        }

        /**
         * Add a RigidBody to the simulation.
         *
         * @param body
         *   Rigid body to add.
         *
         * @returns
         *   Pointer to the added body.
         */
        RigidBody* add(std::unique_ptr<RigidBody> body);

        /**
         * Add a CharacterController to the simulation.
         *
         * @param character
         *   Character controller to add.
         *
         * @returns
         *   Pointer to the added character controller.
         */
        CharacterController* add(std::unique_ptr<CharacterController> character);

        /**
         * Cast a ray into physics engine world.
         *
         * @param origin
         *   Origin of ray.
         *
         * @param direction.
         *   Direction of ray.
         *
         * @returns
         *   If ray hits an object then the point of intersection, else empty
         *   optional.
         */
        std::optional<Vector3> ray_cast(
            const Vector3 &origin,
            const Vector3 &direction) const;

        /**
         * Save the current state of the simulation.
         * 
         * Note that depending on the implementation this may be a "best guess"
         * state save. Restoring isn't guaranteed to produce identical results
         * although it should be close enough.
         * 
         * @returns
         *   Saved state.
         */
        std::unique_ptr<PhysicsState, PhysicsStateDeleter> save();

        /**
         * Load saved state. This will restore the simulation to that of the
         * supplied state.
         * 
         * See save() comments for details of limitations.
         * 
         * @param state
         *   State to restore from.
         */
        void load(const PhysicsState *state);

    private:

        /** Physics API implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}
