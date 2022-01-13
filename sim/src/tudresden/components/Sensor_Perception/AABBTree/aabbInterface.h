#pragma once
#include "aabb.h"

struct AABBInterface {
  public:
    virtual ~AABBInterface() = default;
    virtual AABB GetAABB() const = 0;
};
