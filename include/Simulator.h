// Copyright (c) 2023. Minh Nguyen, Lam Luu
// All rights reserved.

#pragma once

#include <vector>

#include "ParamSet.h"

struct OnlineStat {
    std::vector<Point2D> visiblePoints;
    Point2D closestPoint;
    std::vector<Point2D> suggestedPoints;
    double onlineTime;
};

struct OfflineStat {
    std::vector<ParamSet> myMap;
    double offlineTime;
};

OnlineStat navigate(const std::vector<ParamSet>& myMap, const std::vector<Point2D>& mesh, const Point2D& lostPoint, int visibleRadius);

OfflineStat generateMyMap(const std::vector<Point2D>& mesh);
