// Copyright (c) 2023. Minh Nguyen, Lam Luu
// All rights reserved.

#pragma once

#include <vector>

struct ParamSet {
    std::vector<float> distances{};
    std::vector<float> angles{};
};

struct Point2D {
    float x{ 0.0f };
    float y{ 0.0f };
};

struct ParamSetNode {
    Point2D point;
    float distance{};
    float theta{};    // angle created by Oy and the line from source to point
};

ParamSet generateParamSet(const Point2D& source, const std::vector<Point2D>& mesh);
std::vector<ParamSet> offlinePhase(const std::vector<Point2D>& mesh);
std::vector<Point2D> onlinePhase(const std::vector<ParamSet>& myMap, const ParamSet& visiblePs, const std::vector<Point2D>& mesh);