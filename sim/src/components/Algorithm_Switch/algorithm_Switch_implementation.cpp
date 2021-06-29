/*********************************************************************
* Copyright (c) 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

/**
 * @defgroup module_switch Switch
 * The Algorithm component for switch.
 * \image html ecu_switch.png "Overview of Switch"
 */

/**
 * @ingroup module_switch
 * @defgroup switch_group1 Initialization
*/

/**
 * @ingroup module_switch
 * @defgroup switch_group2 Calculation of outputs
*/

#include "algorithm_Switch_implementation.h"

Algorithm_Switch_Implementation::Algorithm_Switch_Implementation(std::string componentName,
                                                                 bool isInit,
                                                                 int priority,
                                                                 int offsetTime,
                                                                 int responseTime,
                                                                 int cycleTime,
                                                                 StochasticsInterface *stochastics,
                                                                 const ParameterInterface *parameters,
                                                                 PublisherInterface * const publisher,
                                                                 const CallbackInterface *callbacks,
                                                                 AgentInterface *agent) :
          AlgorithmInterface(
              componentName,
              isInit,
              priority,
              offsetTime,
              responseTime,
              cycleTime,
              stochastics,
              parameters,
              publisher,
              callbacks,
              agent)
{
    LOGINFO("Constructing Algorithm_Switch");

	/** @addtogroup switch_group1
     * Initialize the output control signal to zero values. Set input parameters to default.
    */

    int noActivityNoCollision = -1;
    index.SetDefaultValue(noActivityNoCollision);

    ControlData defaultControl = {0.0, 0.0, 0.0, {0.0, 0.0, 0.0, 0.0}};
    driverControl.SetDefaultValue(defaultControl);
    prio1Control.SetDefaultValue(defaultControl);
    prio2Control.SetDefaultValue(defaultControl);
    prio3Control.SetDefaultValue(defaultControl);

    resultingControl.SetValue(defaultControl);

    switchAssist = new Switch();

    LOGINFO("Constructing Algorithm_Switch successful");
}

Algorithm_Switch_Implementation::~Algorithm_Switch_Implementation()
{
    delete switchAssist;
    switchAssist = nullptr;
}

void Algorithm_Switch_Implementation::UpdateInput(int localLinkId,
                                                       const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    bool success = inputPorts.at(localLinkId)->SetSignalValue(data);
    if (success)
        LOGDEBUG(QString().sprintf("Algorithm_Switch: Update input #%d successful", localLinkId).toStdString());
    else
        LOGERROR(QString().sprintf("Algorithm_Switch: Update input #%d failed", localLinkId).toStdString());
}

void Algorithm_Switch_Implementation::UpdateOutput(int localLinkId,
                                                        std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    bool success = outputPorts.at(localLinkId)->GetSignalValue(data);
    if (success)
        LOGDEBUG(QString().sprintf("Algorithm_Switch: Update output #%d successful", localLinkId).toStdString());
    else
        LOGERROR(QString().sprintf("Algorithm_Switch: Update output #%d failed", localLinkId).toStdString());
}

void Algorithm_Switch_Implementation::Trigger(int time)
{
    Q_UNUSED(time);
	
    ControlData result = switchAssist->Perform(index.GetValue(), driverControl.GetValue(), prio1Control.GetValue(), prio2Control.GetValue(), prio3Control.GetValue());

    resultingControl.SetValue(result);

    return;
}
