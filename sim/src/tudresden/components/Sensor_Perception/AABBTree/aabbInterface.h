/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once
#include "aabb.h"

struct AABBInterface {
  public:
    virtual ~AABBInterface() = default;
    virtual AABB GetAABB() const = 0;
};
