/**********************************************
* Copyright (c) 2018 in-tech GmbH             *
* on behalf of BMW AG                         *
***********************************************/


//-----------------------------------------------------------------------------
//! @file  FmuHandlerInterface.h
//! @brief This file contains the interface for FMU interaction
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <unordered_map>
#include <variant>

#include "include/callbackInterface.h"
#include "include/signalInterface.h"

#include "FMI1/fmi1_import_variable_list.h"

static constexpr size_t FMI1 = 0;
static constexpr size_t FMI2 = 1;

enum class VariableType
{
    Bool,
    Int,
    Double,
    String,
    Enum
};

typedef std::pair<fmi1_value_reference_t, VariableType> ValueReferenceAndType1;
typedef std::pair<fmi2_value_reference_t, VariableType> ValueReferenceAndType2;
using ValueReferenceAndType = std::variant<ValueReferenceAndType1, ValueReferenceAndType2>;

class AgentInterface;
struct fmu_check_data_t;

using Fmu1Variables = std::unordered_map<std::string, ValueReferenceAndType1>;
using Fmu2Variables= std::unordered_map<std::string, ValueReferenceAndType2>;
using FmuVariables = std::variant<Fmu1Variables, Fmu2Variables>;

/*!
 * \brief The FmuHandlerInterface class provides an interface for implementations of the FMU type-specific parts of the FmuWrapper
 */
class FmuHandlerInterface
{
public:
    FmuHandlerInterface(fmu_check_data_t* cdata, AgentInterface* agent, const CallbackInterface* callbacks) :
        cdata(cdata),
        agent(agent),
        callbacks(callbacks) {}

    FmuHandlerInterface(const FmuHandlerInterface&) = delete;
    FmuHandlerInterface(FmuHandlerInterface&&) = delete;
    FmuHandlerInterface& operator=(const FmuHandlerInterface&) = delete;
    FmuHandlerInterface& operator=(FmuHandlerInterface&&) = delete;
    virtual ~FmuHandlerInterface() = default;

    /*!
     * \brief Has to be called from UpdateInput in AlgorithmFmuWrapper
     *
     * \param[in]   localLinkId     Local link id forwarded from FmuWrapper
     * \param[in]   data            Shared pointer of signal interface forwarded from FmuWrapper
     * \param[in]   time            Timestep forwarded from FmuWrapper
     */
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time) = 0;

    /*!
     * \brief Has to be called from UpdateOutput in AlgorithmFmuWrapper
     *
     * \param[in]   localLinkId     Local link id forwarded from FmuWrapper
     * \param[in]   data            Shared pointer of signal interface forwarded from FmuWrapper
     * \param[in]   time            Timestep forwarded from FmuWrapper
     */
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time) = 0;

    /*!
     * \brief Has to be called from Trigger in AlgorithmFmuWrapper
     *
     * \param[in]   time            Timestep forwarded from FmuWrapper
     */
    virtual void PreStep(int time) = 0;

    /*!
     * \brief Has to be called from Trigger in AlgorithmFmuWrapper
     *
     * \param[in]   time            Timestep forwarded from FmuWrapper
     */
    virtual void PostStep(int time) = 0;

    //-----------------------------------------------------------------------------
    //! Provides callback to LOG() macro.
    //!
    //! @param[in]     logLevel    Importance of log
    //! @param[in]     file        Name of file where log is called
    //! @param[in]     line        Line within file where log is called
    //! @param[in]     message     Message to log
    //-----------------------------------------------------------------------------
    void Log(CbkLogLevel logLevel, const char *file, int line, const std::string &message)
    {
        if (callbacks)
        {
            callbacks->Log(logLevel, file, line, message);
        }
    }

    //! List of supported FMU types
    //! \note has to be instantiated in FmuWrapper implementation with all supported types
    enum class FmuType;

    //! Maps FMU type strings to enumeration values
    //! \note has to be instantiated in FmuWrapper implementation with all supported type strings
    static const std::unordered_map<std::string, FmuType> fmuTypeStrings;

    //! Common storage for values from FMUs. User has to take care of accessing the correct datatype.
    union FmuValue
    {
        bool boolValue;
        int intValue;
        double realValue;
        char* stringValue;
    };

protected:
    struct fmu_check_data_t* cdata;   //!< check data to be passed around between the FMIL functions

    int    offsetTime   = 0;        //!< after spawn  call of trigger is delayed by this value (ms)
    int    responseTime = 0;        //!< call of UpdateOutput after responseTime elapsed (ms)
    int    cycleTime    = 100;      //!< Function Trigger of this model is called again after cycleTime  (ms)
    int    startTime;               //!< Time of FMU start (simulation timebase)

    AgentInterface* agent;          //!< References the agent interface
    const CallbackInterface* callbacks;   //!< References the callback functions of the framework
};
