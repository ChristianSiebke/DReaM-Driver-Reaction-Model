/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "DatabaseReaderPcm.h"

DatabaseReader::DatabaseReader()
{
}

DatabaseReader::~DatabaseReader()
{
    CloseDataBase();
}

bool DatabaseReader::SetDatabase(QString &dbName)
{
    if (!QFileInfo::exists(dbName))
    {
        databaseName = "";
        return false;
    }

    if (IsDataBaseOpen())
    {
        CloseDataBase();
    }

    databaseName = dbName;

    return true;
}

bool DatabaseReader::OpenDataBase()
{
    if (!QFileInfo::exists(databaseName))
    {
        return false;
    }

    bool success = true;

    if (IsDataBaseOpen())
    {
        CloseDataBase();
    }

    db = QSqlDatabase::addDatabase("QODBC");
    QString dbString = "Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ="
                       + databaseName;
    db.setDatabaseName(dbString);
    connection = db.connectionName();

    success =  db.open();
    if (!success)
    {
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connection);
        connection = "";
    }

    return success;
}

bool DatabaseReader::CloseDataBase()
{
    if (IsDataBaseOpen())
    {
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connection);
        connection = "";
    }

    return true;
}

bool DatabaseReader::IsDataBaseOpen()
{
    return db.isOpen();
}

bool DatabaseReader::ReadCaseList(QStringList &caseList)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    // Read the Cases from the table "participant_data"
    QSqlQuery query;

    query.exec("SELECT DISTINCT FALL FROM participant_data ORDER BY FALL ASC");
    while (query.next())
    {
        caseList.append(QString("%1").arg(query.value(0).toInt()));
    }

    query.clear();

    return true;

}

PCM_SimulationSet *DatabaseReader::Read(const QString &pcmCase)
{
    std::vector<PCM_ParticipantData *> participants;
    std::vector<PCM_InitialValues *> initials;
    std::vector<PCM_Trajectory *> trajectories;
    PCM_Data *pcmData = new PCM_Data();

    if (!OpenDataBase())
    {
        std::cout << "Error (ConfigGenerator): could not open database." << std::endl;
        return nullptr;
    }

    if (!ReadParticipantData(pcmCase,
                             participants))
    {
        std::cout << "Error (ConfigGenerator): could not read participant data" << std::endl;
        return nullptr;
    }

    if (!ReadDynamicsData(pcmCase,
                          initials))
    {
        std::cout << "Error (ConfigGenerator): could not read dynamics data" << std::endl;
        return nullptr;
    }

    if (!ReadTrajectoryData(pcmCase,
                            trajectories))
    {
        std::cout << "Error (ConfigGenerator): could not read trajectory data" << std::endl;
        return nullptr;
    }

    if (!ReadMarksData(pcmCase,
                       pcmData))
    {
        std::cout << "Error (ConfigGenerator): could not read marks data" << std::endl;
        return nullptr;
    }

    if (!ReadObjectsData(pcmCase,
                         pcmData))
    {
        std::cout << "Error (ConfigGenerator): could not read object data" << std::endl;
        return nullptr;
    }

    if (!ReadViewObjectsData(pcmCase,
                             pcmData))
    {
        std::cout << "Error (ConfigGenerator): could not read viewObject data" << std::endl;
        return nullptr;
    }

    if (!ReadIntendedCourseData(pcmCase,
                                pcmData))
    {
        std::cout << "Error (ConfigGenerator): could not read intendedCourses data" << std::endl;
        return nullptr;
    }

    if (!ReadGlobalData(pcmCase,
                        pcmData))
    {
        std::cout << "Error (ConfigGenerator): could not read intendedCourses data" << std::endl;
        return nullptr;
    }

    CloseDataBase();

    return new PCM_SimulationSet(participants,
                                 initials,
                                 trajectories,
                                 pcmData);
}

bool DatabaseReader::ReadParticipantData(const QString &pcmCase,
                                         std::vector<PCM_ParticipantData *> &participants)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    QSqlQuery query;
    query.exec("SELECT TYPEPCM,WIDTH,LENGTH,DISTCGFA,WEIGHT,HEIGHTCG,WHEELBASE,IXX,IYY,IZZ,MUE,TRACKWIDTH,HEIGHT,CGFRONT FROM participant_data WHERE FALL = "
               + pcmCase);
    while (query.next())
    {
        participants.push_back(new PCM_ParticipantData(query.value(0).toString(),
                                                       query.value(1).toString(),
                                                       query.value(2).toString(),
                                                       query.value(3).toString(),
                                                       query.value(4).toString(),
                                                       query.value(5).toString(),
                                                       query.value(6).toString(),
                                                       query.value(7).toString(),
                                                       query.value(8).toString(),
                                                       query.value(9).toString(),
                                                       query.value(10).toString(),
                                                       query.value(11).toString(),
                                                       query.value(12).toString(),
                                                       query.value(13).toString()));
    }

    query.clear();

    return !(participants.empty());
}

