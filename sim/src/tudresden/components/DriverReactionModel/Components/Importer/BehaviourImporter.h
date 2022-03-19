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
      BehaviourImporter(std::string path, LoggerInterface *loggerInterface);
      ~BehaviourImporter() = default;

      bool Import(const std::string &filename);
      std::unique_ptr<BehaviourData> GetBehaviourData();

  private:
    bool ImportGroup(QDomElement& groupElement, StatisticsGroup& group);
    bool ImportSet(QDomElement& setElement, StatisticsSet& set);
    bool ImportEntry(QDomElement& entryElement, std::string id, StatisticsSet& set);
    void Log(const std::string& message, LogLevel_new level = info) const { loggerInterface->Log(message, level); }


    std::unique_ptr<BehaviourData> behaviourData;
    StatisticsGroup mainGroup{"-1"};
    LoggerInterface* loggerInterface;
};

#endif // BEHAVIOURIMPORTER_H
