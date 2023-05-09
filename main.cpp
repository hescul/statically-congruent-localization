// Copyright (c) 2023. Minh Nguyen, Lam Luu
// All rights reserved.

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Simulator.h"

std::vector<cv::Point2f> obtainCorners(cv::Mat dst);
void mouseCallback(int event, int x, int y, int flags, void* userData);
void makeText(const cv::Mat& target, const std::string& text, int line);

std::vector<Point2D> myMap{};
Point2D lostPoint{};

constexpr auto WINDOW = "Localization";

cv::Mat img;
auto visibleRadius = 100;

int main(const int argc, char** argv) {
    if (argc > 1) {
        try {
            visibleRadius = std::stoi(argv[1]);
            if (visibleRadius <= 0) {
                visibleRadius = 100;
            }
        }
        catch (const std::exception&) {
            visibleRadius = 100;
        }
    }

    img = cv::imread("input/hcmut_campus.jpg");

    cv::Mat gray;
    cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    gray.convertTo(gray, CV_32FC1);

    cv::Mat dst = cv::Mat::zeros(gray.size(), CV_32FC1);
    cornerHarris(gray, dst, 2, 3, 0.04);
    //result is dilated for marking the corners, not important
    dilate(dst, dst, cv::Mat(), cv::Point(-1, -1));

    // Initialize my map
    const auto corners = obtainCorners(dst);
    for (const auto& corner : corners) {
        myMap.emplace_back(corner.x, corner.y);
    }


    // Threshold for an optimal value, it may vary depending on the image.
    for (int i = 0; i < dst.rows; i++) {
        for (int j = 0; j < dst.cols; j++) {
            if (dst.at<float>(i, j) > 0.01f * dst.at<float>(0, 0)) {
                img.at<cv::Vec3b>(i, j)[0] = 0;
                img.at<cv::Vec3b>(i, j)[1] = 0;
                img.at<cv::Vec3b>(i, j)[2] = 255;
            }
        }
    }

    std::cout << "==================== NO GPS LOCALIZATION ====================== \n";
    cv::namedWindow(WINDOW);
    cv::setMouseCallback(WINDOW, mouseCallback, nullptr);

    imshow(WINDOW, img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}

std::vector<cv::Point2f> obtainCorners(cv::Mat dst) {
    double minVal, maxVal;
    minMaxLoc(dst, &minVal, &maxVal);

    const auto thresh = 0.01 * maxVal;
    threshold(dst, dst, thresh, 255, 0);
    dst.convertTo(dst, CV_8UC1);

    // find centroids
    cv::Mat labels, stats, centroids;
    const auto numLabels = connectedComponentsWithStats(dst, labels, stats, centroids);

    // define the criteria to stop and refine the corners
    const auto criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.001);
    const auto winSize = cv::Size(5, 5);
    const auto zeroZone = cv::Size(-1, -1);
    std::vector<cv::Point2f> corners{};

    for (int i = 1; i < numLabels; i++) {
        if (stats.at<int>(i, cv::CC_STAT_AREA) > 10) {  // filter out small blobs
            const auto centroidX = static_cast<float>(centroids.at<double>(i, 0));
            const auto centroidY = static_cast<float>(centroids.at<double>(i, 1));
            cv::Point2f centroid(centroidX, centroidY);
            corners.push_back(centroid);
        }
    }

    cornerSubPix(dst, corners, winSize, zeroZone, criteria);

    return corners;
}

void mouseCallback(const int event, const int x, const int y, const int flags, void* userData) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        // Clear the old traces
        imshow(WINDOW, img);

        // Draw on a clone to maintain the clean image
        auto clone = img.clone();

        std::cout << "Lost at: " << x << ' ' << y << '\n';

        lostPoint = Point2D(static_cast<float>(x), static_cast<float>(y));
        const auto stat = navigate(myMap, lostPoint, visibleRadius);

        // The lost point
        circle(clone, cv::Point(x, y), 6, cv::Scalar(0, 0, 0), -1);

        // The closest point
        const auto closestPoint = stat.closestPoint;
        circle(
                clone, cv::Point(static_cast<int>(closestPoint.x), static_cast<int>(closestPoint.y)),
                visibleRadius, cv::Scalar(255, 255, 20), 2
        );

        // The visible points
        for (const auto& visible : stat.visiblePoints) {
            circle(clone, cv::Point(static_cast<int>(visible.x), static_cast<int>(visible.y)),
                   6, cv::Scalar(169, 169, 169), -1
            );
        }

        // The found point
        for (const auto& result : stat.suggestedPoints) {
            circle(clone, cv::Point(static_cast<int>(result.x), static_cast<int>(result.y)),
                   6, cv::Scalar(255, 255, 0), -1
            );
        }

        const auto mapSizeText = "Map Size: " + std::to_string(myMap.size());
        makeText(clone, mapSizeText, 0);

        const auto visibleRadiusText = "Visible Radius : " + std::to_string(visibleRadius);
        makeText(clone, visibleRadiusText, 1);

        const auto visiblePointText = "Visible Points: " + std::to_string(stat.visiblePoints.size());
        makeText(clone, visiblePointText, 2);

        const auto offlineTimeText = "[+] Offline phase took: " + std::to_string(stat.offlineTime) + " seconds";
        makeText(clone, offlineTimeText, 3);

        const auto onlineTimeText = "[+] Online phase took: " + std::to_string(stat.onlineTime) + " seconds";
        makeText(clone, onlineTimeText, 4);

        const auto separateText = std::string{ "-----------------" };
        makeText(clone, separateText, 5);

        const auto resultText = std::string{ "Suggested Points:" };
        makeText(clone, resultText, 6);

        auto line = 7;
        for (const auto& result : stat.suggestedPoints) {
            const auto pointText = "(" + std::to_string(result.x) + ", " + std::to_string(result.y) + ")";
            makeText(clone, pointText, line++);
        }

        imshow(WINDOW, clone);

        std::cout << "\n==================== NO GPS LOCALIZATION ====================== \n";
    }
}

void makeText(const cv::Mat& target, const std::string& text, const int line) {
    const cv::Point org(0, (line + 1) * 20);
    constexpr int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    constexpr double fontScale = 0.5;
    const cv::Scalar color(0, 0, 0);
    constexpr auto thickness = 2;

    putText(target, text, org, fontFace, fontScale, color, thickness);
}
