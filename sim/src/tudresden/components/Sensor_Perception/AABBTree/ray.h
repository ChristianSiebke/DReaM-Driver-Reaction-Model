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

#include "Common/vector2d.h"

struct Ray {

  public:
    Ray() : origin(Common::Vector2d(0, 0)), direction(Common::Vector2d(0, 0)) {}
    Ray(Common::Vector2d start, Common::Vector2d dir) : origin(start), direction(dir) { direction.Norm(); }

  public:
    Common::Vector2d origin;
    Common::Vector2d direction;
};
