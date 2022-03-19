#include "BehaviourImporter.h"

BehaviourImporter::BehaviourImporter(std::string path, LoggerInterface *loggerInterface) : loggerInterface{loggerInterface} {
    if (!Import(path)) {
        Log("Could not import Behaviour.xml ", LogLevel_new::error);
    }
    BehaviourConverter converter(loggerInterface);
    behaviourData = converter.Convert(mainGroup);
}

std::unique_ptr<BehaviourData> BehaviourImporter::GetBehaviourData() { return std::move(behaviourData); }

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

            StatisticsGroup group(id);
            if (!ImportGroup(groupElement, group)) {
                return false;
            }

            mainGroup.groups.insert(std::make_pair(id, group));

            groupElement = groupElement.nextSiblingElement("group");
        } // group loop
    }     // if group exist

    return true;
}

bool BehaviourImporter::ImportGroup(QDomElement& groupElement, StatisticsGroup& group) {
    QDomElement subgroupElement;
    if (SimulationCommon::GetFirstChildElement(groupElement, "group", subgroupElement)) {
        while (!subgroupElement.isNull()) {
            // subgroup id
            std::string id;
            if (!SimulationCommon::ParseAttributeString(subgroupElement, "name", id)) {
                return false;
            }

            StatisticsGroup subgroup(id);
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

            StatisticsSet set(id);
            if (!ImportSet(setElement, set)) {
                return false;
            }

            group.sets.insert(std::make_pair(id, set));

            setElement = setElement.nextSiblingElement("set");
        } // set loop
    }     // if set exist

    return true;
}

bool BehaviourImporter::ImportSet(QDomElement& setElement, StatisticsSet& set) {
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

bool BehaviourImporter::ImportEntry(QDomElement& entryElement, std::string id, StatisticsSet& set) {

    // entry type
    std::string type;
    if (!SimulationCommon::ParseAttributeString(entryElement, "type", type)) {
        return false;
    }

    if (type == "distribution") {
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

        set.entries.insert(std::make_pair(id, std::make_shared<DistributionEntry>(mean, std_dev, min, max)));
    } else if (type == "parameter") {

        double value;
        if (!SimulationCommon::ParseAttributeDouble(entryElement, "value", value)) {
            auto msg = set.identifier + " " + id + "   \" value \" Attribute is missing";
            Log(msg);
            return false;
        }

        set.entries.insert(std::make_pair(id, std::make_shared<StandardDoubleEntry>(value)));
    } else {
        // TODO account for other types
        return false;
    }

    return true;
}
