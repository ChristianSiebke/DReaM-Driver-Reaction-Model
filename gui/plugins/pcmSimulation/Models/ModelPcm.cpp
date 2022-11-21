/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ModelPcm.h"

#include "ConfigurationGeneratorPcm/ConfigSetReaderPcm.h"
#include "GUI_Definitions.h"

ModelPcm::ModelPcm(QObject *parent) :
    QObject(parent)
{
    listModelPcm = new QStringListModel(this);
    listModelPcm->setStringList(pcmCaseList);

    baseFolder = QCoreApplication::applicationDirPath();
}

ModelPcm::~ModelPcm()
{
    ClearCaseList();
    if (listModelPcm != nullptr)
    {
        delete listModelPcm;
    }
}

QAbstractItemModel *ModelPcm::GetItemModelPcm() const
{
    return listModelPcm;
}

void ModelPcm::SetSelectionModelPcm(QItemSelectionModel *selectionModel)
{
    selectionModelPcm = selectionModel;
}

void ModelPcm::StartSimulationTrigger()
{
    simulationStop = false;
    StartSimulation();
}

void ModelPcm::SimulationStop()
{
    simulationStop = true;
}

bool ModelPcm::ClearCaseList()
{
    pcmCaseList.clear();
    listModelPcm->setStringList(pcmCaseList);
    return true;
}

bool ModelPcm::LoadCasesFromPcmFile(const QString &pcmFilePath)
{
    inputFromPCMDB = true;
    ClearCaseList();

    DatabaseReader dbReader;

    if(pcmFilePath.isEmpty()){
        return false;
    }

    currentPcmFilePath = pcmFilePath;
    dbReader.SetDatabase(currentPcmFilePath);
    bool success = dbReader.OpenDataBase();
    if (!success)
    {
        currentPcmFilePath = "./" + currentPcmFilePath;
        dbReader.SetDatabase(currentPcmFilePath);
        success = dbReader.OpenDataBase();
    }
    if (success)
    {
        success = dbReader.ReadCaseList(pcmCaseList);
    }

    if (success)
    {
        listModelPcm->setStringList(pcmCaseList);
    }

    if (!success)
    {
        Q_EMIT ShowMessage("ERROR",
                           "Database cannot be loaded! Please try to install the MADE Redistributable from https://www.microsoft.com/download/details.aspx?id=13255");
    }

    dbReader.CloseDataBase();

    return success;
}

