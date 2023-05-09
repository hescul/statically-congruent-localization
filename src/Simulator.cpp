// Copyright (c) 2023. Minh Nguyen, Lam Luu
// All rights reserved.

#include <chrono>
#include <cmath>
#include <limits>
#include <iostream>

#include "Simulator.h"

class Timer {
    // Type aliases to make accessing nested type easier
    using Clock = std::chrono::steady_clock;
    using Second = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<Clock> m_beg{ Clock::now() };

public:

    void reset() {
        m_beg = Clock::now();
    }

    [[nodiscard]] double elapsed() const {
        return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
    }
};

Stat navigate(const std::vector<Point2D>& mesh, const Point2D& lostPoint, const int visibleRadius) {
    std::cout << "Map Size: " << mesh.size() << '\n';
    std::cout << "Visible Radius: " << visibleRadius << '\n';

    // Offline Phase
    auto timer = Timer{};
    const auto myMap = offlinePhase(mesh);
    const auto offlineTime = timer.elapsed();
    std::cout << "[t] Offline phase took: " << offlineTime << " seconds\n";

    // Online Phase
    auto closestPoint = Point2D{};
    auto minDistance = std::numeric_limits<float>::max();

    for (const auto& x : mesh) {
        if (minDistance > std::sqrt(std::pow(lostPoint.x - x.x, 2.0f) + std::pow(lostPoint.y - x.y, 2.0f))) {
            minDistance = std::sqrt(std::pow(lostPoint.x - x.x, 2.0f) + std::pow(lostPoint.y - x.y, 2.0f));
            closestPoint = x;
        }
    }
    std::cout << "Closest point: " << "(" << closestPoint.x << ", " << closestPoint.y << "), with distance: " << minDistance << '\n';

    auto visiblePoints = std::vector<Point2D>{};
    for (auto x : mesh) {
        if (std::sqrt(std::pow(closestPoint.x - x.x, 2.0f) + std::pow(closestPoint.y - x.y, 2.0f)) < visibleRadius) {
            visiblePoints.push_back(x);
        }
    }

    const auto visiblePs = generateParamSet(closestPoint, visiblePoints);
    std::cout << "Number of visible points: " << visiblePoints.size() - 1 << '\n';

    timer.reset();
    const auto suggestedPoints = onlinePhase(myMap, visiblePs, mesh);
    const auto onlineTime = timer.elapsed();
    std::cout << "[t] Online phase took: " << onlineTime << " seconds\n";

    std::cout << "----------------------------------------------\n";
    std::cout << "Result:\n";
    for (const auto& result : suggestedPoints) {
        std::cout << result.x << " " << result.y << '\n';
    }
    return Stat{ visiblePoints, closestPoint, suggestedPoints, offlineTime, onlineTime };
}