bool DatabaseReader::ReadDynamicsData(const QString &pcmCase,
                                      std::vector<PCM_InitialValues *> &initials)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    QSqlQuery query;
    query.exec("SELECT XPOS,YPOS,VX,VY,AX,AY,PSI FROM dynamics WHERE STEP = 0 AND FALL = " + pcmCase);
    while (query.next())
    {
        initials.push_back(new PCM_InitialValues(query.value(0).toString(),
                                                 query.value(1).toString(),
                                                 query.value(2).toString(),
                                                 query.value(3).toString(),
                                                 query.value(4).toString(),
                                                 query.value(5).toString(),
                                                 query.value(6).toString()));
    }

    query.clear();

    return !(initials.empty());
}

bool DatabaseReader::ReadTrajectoryData(const QString &pcmCase,
                                        std::vector<PCM_Trajectory *> &trajectories)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    QSqlQuery query;
    query.exec("SELECT BETNR FROM dynamics WHERE FALL = " + pcmCase + " GROUP BY BETNR");
    std::vector<QString>betNr;
    while (query.next())
    {
        betNr.push_back(query.value(0).toString());
    }

    query.clear();

    for (size_t i = 0; i < betNr.size(); i++)
    {
        query.exec("SELECT STEP,XPOS,YPOS,VX,VY,PSI FROM dynamics WHERE BETNR = " + betNr.at(i) + "AND FALL = " + pcmCase + " ORDER BY STEP ASC");

        std::vector<double> *timeVec = new std::vector<double>();
        std::vector<double> *xPosVec = new std::vector<double>();
        std::vector<double> *yPosVec = new std::vector<double>();
        std::vector<double> *uVelVec = new std::vector<double>();
        std::vector<double> *vVelVec = new std::vector<double>();
        std::vector<double> *psiVec = new std::vector<double>();

        while (query.next())
        {
            timeVec->push_back(query.value(0).toDouble());
            xPosVec->push_back(query.value(1).toDouble());
            yPosVec->push_back(query.value(2).toDouble());
            uVelVec->push_back(query.value(3).toDouble());
            vVelVec->push_back(query.value(4).toDouble());
            psiVec->push_back(query.value(5).toDouble());
        }

        trajectories.push_back(new PCM_Trajectory(timeVec,
                                                  xPosVec,
                                                  yPosVec,
                                                  uVelVec,
                                                  vVelVec,
                                                  psiVec));
    }

    query.clear();

    return !(trajectories.empty());
}

bool DatabaseReader::ReadMarksData(const QString &pcmCase, PCM_Data *pcmData)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    for (size_t i = 1; i < static_cast<int>(MarkType::NumberOfMarkTypes); i++)
    {
        PCM_Marks *marks = new PCM_Marks(static_cast<MarkType>(i));

        QString marksTypeName = QString::fromStdString(PCM_Helper::ConvertMarkTypeToDBString(
            marks->GetMarkType()));

        QSqlQuery query;
        query.exec( "SELECT LINENO,POINTNO,X,Y,Z FROM " + marksTypeName + " WHERE FALL = " + pcmCase +
                    " ORDER BY LINENO, POINTNO");

        while (query.next())
        {
            int lineNo = query.value(0).toInt();
            int pointNo = query.value(1).toInt();
            double x = query.value(2).toDouble();
            double y = query.value(3).toDouble();
            double z = query.value(4).toDouble();

            AddLineData(marks, lineNo, pointNo, x, y, z);
        }

        pcmData->AddPCM_Marks(marks);
        query.clear();
    }

    return true;
}

bool DatabaseReader::ReadObjectsData(const QString &pcmCase, PCM_Data *pcmData)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    QString objectsName = QString::fromStdString(PCM_Helper::ConvertObjectTypeToDBString(
                                                     ObjectType::OBJECT));

    QSqlQuery query;
    query.exec("SELECT LINENO,POINTNO,X,Y,Z,OBJTYPE FROM " + objectsName + " WHERE FALL = " + pcmCase +
               " ORDER BY LINENO, POINTNO");

    PCM_Object *object = new PCM_Object();
    while (query.next())
    {
        int lineNo = query.value(0).toInt();
        int pointNo = query.value(1).toInt();
        double x = query.value(2).toDouble();
        double y = query.value(3).toDouble();
        double z = query.value(4).toDouble();
        int objectType = query.value(5).toInt();

        AddLineData(object, lineNo, pointNo, x, y, z);
        object->SetObjectType(objectType);
    }
    pcmData->SetPCM_Object(object);

    query.clear();

    return true;
}

