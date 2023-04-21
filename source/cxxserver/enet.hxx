///
/// @file enet.hxx
/// @brief ENet global state initializer/de-initializer
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <enet/enet.h>

namespace cxxserver {

class ENet
{
  public:

    ENet(const ENet &)             = delete;
    ENet(ENet &&)                  = delete;
    ENet & operator=(const ENet &) = delete;
    ENet & operator=(ENet &&)      = delete;

    static ENet & get()
    {
        static ENet sENet;
        return sENet;
    }

    void init()
    {
        if (++mReferences == 1)
        {
            enet_initialize();
        }
    }

    void stop()
    {
        if ((mReferences >= 1) && (--mReferences == 0))
        {
            enet_deinitialize();
        }
    }

  private:

    ENet() = default;

    ~ENet()
    {
        if (mReferences != 0)
        {
            enet_deinitialize();
            mReferences = 0;
        }
    }

    std::size_t mReferences { 0 };
};

} // namespace cxxserver
