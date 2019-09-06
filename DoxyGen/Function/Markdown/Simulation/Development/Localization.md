\page localization Localization

\tableofcontents

This page describes the translation of coorindates (x,y) of an agent into RoadCoordinate (s,t), whereas the notion of (s,t) comes from the [OpenDRIVE](www.opendrive.org) standard.  
For more information, please refer to the [OpenDRIVE Format Specification](http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf).

\section dev_localization_basics Basics

The following image depics the basic principes of the localization which is rooted on the specifics of the OSI World Layer (aka OWL).

![Localization Basics](LocalizationBasics.svg)

Given is a point P in cartesian coordinates (x/y). 
The task is to assign the point to a lane, defined by a general road geometry and calculate the transformed Point P' in road coordinates (s/t).

Road geometry (based on OpenDRIVE):
 - A road consists of several sections
 - Each section consists of several lanes
 - Within a section, the number of lanes is constant
 - Lanes can have one predecessor and one successor
 - The road follows a reference line, which is the reference for the _s_-coordinate.
   The _t_-coordinate is perpendicular to this line.

OWL specifics:
 - All lanes are sampled, generating a stream of generic quadrilaterals (LaneGeometryElements).
 - Within a section, the number of quads per lane is equal, and all lanes have the same length in s.
 - This is realized by a variable sampling width, determined by a constant sampling width along the longest arc.
 - Consequently, points perpendicular to the reference line (_t_-axis) have the same _s_-coordinate.

Note, that the final _t_-coorindate is calculated with respect to a virtual reference line for each lane.
This means, that points on the center of a lane have _t_-coordinate of 0.

\section dev_localization_sequence Localization sequence

An [r-tree](https://www.boost.org/doc/libs/1_65_0/libs/geometry/doc/html/geometry/reference/spatial_indexes/boost__geometry__index__rtree.html) is used to store each LaneGeometryElement.
Thereby, due to the nature of the r-tree, the bounding box of the LaneGeometryElement is described by its maximum Cartesian coordinates (x_min, x_max, y_min, y_max).
Objects are located by retrieving all intersecting bounding boxes from the r-tree.
The picture below shows an example of an agent (blue) with the corresponding Cartesian bounding box, and all located LaneGeometryElements.
![Example of bounding boxes of LaneGeometryElements and agent](Localization1.png)

As the true boundary polygon may be smaller, the actual intersection polygon of the object and each LaneGeometryElement is calculated. 
For each point of a non-empty intersection polygon, the s and t coordinates are calculated and aggregated with respect to the underlying lane.
For each touched lane, the minimum and maximum s coordinate, and the minimum and maximum lane remainder (delta t) is stored.
![Example for the calculation of s_min, s_max and delta_left](Localization2.png)

In addition, if the reference point (i.e. the middle of the rear axle) or the mainLaneLocator (i.e. the middle of the agent front) are located within a LaneGeometryElement, s/t/yaw is calculated of each point, respectively.
Further aggregation is done with respect to each road by calculating the minimum and maximum s for each road the agent intersects with. 
For the current route of an agent, the following information is stored: s/t/yaw of the reference point and mainLaneLocator on the route (roads along a route are not allowed to intersect), distance from the lane boundary to the left and right for the road(s) along the route, and OpenDRIVE Ids of the lanes on the route that the agent touches. 
The results also holds information wether both the reference point and the mainLaneLocator lay on the route.
In the currently implementation, these points must be located - otherwise the agent is despawened, as the agent cannot execute distance queries without a relation to its current route.
