/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "BehaviourImporter.h"

std::shared_ptr<BehaviourImporter> BehaviourImporter::instance = nullptr;
std::map<DReaMDefinitions::AgentVehicleType, std::shared_ptr<BehaviourData>> BehaviourImporter::behaviourDataMap{};
BehaviourImporter::BehaviourImporter(const std::string &path, LoggerInterface *loggerInterface) : loggerInterface{loggerInterface} {
    if (!Import(path)) {
        Log("Could not import Behaviour.xml ", DReaMLogLevel::error);
    }
    BehaviourConverter converter(loggerInterface);
    behaviourDataMap = converter.Convert(mainGroup);
}

std::shared_ptr<BehaviourData> BehaviourImporter::GetBehaviourData(DReaMDefinitions::AgentVehicleType agentType) {
    if (behaviourDataMap.find(agentType) != behaviourDataMap.end()) {
        return behaviourDataMap.at(agentType);
    }
    else if (behaviourDataMap.find(DReaMDefinitions::AgentVehicleType::Car) != behaviourDataMap.end()) {
        return behaviourDataMap.at(DReaMDefinitions::AgentVehicleType::Car);
    }
    else {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " AgentType exist not in behaviour.xml";
        throw std::runtime_error(message);
    }
}

bool BehaviourImporter::Import(const std::string& filename) {
    std::locale::global(std::locale("C"));

    QFile xmlFile(filename.c_str()); // automatic object will be closed on destruction
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray xmlData(xmlFile.readAll());
    QDomDocument document;
    QString errorMsg;
    int errorLine;
    if (!document.setContent(xmlData, &errorMsg, &errorLine)) {
        return false;
    }

    QDomElement documentRoot = document.documentElement();
    if (documentRoot.isNull()) {
        return false;
    }

    QDomElement groupElement;
    if (SimulationCommon::GetFirstChildElement(documentRoot, "group", groupElement)) {
        while (!groupElement.isNull()) {
            // main stats group id
            std::string id;
            if (!SimulationCommon::ParseAttributeString(groupElement, "name", id)) {
                return false;
            }

            DReaM::StatisticsGroup group(id);
            if (!ImportGroup(groupElement, group)) {
                return false;
            }

            mainGroup.groups.insert(std::make_pair(id, group));

            groupElement = groupElement.nextSiblingElement("group");
        } // group loop
    }     // if group exist

    return true;
}

bool BehaviourImporter::ImportGroup(QDomElement &groupElement, DReaM::StatisticsGroup &group) {
    QDomElement subgroupElement;
    if (SimulationCommon::GetFirstChildElement(groupElement, "group", subgroupElement)) {
        while (!subgroupElement.isNull()) {
            // subgroup id
            std::string id;
            if (!SimulationCommon::ParseAttributeString(subgroupElement, "name", id)) {
                return false;
            }

            DReaM::StatisticsGroup subgroup(id);
            if (!ImportGroup(subgroupElement, subgroup)) {
                return false;
            }

            group.groups.insert(std::make_pair(id, subgroup));

            subgroupElement = subgroupElement.nextSiblingElement("group");
        } // subgroup loop
    }     // if subgroup exist

    QDomElement setElement;
    if (SimulationCommon::GetFirstChildElement(groupElement, "set", setElement)) {
        while (!setElement.isNull()) {
            // set id
            std::string id;
            if (!SimulationCommon::ParseAttributeString(setElement, "name", id)) {
                return false;
            }

            DReaM::StatisticsSet set(id);
            if (!ImportSet(setElement, set)) {
                return false;
            }

            group.sets.insert(std::make_pair(id, set));

            setElement = setElement.nextSiblingElement("set");
        } // set loop
    }     // if set exist

    return true;
}

bool BehaviourImporter::ImportSet(QDomElement &setElement, DReaM::StatisticsSet &set) {
    QDomElement entryElement;
    if (SimulationCommon::GetFirstChildElement(setElement, "entry", entryElement)) {
        while (!entryElement.isNull()) {
            // entry id
            std::string id;
            if (!SimulationCommon::ParseAttributeString(entryElement, "name", id)) {
                auto msg = "   \" name \" Attribute is missing";
                Log(msg);
                return false;
            }

            if (!ImportEntry(entryElement, id, set)) {
                return false;
            }

            entryElement = entryElement.nextSiblingElement("entry");
        } // entry loop
    }     // if entry exist

    return true;
}

bool BehaviourImporter::ImportEntry(QDomElement &entryElement, std::string id, DReaM::StatisticsSet &set) {
    // entry type
    std::string type;
    if (!SimulationCommon::ParseAttributeString(entryElement, "type", type)) {
        return false;
    }
    if (type == "NormalDistribution") {
        // distribution mean
        double mean;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "mean", mean)) {
            auto msg = set.identifier + " " + id + "   \" mean \" Attribute is missing";
            Log(msg);
            return false;
        }
        // distribution std deviation
        double std_dev;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "stdDeviation", std_dev)) {
            auto msg = set.identifier + " " + id + "   \" stdDeviation \" Attribute is missing";
            Log(msg);
            return false;
        }
        double min;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "min", min)) {
            auto msg = set.identifier + " " + id + "   \" min \" Attribute is missing";
            Log(msg);
            return false;
        }
        double max;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "max", max)) {
            auto msg = set.identifier + " " + id + "   \" max \" Attribute is missing";
            Log(msg);
            return false;
        }

        set.entries.insert(std::make_pair(id, std::make_shared<DReaM::NormalDistribution>(mean, std_dev, min, max)));
    }
    else if (type == "LogNormalDistribution") {
        // distribution mean
        double sigma;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "sigma", sigma)) {
            auto msg = set.identifier + " " + id + "   \" sigma \" Attribute is missing";
            Log(msg);
            return false;
        }
        // distribution std deviation
        double mu;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "mu", mu)) {
            auto msg = set.identifier + " " + id + "   \" mu \" Attribute is missing";
            Log(msg);
            return false;
        }
        double min;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "min", min)) {
            auto msg = set.identifier + " " + id + "   \" min \" Attribute is missing";
            Log(msg);
            return false;
        }
        double max;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "max", max)) {
            auto msg = set.identifier + " " + id + "   \" max \" Attribute is missing";
            Log(msg);
            return false;
        }

        set.entries.insert(std::make_pair(id, std::make_shared<DReaM::LogNormalDistribution>(sigma, mu, min, max)));
    }
    else if (type == "parameter") {
        double value;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "value", value)) {
            auto msg = set.identifier + " " + id + "   \" value \" Attribute is missing";
            Log(msg);
            return false;
        }

        set.entries.insert(std::make_pair(id, std::make_shared<DReaM::StandardDoubleEntry>(value)));
    }
    else {
        // TODO account for other types
        return false;
    }

    return true;
}
