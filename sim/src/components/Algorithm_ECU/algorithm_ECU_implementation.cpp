/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

/**
 * @defgroup module_eCU ECU
 * The Algorithm component for eCU.
 * \image html ecu_switch.png "Overview of ECU"
 */

/**
 * @ingroup module_eCU
 * @defgroup eCU_group1 Initialization
*/

/**
 * @ingroup module_eCU
 * @defgroup eCU_group2 Decision making
*/

#include "algorithm_ECU_implementation.h"

Algorithm_ECU_Implementation::Algorithm_ECU_Implementation(std::string componentName,
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
    LOGINFO("Constructing Algorithm_ECU");

	/** @addtogroup eCU_group1
     * Initialize assistance system to default for no activation and no collision. Set defaults for inputs.
    */
    int noActivityNoCollision = -1;
    index.SetValue(noActivityNoCollision);

    collisionOccurred.SetDefaultValue(false);
    prio1AssistActivity.SetDefaultValue(false);
    prio2AssistActivity.SetDefaultValue(false);
    prio3AssistActivity.SetDefaultValue(false);

    ecu = new Ecu();

    LOGINFO("Constructing Algorithm_ECU successful");
}

Algorithm_ECU_Implementation::~Algorithm_ECU_Implementation()
{
    delete ecu;
    ecu = nullptr;
}

void Algorithm_ECU_Implementation::UpdateInput(int localLinkId,
                                                       const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    bool success = inputPorts.at(localLinkId)->SetSignalValue(data);
    if (success)
        LOGDEBUG(QString().sprintf("Algorithm_ECU: Update input #%d successful", localLinkId).toStdString());
    else
        LOGERROR(QString().sprintf("Algorithm_ECU: Update input #%d failed", localLinkId).toStdString());
}

void Algorithm_ECU_Implementation::UpdateOutput(int localLinkId,
                                                        std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    bool success = outputPorts.at(localLinkId)->GetSignalValue(data);
    if (success)
        LOGDEBUG(QString().sprintf("Algorithm_ECU: Update output #%d successful", localLinkId).toStdString());
    else
        LOGERROR(QString().sprintf("Algorithm_ECU: Update output #%d failed", localLinkId).toStdString());
}

void Algorithm_ECU_Implementation::Trigger(int time)
{
    Q_UNUSED(time);
	
	/** @addtogroup eCU_group2
     * Decide which assistance system to use, based on the logic of collision and system activations.
    */

    int indexResult = ecu->Perform(collisionOccurred.GetValue(), prio1AssistActivity.GetValue(), prio2AssistActivity.GetValue(), prio3AssistActivity.GetValue());

    index.SetValue(indexResult);

    return;
}