bool DatabaseReader::ReadViewObjectsData(const QString &pcmCase, PCM_Data *pcmData)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    QString viewObjectsName = QString::fromStdString(PCM_Helper::ConvertObjectTypeToDBString(
                                                         ObjectType::VIEWOBJECT));

    QSqlQuery query;
    query.exec("SELECT LINENO,POINTNO,X,Y,Z FROM " + viewObjectsName + " WHERE FALL = " + pcmCase +
               " ORDER BY LINENO, POINTNO");

    PCM_ViewObject *viewObject = new PCM_ViewObject();
    while (query.next())
    {
        int lineNo = query.value(0).toInt();
        int pointNo = query.value(1).toInt();
        double x = query.value(2).toDouble();
        double y = query.value(3).toDouble();
        double z = query.value(4).toDouble();

        AddLineData(viewObject, lineNo, pointNo, x, y, z);
    }

    pcmData->SetPCM_ViewObject(viewObject);
    query.clear();

    return true;
}

bool DatabaseReader::ReadIntendedCourseData(const QString &pcmCase,
                                            PCM_Data *pcmData)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    QString intendedCourseName = QString::fromStdString(
                                     PCM_Helper::GetIntendedCourseDBString());

    QSqlQuery query;
    query.exec( "SELECT BETNR,POINTNO,X,Y,Z FROM " + intendedCourseName + " WHERE FALL = " + pcmCase +
                " ORDER BY BETNR, POINTNO");

    while (query.next())
    {
        int betNr = query.value(0).toInt();
        int pointNo = query.value(1).toInt();
        double x = query.value(2).toDouble();
        double y = query.value(3).toDouble();
        double z = query.value(4).toDouble();

        PCM_Course *course = new PCM_Course(betNr);

        if (!course->IsPointPresent(pointNo))
        {
            PCM_Point *newPoint = new PCM_Point(pointNo, x, y, z);
            course->AddPCM_Point(newPoint);
        }
        else
        {
            std::cout << "WARNING: Point: " << std::to_string(pointNo)
                      << " already exists in course: "
                      << std::to_string(betNr)
                      << ". Point is not added to line." << std::endl;
        }

        pcmData->AddPCM_Course(course);
    }

    query.clear();

    return true;
}

bool DatabaseReader::ReadGlobalData(const QString &pcmCase, PCM_Data *pcmData)
{
    if (!IsDataBaseOpen())
    {
        return false;
    }

    QString globalDataName = QString::fromStdString(PCM_Helper::GetGlobalDataDBString());

    QSqlQuery query;
    query.exec("SELECT OFFSETX,OFFSETY,PARTICIP,SIMUVERS FROM " + globalDataName + " WHERE FALL = " +
               pcmCase);

    PCM_GlobalData *globalData = new PCM_GlobalData();
    while (query.next())
    {
        double offsetX = query.value(0).toDouble();
        double offsetY = query.value(1).toDouble();
        int participants = query.value(2).toInt();
        int simulationVersion = query.value(3).toInt();

        globalData->SetOffsetX(offsetX);
        globalData->SetOffsetY(offsetY);
        globalData->SetParticipants(participants);
        globalData->SetSimulationVersion(simulationVersion);
    }
    pcmData->SetPCM_GlobalData(globalData);

    query.clear();

    return true;
}

bool DatabaseReader::AddLineData(PCM_LineContainer *lineContainer, int lineNo, int pointNo,
                                 double x, double y, double z)
{
    if (!lineContainer->IsLinePresent(lineNo))
    {
        PCM_Line *newPcmLine = new PCM_Line(lineNo);
        lineContainer->AddPCM_Line(newPcmLine);
    }
    PCM_Line *line = lineContainer->GetLineById(lineNo);

    if (!line->IsPointPresent(pointNo))
    {
        PCM_Point *newPoint = new PCM_Point(pointNo, x, y, z);
        line->AddPCM_Point(newPoint);
    }
    else
    {
        std::cout << "WARNING: Point: " << std::to_string(pointNo)
                  << " already exists in line: "
                  << std::to_string(lineNo)
                  << ". Point is not added to line." << std::endl;
        return false;
    }

    return true;
}
