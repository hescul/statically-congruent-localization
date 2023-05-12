// Copyright (c) 2023. Minh Nguyen, Lam Luu
// All rights reserved.

#include <algorithm>
#include <cmath>
#include <numbers>

#include "ParamSet.h"

constexpr auto ANGLE_ERROR = 0.00001f;

bool approx(float x, float y);
bool isSubset(const std::vector<float>& a, const std::vector<float>& b);

bool approx(float x, float y) {
    return std::abs(x - y) <= ANGLE_ERROR;
}

bool isSubset(const std::vector<float>& a, const std::vector<float>& b) {
    auto tmpA = a;
    auto tmpB = b;
    std::ranges::sort(tmpA);
    std::ranges::sort(tmpB);

    auto count = 0;
    for (const auto bNode : tmpB) {
        if (tmpA[count] == bNode) {
            ++count;
        }
    }
    return count == tmpA.size();
}

ParamSet generateParamSet(const Point2D& source, const std::vector<Point2D>& mesh) {
    static constexpr auto TWO_PI = std::numbers::pi_v<float> * 2.0f;

    // Compute the ParamSet help list
    auto psHelpList = std::vector<ParamSetNode>{};
    for (const auto& point : mesh) {
        // We do not need to calculate for the source point.
        if (point.x == source.x && point.y == source.y) {
            continue;
        }

        auto node = ParamSetNode{};
        node.point = point;
        node.distance = std::sqrt(std::pow(source.x - point.x, 2.0f) + std::pow(source.y - point.y, 2.0f));
        auto theta = std::atan2(point.y - source.y, point.x - source.x);
        if (theta < 0.0f) {
            theta += TWO_PI;
        }
        node.theta = theta;

        // Insertion sort idea
        auto haveInserted = false;
        for (unsigned j = 0; j < psHelpList.size(); ++j) {
            if (node.theta == psHelpList[j].theta) {
                if (node.distance < psHelpList[j].distance) {
                    psHelpList.insert(psHelpList.begin() + j, node);
                    haveInserted = true;
                    break;
                }
            }
            if (node.theta < psHelpList[j].theta) {
                psHelpList.insert(psHelpList.begin() + j, node);
                haveInserted = true;
                break;
            }
        }
        if (!haveInserted) {
            psHelpList.push_back(node);
        }
    }

    // Compute the ParamSet
    auto ps = ParamSet{};
    for (unsigned i = 0; i < psHelpList.size(); ++i) {
        ps.distances.push_back(psHelpList[i].distance);
        if (i == psHelpList.size() - 1) {
            ps.angles.push_back(TWO_PI - (psHelpList[i].theta - psHelpList[0].theta));
        } else {
            ps.angles.push_back(psHelpList[i + 1].theta - psHelpList[i].theta);
        }
    }
    return ps;
}

std::vector<ParamSet> offlinePhase(const std::vector<Point2D>& mesh) {
    auto result = std::vector<ParamSet>{};
    for (const auto& point : mesh) {
        result.push_back(generateParamSet(point, mesh));
    }
    return result;
}

std::vector<Point2D> onlinePhase(const std::vector<ParamSet>& myMap, const ParamSet& visiblePs, const std::vector<Point2D>& mesh) {
    auto resultPoints = std::vector<Point2D>{};
    if (visiblePs.distances.empty()) {
        return resultPoints;
    }
    for (unsigned i = 0; i < myMap.size(); ++i) {
        // Check if target visiblePs's distances is subset of node's distances
        auto found = isSubset(visiblePs.distances, myMap[i].distances);
        if (found) {
            if (visiblePs.angles.empty()) {
                if (!visiblePs.distances.empty())
                    resultPoints.push_back(mesh[i]);
                continue;
            }

            const auto alpha = visiblePs.angles[0];
            auto lPtr  = 0;
            auto rPtr = 0;
            auto windowSum = 0.0f;

            do {
                rPtr %= static_cast<int>(myMap[i].angles.size());
                windowSum += myMap[i].angles[rPtr];
                if (approx(windowSum, alpha)) {
                    auto isMatch = true;
                    auto accumulatedSum = 0.0f;

                    auto l = 1;
                    for (
                        auto k = (rPtr + 1) % myMap[i].angles.size();
                        k != lPtr;
                        k = (k + 1) % myMap[i].angles.size()
                    ) {
                        accumulatedSum += myMap[i].angles[k];
                        if (approx(accumulatedSum, visiblePs.angles[l])) {
                            accumulatedSum = 0;
                            l++;
                        }
                        else if (accumulatedSum > visiblePs.angles[l]) {
                            isMatch = false;
                            break;
                        }
                    }
                    if (accumulatedSum != 0)
                        isMatch = false;
                    if (isMatch) {
                        resultPoints.push_back(mesh[i]);
                        break;
                    }
                    windowSum -= myMap[i].angles[lPtr];
                    // window_sum -= myMap[i].angle[right_pointer];
                    ++rPtr;
                    ++lPtr;
                } else if (windowSum > alpha) {
                    windowSum -= myMap[i].angles[lPtr];
                    windowSum -= myMap[i].angles[rPtr];
                    ++lPtr;
                } else if (windowSum < alpha) {
                    ++rPtr;
                }
            } while (lPtr != myMap[i].angles.size());
        }
    }
    return resultPoints;
}
