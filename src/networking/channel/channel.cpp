////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "networking/channel/channel.h"

namespace iris
{

std::vector<Packet> Channel::yield_send_queue()
{
    std::vector<Packet> queue;
    std::swap(queue, send_queue_);
    return queue;
}

std::vector<Packet> Channel::yield_receive_queue()
{
    std::vector<Packet> queue;
    std::swap(queue, receive_queue_);
    return queue;
}

}
