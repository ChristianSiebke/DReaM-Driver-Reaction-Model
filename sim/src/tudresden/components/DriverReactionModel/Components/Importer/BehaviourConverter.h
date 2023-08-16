/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef BEHAVIOURCONVERTER_H
#define BEHAVIOURCONVERTER_H

#include "Common/BehaviourData.h"
#include "LoggerInterface.h"

class BehaviourConverter {
  public:
    BehaviourConverter(LoggerInterface* loggerInterface) : loggerInterface{loggerInterface} {}
    ~BehaviourConverter() = default;

    std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<BehaviourData>> Convert(const DReaM::StatisticsGroup &main);

private:
    std::shared_ptr<BehaviourData> ConvertActionDecisionStatistics(std::shared_ptr<BehaviourData> data, const DReaM::StatisticsGroup &main);
    std::shared_ptr<BehaviourData> ConvertActionDecisionParameters(std::shared_ptr<BehaviourData> data, const DReaM::StatisticsGroup &main);
    std::shared_ptr<BehaviourData> ConvertCognitiveMapParameters(std::shared_ptr<BehaviourData> data, const DReaM::StatisticsGroup &main);
    std::shared_ptr<BehaviourData> ConvertGazeMovementParameters(std::shared_ptr<BehaviourData> data, const DReaM::StatisticsGroup &main);
    void Log(const std::string &message, DReaMLogLevel level = info) const {
        loggerInterface->Log(message, level);
    }

    std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<BehaviourData>> behaviourMap{};
    LoggerInterface* loggerInterface;
};

#endif // BEHAVIOURCONVERTER_H
