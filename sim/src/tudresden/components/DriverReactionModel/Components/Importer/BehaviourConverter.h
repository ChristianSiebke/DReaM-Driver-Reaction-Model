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

    std::unique_ptr<BehaviourData> Convert(const StatisticsGroup& main);

  private:
    void ConvertActionDecisionStatistics(const StatisticsGroup& main);
    void ConvertActionDecisionParameters(const StatisticsGroup& main);
    void ConvertCognitiveMapParameters(const StatisticsGroup& main);
    void ConvertGazeMovementParameters(const StatisticsGroup& main);
    void Log(const std::string &message, DReaMLogLevel level = info) const {
        loggerInterface->Log(message, level);
    }

    std::unique_ptr<BehaviourData> behaviourData;
    LoggerInterface* loggerInterface;
};

#endif // BEHAVIOURCONVERTER_H
