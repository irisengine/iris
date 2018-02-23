#pragma once

#include "matrix3.hpp"
#include "matrix4.hpp"
#include "quaternion.hpp"
#include "rigid_body_shape.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * A rigid body is an approximation of a physical object. It can be moved and
 * rotated but not deformed. Forces and impulses can be added and the physics
 * engine will integrate the laws of motion over time. In addition to physical
 * properties such as mass a rigid_body also has a 'shape' property which
 * defines how collision detection is performed. As this is an abstract class
 * there should be a concrete implementation for all supported shape types which
 * defines shape specific data.
 */
class rigid_body
{
    public:

        /**
         * Construct a new rigid_body.
         *
         * If a rigid_body is constructed with is_static as true then it cannot
         * be transformed and all applied forces are ignored. It effectively
         * has infinite mass.
         *
         * @param position
         *   The position in world space of the centre of mass.
         *
         * @param mass
         *   The mass of the body.
         *
         * @param inertia_tensor
         *   The inertia tensor of the body in local space. This determines the
         *   torque needed for applying angular acceleration to each of the
         *   three axis.
         *
         * @param shape
         *   The type of shape of the rigid body.
         *
         * @param is_static
         *   Is the body static.
         */
        rigid_body(
            const vector3 &position,
            const float mass,
            const matrix3 &inertia_tensor,
            const rigid_body_shape shape,
            const bool is_static);

        /**
         * Virtual destructor so class is abstract.
         */
        virtual ~rigid_body() = 0;

        /** Default */
        rigid_body(const rigid_body&) = default;
        rigid_body& operator=(const rigid_body&) = default;
        rigid_body(rigid_body&&) = default;
        rigid_body& operator=(rigid_body&&) = default;

        /**
         * Integrates the laws of motion over the specifies time period. If this
         * is a static body then this is a no-op.
         *
         * After this call any accumulated forces and torques are cleared,
         * however their effects on velocity, rotation, acceleration and
         * angular acceleration will continue.
         *
         * @param delta
         *   The time period to integrate over.
         */
        void integrate(const float delta);

        /**
         * Update the internal state of the rigid body. This is mostly used by
         * the engine and shouldn't normally need to be called manually.
         */
        void update();

        /**
         * Add a force applied to the centre of mass. Direction and magnitude
         * of force are represented by the vector direction and magnitude
         * respectively.
         *
         * After integration any forces are cleared but their effects on
         * velocity and acceleration will continue. For example applying a force
         * to a stationary object will cause it to move. After integration that
         * initial force is cleared but it will continue moving until it stops
         * (either via friction/dampening or another force is applied). To
         * apply a continual force call add_force each frame, or use
         * set_constant_acceleration.
         *
         * @param force
         *   Force to apply to centre of mass.
         */
        void add_force(const vector3 &force);

        /**
         * Add a torque applied around the centre of mass. Direction and magnitude
         * of torque are represented by yaw, pitch and roll.
         *
         * After integration any torques are cleared but their effects on
         * rotation and angular acceleration will continue. For example applying
         * a torque to a stationary object will cause it to rotate. After
         * integration that initial torque is cleared but it will continue
         * rotating until it stops (either via friction/dampening or another
         * torque is applied). To apply a continual torque call add_torque each
         * frame.
         *
         * @param torque
         *   Torque to apply to centre of mass.
         */
        void add_torque(const vector3 &torque);

        /**
         * Add an impulse applied to the centre of mass. An impulse is applied
         * instantaneously as opposed to a force which is dependant on the time
         * step. Direction and magnitude of impulse are represented by vector
         * direction and magnitude respectively.
         *
         * @param impulse
         *   Impulse to apply to centre of mass.
         *
         * @returns
         *   Change in velocity as a result of applying the impulse.
         */
        vector3 add_impulse(const vector3 &impulse);

        /**
         * Add an angular impulse applied to the centre of mass. An impulse is
         * applied instantaneously as opposed to a torque which is dependant on
         * the time step. Direction and magnitude of impulse are represented by
         * yaw, pitch and roll
         *
         * @param impulse
         *   Impulse to apply to centre of mass.
         *
         * @returns
         *   Change in angular velocity as a result of applying the impulse.
         */
        vector3 add_angular_impulse(const vector3 &impulse);

