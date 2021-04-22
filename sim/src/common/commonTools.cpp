/*******************************************************************************
* Copyright (c) 2018, 2019, 2021 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "commonTools.h"
#include <algorithm>

bool CommonHelper::IntersectionCalculation::OnEdge(const Common::Vector2d &A, const Common::Vector2d &B, const Common::Vector2d &P)
{
    const auto PA = P - A;
    const auto BA = B - A;

    if (std::abs(PA.x * BA.y - PA.y * BA.x) > CommonHelper::EPSILON)
    {
        return false;
    }

    if (std::abs(BA.y) < CommonHelper::EPSILON)
    {
        return BA.x > 0 ? A.x <= P.x && P.x <= B.x :
                          B.x <= P.x && P.x <= A.x;
    }
    else
    {
        return BA.y > 0 ? A.y <= P.y && P.y <= B.y :
                          B.y <= P.y && P.y <= A.y;
    }
}

bool CommonHelper::IntersectionCalculation::WithinBarycentricCoords(double dot00, double dot02, double dot01, double dot11, double dot12)
{
    // Compute barycentric coordinates
    const auto denom = (dot00 * dot11 - dot01 * dot01);
    const auto u = (dot11 * dot02 - dot01 * dot12) / denom;
    const auto v = (dot00 * dot12 - dot01 * dot02) / denom;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}

bool CommonHelper::IntersectionCalculation::IsWithin(const Common::Vector2d &A, const Common::Vector2d &B, const Common::Vector2d &C, const Common::Vector2d &D, const Common::Vector2d &P)
{
    // Triangle 1 (A, B, C)
    const auto BA = A - B;
    const auto BC = C - B;
    const auto BP = P - B;

    auto dot00 = BA.Dot(BA);
    auto dot01 = BA.Dot(BC);
    auto dot02 = BA.Dot(BP);
    auto dot11 = BC.Dot(BC);
    auto dot12 = BC.Dot(BP);

    if (WithinBarycentricCoords(dot00, dot02, dot01, dot11, dot12))
    {
        return true;
    }

    // Triangle 2 (C, B, D)
    const auto BD = D - B;

    dot00 = dot11;
    dot02 = dot12;
    dot01 = BC.Dot(BD);
    dot11 = BD.Dot(BD);
    dot12 = BD.Dot(BP);

    if (WithinBarycentricCoords(dot00, dot02, dot01, dot11, dot12))
    {
        return true;
    }

    // On an edge
    return OnEdge(A, B, P) || // along left edge
            OnEdge(B, D, P) || // along end of next element
            OnEdge(D, C, P) || // along right edge
            OnEdge(C, A, P) || // along start of current element
            OnEdge(B, C, P);   // along triangular split
}

std::vector<Common::Vector2d> CommonHelper::IntersectionCalculation::GetIntersectionPoints(const std::vector<Common::Vector2d> &firstPoints, const std::vector<Common::Vector2d> &secondPoints)
{
    std::vector<Common::Vector2d> intersectionPoints{};
    intersectionPoints.reserve(8);
    Common::Vector2d firstEdges[4];
    firstEdges[0] = firstPoints[1] - firstPoints[0];
    firstEdges[1] = firstPoints[2] - firstPoints[1];
    firstEdges[2] = firstPoints[3] - firstPoints[2];
    firstEdges[3] = firstPoints[0] - firstPoints[3];
    Common::Vector2d secondEdges[4];
    secondEdges[0] = secondPoints[1] - secondPoints[0];
    secondEdges[1] = secondPoints[2] - secondPoints[1];
    secondEdges[2] = secondPoints[3] - secondPoints[2];
    secondEdges[3] = secondPoints[0] - secondPoints[3];


    double lambda[4][4];
    double kappa[4][4];
    bool parallel[4][4];
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t k = 0; k < 4; k++)
        {
            double determinant = secondEdges[i].x * firstEdges[k].y - secondEdges[i].y * firstEdges[k].x;
            parallel[i][k] = (std::abs(determinant) < CommonHelper::EPSILON);
            lambda[i][k] = ( - secondPoints[i].x * firstEdges[k].y + secondPoints[i].y * firstEdges[k].x
                             + firstPoints[k].x * firstEdges[k].y - firstPoints[k].y * firstEdges[k].x ) / determinant;
            kappa[i][k] = ( - secondPoints[i].x * secondEdges[i].y + secondPoints[i].y * secondEdges[i].x
                            + firstPoints[k].x * secondEdges[i].y - firstPoints[k].y * secondEdges[i].x) / determinant;
            if (lambda[i][k] > 0 && lambda[i][k] < 1 && kappa[i][k] > 0 && kappa[i][k] < 1)
            {
                //0 < lambda < 1 and 0 < kappa < 1 => edges intersect. Intersection point is left hand side (and rhs) of the above equation
                double intersectionPointX = secondPoints[i].x + lambda[i][k] * secondEdges[i].x;
                double intersectionPointY = secondPoints[i].y + lambda[i][k] * secondEdges[i].y;
                intersectionPoints.emplace_back(intersectionPointX, intersectionPointY);
            }
        }
    }
    // For each vertex of the first quadrangle check, if it is inside the second. If true, it is also part of the intersection polygon.
    for (size_t k = 0; k < 4; k++)
    {
        if ((!parallel[0][k] ? kappa[0][k] * kappa[2][k] < 0 : (1 - kappa[0][(k -1) % 4]) * (1 - kappa[2][(k -1) % 4]) < 0) &&
                (!parallel[1][k] ? kappa[1][k] * kappa[3][k] < 0 : (1 - kappa[1][(k -1) % 4]) * (1 - kappa[3][(k -1) % 4]) < 0))
        {
            intersectionPoints.emplace_back(firstPoints[k]);
        }
    }
    // For each object vertex of the second quadrangle check, if it is inside the first. If true, it is also part of the intersection polygon.
    for (size_t i = 0; i < 4; i++)
    {
        if (IsWithin(firstPoints[1], firstPoints[2], firstPoints[0], firstPoints[3], secondPoints[i]))
        {
            intersectionPoints.emplace_back(secondPoints[i]);
        }
    }
    return intersectionPoints;
}

std::vector<Common::Vector2d> CommonHelper::IntersectionCalculation::GetIntersectionPoints(const polygon_t &firstPolygon, const polygon_t &secondPolygon)
{
    std::vector<Common::Vector2d> firstPoints;
    std::transform(firstPolygon.outer().begin(), firstPolygon.outer().end(), std::back_insert_iterator(firstPoints),
                   [&](const auto& point){return Common::Vector2d{bg::get<0>(point), bg::get<1>(point)};});
    firstPoints.pop_back();

    std::vector<Common::Vector2d> secondPoints;
    std::transform(secondPolygon.outer().begin(), secondPolygon.outer().end(), std::back_insert_iterator(secondPoints),
                   [&](const auto& point){return Common::Vector2d{bg::get<0>(point), bg::get<1>(point)};});
    secondPoints.pop_back();

    return GetIntersectionPoints(firstPoints, secondPoints);
}
