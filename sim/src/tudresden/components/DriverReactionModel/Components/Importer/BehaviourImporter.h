/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef BEHAVIOURIMPORTER_H
#define BEHAVIOURIMPORTER_H

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QFile>

#include "BehaviourConverter.h"
#include "LoggerInterface.h"
#include "common/xmlParser.h"

class BehaviourImporter {
  public:
      BehaviourImporter(const BehaviourImporter &) = delete;
      BehaviourImporter &operator=(const BehaviourImporter &) = delete;
      ~BehaviourImporter() {
          std::cout << "BehaviourImporter destroyed" << std::endl;
      }

      bool Import(const std::string &filename);
      std::shared_ptr<BehaviourData> GetBehaviourData(DReaMDefinitions::AgentVehicleType agentType);

      static std::shared_ptr<BehaviourImporter> GetInstance(const std::string &path, LoggerInterface *loggerInterface) {
          if (!instance)
              instance = std::shared_ptr<BehaviourImporter>(new BehaviourImporter(path, loggerInterface));
          return instance;
      }

      static void ResetBehaviourImporter() {
          instance.reset();
      }

  private:
      BehaviourImporter(const std::string &path, LoggerInterface *loggerInterface);

      bool ImportGroup(QDomElement &groupElement, DReaM::StatisticsGroup &group);
      bool ImportSet(QDomElement &setElement, DReaM::StatisticsSet &set);
      bool ImportEntry(QDomElement &entryElement, std::string id, DReaM::StatisticsSet &set);
      void Log(const std::string &message, DReaMLogLevel level = info) const {
          loggerInterface->Log(message, level);
      }

      static std::shared_ptr<BehaviourImporter> instance;
      static std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<BehaviourData>> behaviourDataMap;
      DReaM::StatisticsGroup mainGroup{"-1"};
      LoggerInterface *loggerInterface;
};

#endif // BEHAVIOURIMPORTER_H