        /**
         * Set the constant acceleration of the body. The resulting acceleration
         * on the body will still be affected by other forces but is useful
         * if the body receives a constant acceleration i.e. gravity.
         *
         * @param acceleration
         *   The acceleration value to set.
         */
        void set_constant_acceleration(const vector3 &acceleration);

        /**
         * The mass of the body.
         *
         * @returns
         *   Mass of body.
         */
        float mass() const;

        /**
         * The inverse mass of the body, i.e. 1.0 / mass. Useful for some
         * calculations.
         *
         * @returns
         *   Inverse mass of body.
         */
        float inverse_mass() const;

        /**
         * Position of centre of mass in world space.
         *
         * @returns
         *   Position of centre of mass.
         */
        vector3 position() const;

        /**
         * Instantly set the position of the body centre of mass. Any forces
         * acting on the body will continue after the warp. This generally
         * isn't the method used for moving rigid bodies around the scene as
         * that should really be done with forces and impulses.
         *
         * Internally calls update.
         *
         * @param position
         *  New position of body in world coordinates.
         */
        void warp_position(const vector3 &position);

        /**
         * Get the velocity of the body.
         *
         * @returns
         *   Body velocity.
         */
        vector3 velocity() const;

        /**
         * Get the acceleration of the body.
         *
         * @returns
         *   Body acceleration.
         */
        vector3 acceleration() const;

        /*
         * Get the angular velocity of the body.
         *
         * @returns
         *   Body angular velocity.
         */
        vector3 angular_velocity() const;

        /**
         * Get the orientation of the body.
         *
         * @returns
         *   Body orientation.
         */
        quaternion orientation() const;

        /**
         * Instantly set the orientation of the body. Any torques acting on the
         * body will continue after the warp. This generally isn't the method
         * used for rotating rigid bodies around in a scene as that should
         * really be done with torques and impulses.
         *
         * Internally calls update.
         *
         * @param orientation
         *  New orientation of body.
         */
        void warp_orientation(const quaternion &orientation);

        /**
         * Get the transform matrix which describes the position and orientation
         * of this rigid body in world space.
         *
         * @returns
         *   Transformation matrix.
         */
        matrix4 transform() const;

        /**
         * Get the inverse of the objects world space inertia tensor.
         *
         * @returns
         *   Inverse of world space inertia tensor.
         */
        matrix3 inverse_inertia_tensor_world() const;

        /**
         * Returns whether the rigid body is static or not.
         *
         * @returns
         *   True if body is static.
         */
        bool is_static() const;

        /**
         * Get the shape of the rigid body.
         *
         * @returns
         *   Rigid body shape.
         */
        rigid_body_shape shape() const noexcept;

    private:

        /** Inverse mass of body. */
        float inverse_mass_;

        /** Position of the centre of mass in world space. */
        vector3 position_;

        /** Velocity of body. */
        vector3 velocity_;

        /** Orientation of body.*/
        quaternion orientation_;

        /** Angular velocity of body. */
        vector3 angular_velocity_;

        /** Acceleration of body. */
        vector3 acceleration_;

        /** Constant acceleration applied to body. */
        vector3 constant_acceleration_;

        /** The inverse inertia tensor of the body. */
        matrix3 inverse_inertia_tensor_;

        /** The inverse of the world space inertia tensor. */
        matrix3 inverse_inertia_tensor_world_;

        /** Forces accumulated since last integration. */
        vector3 force_accumulator_;

        /** Torques accumulated since last integration. */
        vector3 torque_accumulator_;

        /** Dampening to apply to velocity. */
        float linear_damping_;

        /** Dampening to apply to angular velocity. */
        float angular_damping_;

        /** The bodies position and rotation stored as transformation matrix. */
        matrix4 transform_;

        /** Whether body is static. */
        bool is_static_;

        /** Shape of rigid body. */
        rigid_body_shape shape_;
};

}

