#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class TimeMeasurement {
public:
    TimeMeasurement(std::string title) : name(title) {
    }

    ~TimeMeasurement() {
        PrintStatistics();
    }

    void PrintStatistics() {
        auto map = openPoints;
        for (const auto &element : map) {
            EndTimePoint(element.first);
        }

        std::cout << "TIME MEASUREMENT [" << name << "]" << std::endl;
        for (const auto &[name, times] : measuredTimes) {
            auto mean = CalculateMean(times);
            auto median = CalculateMedian(times);
            std::cout << " - " << name << ": "
                      << "MEAN: " << std::to_string(mean) << ", MED: " << median << std::endl;
        }
    }

    void StartTimePoint(std::string identifier) {
        if (identifier == "") {
            throw std::invalid_argument("Identifer must not be empty");
        }

        if (openPoints.find(identifier) != openPoints.end()) {
            throw std::invalid_argument("A measurement with this time point already exists.");
        }

        lastIdentifier = identifier;
        auto startTimePoint = std::chrono::high_resolution_clock::now();
        openPoints.insert(std::make_pair(identifier, startTimePoint));
    }

    void EndTimePoint(std::string identifier = "") {
        auto endTimePoint = std::chrono::high_resolution_clock::now();
        std::string actualIdentifier = identifier;
        if (identifier == "") {
            actualIdentifier = lastIdentifier;
            lastIdentifier = "";
        }
        if (openPoints.find(actualIdentifier) == openPoints.end()) {
            throw std::invalid_argument("This identifer does not exist.");
        }
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(endTimePoint - openPoints[actualIdentifier]);
        if (measuredTimes.find(actualIdentifier) == measuredTimes.end()) {
            std::vector<std::chrono::microseconds> tmp;
            measuredTimes.insert(std::make_pair(actualIdentifier, tmp));
        }
        measuredTimes[actualIdentifier].push_back(time);
        openPoints.erase(actualIdentifier);
    }

private:
    double CalculateMean(std::vector<std::chrono::microseconds> values) {
        double total = 0;
        for (const auto &value : values) {
            total += value.count();
        }
        return total / values.size();
    }

    double CalculateMedian(std::vector<std::chrono::microseconds> values) {
        std::sort(values.begin(), values.end());
        if (values.size() % 2 != 0)
            return (double)values[values.size() / 2].count();
        return (double)((double)values[(values.size() - 1) / 2].count() + (double)values[values.size() / 2].count()) / 2.0;
    }

private:
    std::string name;
    std::unordered_map<std::string, std::vector<std::chrono::microseconds>> measuredTimes{};

    std::unordered_map<std::string, std::chrono::_V2::system_clock::time_point> openPoints{};
    std::string lastIdentifier;
};