bool ModelPcm::LoadCasesFromPrevResult(const QString &prevResultFolder)
{
    inputFromPCMDB = false;
    ClearCaseList();

    if (prevResultFolder.length() > 0 && QDir(prevResultFolder).exists())
    {
        this->prevResultFolder = prevResultFolder;

        // show case ids in listModelPcm
        QDirIterator it(prevResultFolder, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        while (it.hasNext())
        {
            QString caseNumber = QFileInfo(it.next()).baseName();  // get the base name of the subfolder without path prefix
            if (QRegExp(REGEX_CASE_NUMBER).exactMatch(caseNumber)) // check if the subfolder name is numeric
            {
                pcmCaseList.append(caseNumber);
            }
        }
        listModelPcm->setStringList(pcmCaseList);
        return true;
    }
    return false;
}

void ModelPcm::SetResultFolder(const QString &resultFolder)
{
    this->resultFolder = resultFolder;
}

void ModelPcm::SetLogLevel(const int level)
{
    this->logLevel = level;
}

void ModelPcm::SetOtherSystemFile(const QString &otherSystemFile)
{
    this->otherSystemFile = otherSystemFile;
}

void ModelPcm::SetCar1SystemFile(const QString &car1SystemFile)
{
    this->car1SystemFile = car1SystemFile;
}

void ModelPcm::SetCar2SystemFile(const QString &car2SystemFile)
{
    this->car2SystemFile = car2SystemFile;
}

void ModelPcm::SetInitRandomSeed(const int seed)
{
    this->initRandomSeed = seed;
}

void ModelPcm::SetVariationCount(const int varCount)
{
    this->variationCount = varCount;
}

void ModelPcm::EnableShiftRadius(const bool enable)
{
    this->shiftRadiusEnabled = enable;
}

void ModelPcm::SetShiftRadius1(const double radius)
{
    this->shiftRadius1 = radius;
}

void ModelPcm::SetShiftRadius2(const double radius)
{
    this->shiftRadius2 = radius;
}

void ModelPcm::EnableVelocityScale(const bool enable)
{
    this->velocityScaleEnabled = enable;
}

void ModelPcm::SetVelocityScale1(const double maxScale)
{
    this->velocityMaxScale1 = maxScale;
}

void ModelPcm::SetVelocityScale2(const double maxScale)
{
    this->velocityMaxScale2 = maxScale;
}

void ModelPcm::StartSimulation()
{
    Q_EMIT SimulationStarted();

    if (resultFolder.length() <= 0)
    {
        Q_EMIT ShowMessage("ERROR", "Invalid result directory [" + resultFolder + "]");
        Q_EMIT SimulationFinished();
        return;
    }

    //when reading input from previous results, forbid the result folder and previous result folder to be the same
    if((!inputFromPCMDB) && (resultFolder.compare(prevResultFolder) == 0))
    {
        Q_EMIT ShowMessage("ERROR", "The result directory [" + resultFolder + "] should be different from the previous result folder");
        Q_EMIT SimulationFinished();
        return;
    }

    QModelIndexList pcmCaseIndexList = selectionModelPcm->selectedIndexes();
    QStringList otherSytemList = otherSystemFile.split(QRegExp(",|;"));
    QStringList car1SystemList = car1SystemFile.split(QRegExp(",|;"));
    QStringList car2SystemList = car2SystemFile.split(QRegExp(",|;"));

    Q_EMIT SimulationProgressMaximum(pcmCaseIndexList.count() * otherSytemList.count() * car1SystemList.count() * car2SystemList.count() * 2);

    Q_EMIT SimulationProgressChanged(progress++);

    ConfigGenerator configGenerator(resultFolder);

    CreateConfigs(configGenerator,
                  pcmCaseIndexList,
                  otherSytemList,
                  car1SystemList,
                  car2SystemList);

    const QString frameworkConfigFile = configGenerator.GenerateFrameworkConfig(logLevel);
    if (frameworkConfigFile == "")
    {
        Q_EMIT ShowMessage("ERROR", "Failed to generate framework configuration file");
        Q_EMIT SimulationFinished();
        return;
    }

    // execute the main process
    QString mainPath = baseFolder + "/" + FILENAME_OPSIMULATIONMANAGER_EXE;
    QProcess *mainProcess = new QProcess();
    QStringList arguments;
    arguments << "--config" << QDir(baseFolder).filePath(frameworkConfigFile);

    mainProcess->start(mainPath, arguments);
    mainProcess->waitForFinished(-1);
    if (mainProcess->exitCode() != 0)
    {
        Q_EMIT ShowMessage("ERROR", "Simulation aborted. opSimulationManager returned with -1");
        Q_EMIT SimulationFinished();
        delete mainProcess;
        return;
    }
    delete mainProcess;

    Q_EMIT SimulationProgressChanged(progress++);

    Q_EMIT ShowMessage("Information", "Simulation successfully finished.");
    Q_EMIT SimulationFinished();

    progress = 0;
}

void ModelPcm::CreateConfigs(ConfigGenerator &configGenerator, QModelIndexList &pcmCaseIndexList, QStringList &otherSytemList, QStringList &car1SystemList, QStringList &car2SystemList)
{
    for (QModelIndex &pcmCaseIndex : pcmCaseIndexList)
    {
        if (simulationStop)
        {
            Q_EMIT ShowMessage("Information", "Simulation aborted.");
            Q_EMIT SimulationFinished();
            return;
        }

        // Prepare to read a simulation set
        QString pcmCase = QString("%1").arg(pcmCaseIndex.data().toInt());
        PCM_SimulationSet *simulationSet = ReadSimulationSet(inputFromPCMDB, pcmCase);
        if (simulationSet == nullptr)
        {
            Q_EMIT ShowMessage("ERROR", "Failed to read data for case: " + pcmCase);
            Q_EMIT SimulationFinished();
            return;
        }

        int otherSystemCount = 0;
        for (QString otherSystem : otherSytemList)
        {
            if (otherSystem.isEmpty())
            {
                continue;
            }
            int car1SystemCount = 0;
            for (QString car1System : car1SystemList)
            {
                if (car1System.isEmpty())
                {
                    continue;
                }
                int car2SystemCount = 0;
                for (QString car2System : car2SystemList)
                {
                    if (car2System.isEmpty())
                    {
                        continue;
                    }

                    QString systemName = QString::number(otherSystemCount) + "-" + QString::number(car1SystemCount) + "-" + QString::number(car2SystemCount);
                    QString caseSystemName = pcmCase + "/" + systemName;

                    for (int varIndex = 0; varIndex <= variationCount; varIndex++)
                    {
                        QString varName = "";
                        bool withVariation = false;

                        if (varIndex == 0)
                        {
                            varName = DIR_NO_VARIATION;
                            withVariation = false;
                        }
                        else
                        {
                            varName = QString("Var_%1").arg(varIndex, 5, 10, QChar('0')); // zero padding if var index less than 5 digits
                            withVariation = true;
                        }

                        QString caseSystemVarName = caseSystemName + "/" + varName;

                        // the random seed uses PCM case number if the inital seed is negative. Otherwise it uses the inital seed.
                        int randomSeed = (initRandomSeed < 0) ? (pcmCaseIndex.data().toInt() + varIndex)
                                                              : (initRandomSeed + varIndex);

                        if (withVariation)
                        {
                            SaveState(simulationSet);
                            ApplyVariation(simulationSet, randomSeed);
                        }

                        if (!configGenerator.GenerateConfigs(pcmCase,
                                                             caseSystemVarName,
                                                             otherSystem, car1System, car2System,
                                                             randomSeed,
                                                             simulationSet))
                        {
                            Q_EMIT ShowMessage("ERROR", "Failed to generate configuration file for case: " + pcmCase);
                            Q_EMIT SimulationFinished();

                            if (simulationSet != nullptr)
                            {
                                delete simulationSet;
                            }

                            return;
                        }

                        if (withVariation)
                        {
                            ResetState(simulationSet);
                        }
                    }

                    car2SystemCount++;

                    Q_EMIT SimulationProgressChanged(progress++);
                }
                car1SystemCount++;
            }
            otherSystemCount++;
        }

        if (simulationSet != nullptr)
        {
            delete simulationSet;
        }
    }
}

bool ModelPcm::PrepareFolderstructure(QString folder)
{
    QDir caseSystemVarDir(folder);
    if (caseSystemVarDir.exists())
    {
        if (!caseSystemVarDir.removeRecursively())
        {
            Q_EMIT ShowMessage("ERROR", "Failed to delete directory " + folder + " due to access control from another program");
            return false;
        }
    }
    if (!caseSystemVarDir.mkpath(caseSystemVarDir.absolutePath()))
    {
        Q_EMIT ShowMessage("ERROR", "Failed to create directory " + folder);
        return false;
    }

    return true;
}

void ModelPcm::RelocateCog(PCM_SimulationSet *simulationSet)
{
    for (uint i = 0; i < simulationSet->GetTrajectories().size(); i++)
    {
        if (simulationSet->GetParticipants().at(i)->GetType() == "0")
        {
            double wheelBase = simulationSet->GetParticipants().at(i)->GetWheelbase().toDouble();
            double distCgfa = simulationSet->GetParticipants().at(i)->GetDistcgfa().toDouble();
            double distanceRearAxleToCOG = wheelBase - distCgfa;

            simulationSet->GetTrajectories().at(i)->ShiftForward(distanceRearAxleToCOG);
        }
    }
}

PCM_SimulationSet *ModelPcm::ReadSimulationSetFromDb(QString pcmCase)
{
    // Read from database
    DatabaseReader dbReader;
    dbReader.SetDatabase(currentPcmFilePath);
    PCM_SimulationSet *simulationSet = dbReader.Read(pcmCase);

    if (simulationSet == nullptr)
    {
        Q_EMIT ShowMessage("ERROR", "Failed to read database for case: " + pcmCase);
        return nullptr;
    }

    // Relocate trajectory because pcm cog is in the geometric middle of the car
    // but simulated cog is between the rear axle
    RelocateCog(simulationSet);

    return simulationSet;
}

PCM_SimulationSet *ModelPcm::ReadSimulationSetFromPrevCase(QString pcmCase)
{
    // Generate Configs from previous result folder
    QString prevCaseFolder = prevResultFolder + "/" + pcmCase;
    QString prevCaseSystemVarFolder, prevSystemName;

    QDirIterator it(prevCaseFolder, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    while (it.hasNext())
    {
        prevSystemName = QFileInfo(it.next()).baseName();          // get the base name of the subfolder without path prefix
        if (QRegExp(REGEX_CASE_SYSTEM).exactMatch(prevSystemName)) // check if the subfolder name matches the pattern like "0-0-0"
        {
            prevCaseSystemVarFolder = prevCaseFolder + "/" + prevSystemName + "/" + DIR_NO_VARIATION + "/" + FILENAME_OPENPASSSIMULATION_CONFIGS;
            QDir prevCaseSystemVarDir(prevCaseSystemVarFolder);
            if (prevCaseSystemVarDir.exists())
            {
                break; // found a valid previous result folder
            }
        }
    }

    if (prevCaseSystemVarFolder.isEmpty())
    {
        Q_EMIT ShowMessage("ERROR", "No valid case result folder in previous results for case " + pcmCase);
        return nullptr;
    }

    ConfigSetReaderPcm configSetReader(prevCaseSystemVarFolder);
    PCM_SimulationSet *simulationSet = configSetReader.Read();

    if (simulationSet == nullptr)
    {
        Q_EMIT ShowMessage("ERROR", "Failed to read database for case: " + pcmCase);
        return nullptr;
    }

    return simulationSet;
}

PCM_SimulationSet *ModelPcm::ReadSimulationSet(bool inputFromDb, QString pcmCase)
{
    if (inputFromDb)
    {
        return ReadSimulationSetFromDb(pcmCase);
    }
    else
    {
        return ReadSimulationSetFromPrevCase(pcmCase);
    }
}

void ModelPcm::SaveState(PCM_SimulationSet *simulationSet)
{
    // Reset to original value without variation
    for (uint i = 0; i < simulationSet->GetTrajectories().size(); ++i)
    {
        simulationSet->GetTrajectories().at(i)->SaveOriginalState();
    }
}

void ModelPcm::ApplyVariation(PCM_SimulationSet *simulationSet, double randomSeed)
{
    std::vector<double> shiftRadiusVector;
    shiftRadiusVector.push_back(shiftRadius1);
    shiftRadiusVector.push_back(shiftRadius2);

    std::vector<double> velocityScaleVector;
    velocityScaleVector.push_back(velocityMaxScale1);
    velocityScaleVector.push_back(velocityMaxScale2);

    if (simulationSet->GetTrajectories().size() == shiftRadiusVector.size())
    {
        for (uint i = 0; i < simulationSet->GetTrajectories().size(); ++i)
        {
            StochasticsPCM stochastics(static_cast<std::uint32_t>(randomSeed));

            if (shiftRadiusEnabled)
            {
                double shiftDistance = stochastics.GetUniformDistributed(0, shiftRadiusVector.at(i));
                double shiftAngle = stochastics.GetUniformDistributed(0, M_PI * 2);
                simulationSet->GetTrajectories().at(i)->ShiftPosition(shiftDistance, shiftAngle);
            }

            if (velocityScaleEnabled)
            {
                double min = 1.0 - velocityScaleVector.at(i) / 100;
                double max = 1.0 + velocityScaleVector.at(i) / 100;
                double velocityScale = stochastics.GetUniformDistributed(min, max);
                simulationSet->GetTrajectories().at(i)->ScaleVelocity(velocityScale);
            }
        }
    }

    return;
}

void ModelPcm::ResetState(PCM_SimulationSet *simulationSet)
{
    // Reset to original value without variation
    for (uint i = 0; i < simulationSet->GetTrajectories().size(); ++i)
    {
        simulationSet->GetTrajectories().at(i)->ResetToOriginalState();
    }
}
