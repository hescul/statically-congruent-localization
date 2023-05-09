// Copyright (c) 2023. Minh Nguyen, Lam Luu
// All rights reserved.

#pragma once

#include <vector>

#include "ParamSet.h"

struct Stat {
    std::vector<Point2D> visiblePoints;
    Point2D closestPoint;
    std::vector<Point2D> suggestedPoints;
    double offlineTime;
    double onlineTime;
};

Stat navigate(const std::vector<Point2D>& mesh, const Point2D& lostPoint, int visibleRadius);
