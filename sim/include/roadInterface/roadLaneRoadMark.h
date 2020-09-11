#pragma once


enum class RoadLaneRoadMarkType;
enum class RoadLaneRoadMarkColor;
enum class RoadLaneRoadMarkLaneChange;
enum class RoadLaneRoadDescriptionType;
enum class RoadLaneRoadMarkWeight;

class RoadLaneRoadMark
{
public:
    RoadLaneRoadMark(double sOffset,
                     RoadLaneRoadDescriptionType descriptionType,
                     RoadLaneRoadMarkType type,
                     RoadLaneRoadMarkColor color,
                     RoadLaneRoadMarkLaneChange laneChange,
                     RoadLaneRoadMarkWeight weight) :
        sOffset(sOffset),
        type(type),
        color(color),
        laneChange(laneChange),
        descriptionType(descriptionType),
        weight(weight)
    {
    }

    RoadLaneRoadMarkType GetType() const
    {
        return type;
    }

    double GetSOffset() const
    {
        return sOffset;
    }

    double GetSEnd() const
    {
        return  sEnd;
    }

    RoadLaneRoadMarkColor GetColor() const
    {
        return color;
    }

    RoadLaneRoadMarkWeight GetWeight() const
    {
        return  weight;
    }

    RoadLaneRoadMarkLaneChange GetLaneChange() const
    {
        return laneChange;
    }

    RoadLaneRoadDescriptionType GetDescriptionType() const
    {
        return descriptionType;
    }

    void LimitSEnd (double limit)
    {
        sEnd = std::min(sEnd, limit);
    }


private:

double sOffset;
double sEnd = std::numeric_limits<double>::max();
RoadLaneRoadMarkType type;
RoadLaneRoadMarkColor color;
RoadLaneRoadMarkLaneChange laneChange;
RoadLaneRoadDescriptionType descriptionType;
RoadLaneRoadMarkWeight weight;

};
