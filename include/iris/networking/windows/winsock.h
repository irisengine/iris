#pragma once

namespace iris
{
/**
 * RAII class to initialise and cleanup winsock.
 */
class Winsock
{
  public:
    /**
     * Initialise winsock.
     */
    Winsock();

    /**
     * Cleanup winsock.
     */
    ~Winsock();
};
}
