#include "physics_system.hpp"

#include <iterator>
#include <memory>
#include <vector>

#include "collision_detector.hpp"
#include "collision_resolver.hpp"
#include "contact.hpp"
#include "log.hpp"
#include "rigid_body.hpp"

namespace eng
{

void physics_system::add(std::shared_ptr<rigid_body> body)
{
    bodies_.emplace_back(body);

    LOG_INFO("physics_system", "adding body");
}

void physics_system::step(const float delta) const
{
    LOG_INFO("physics_system", "integrating {} bodies", bodies_.size());

    // integrate all bodies
    for(const auto &body : bodies_)
    {
        body->integrate(delta);
    }

    std::vector<contact> contacts{ };

    // generate all collision contacts
    for(auto i = 0u; i < bodies_.size(); ++i)
    {
        for(auto j = i + 1u; j < bodies_.size(); ++j)
        {
            auto current_contacts = collision::detect(*bodies_[i], *bodies_[j]);
            contacts.insert(
                std::cend(contacts),
                std::begin(current_contacts),
                std::end(current_contacts));
        }
    }

    LOG_INFO("physics_system", "resolving {} constacts", contacts.size());

    // resolve all collision contacts
    collision::resolve(contacts);
}

}

