#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop *ownerloop)
    : _ownerloop_(ownerloop)
{
}

bool Poller::hasPoller(Channel *channel) const
{
    auto it = _channelmap_.find(channel->fd());
    return it != _channelmap_.end() && it->second == channel;
}
