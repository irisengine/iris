////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "networking/channel/unreliable_unordered_channel.h"

namespace iris
{

void UnreliableUnorderedChannel::enqueue_send(Packet packet)
{
    send_queue_.emplace_back(std::move(packet));
}

void UnreliableUnorderedChannel::enqueue_receive(Packet packet)
{
    receive_queue_.emplace_back(std::move(packet));
}

}
