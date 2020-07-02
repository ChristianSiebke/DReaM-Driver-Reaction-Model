#pragma once

#include "gmock/gmock.h"
#include "Interfaces/componentInterface.h"

namespace SimulationSlave {

class FakeComponent : public ComponentInterface {
 public:
  MOCK_METHOD1(SetImplementation,
      void(ModelInterface *implementation));
  MOCK_METHOD1(SetParameter,
      void(std::unique_ptr<ParameterInterface>));
  MOCK_CONST_METHOD0(GetAgent,
      Agent*());
  MOCK_METHOD2(AddInputLink,
      bool(Channel *input, int linkId));
  MOCK_METHOD2(AddOutputLink,
      bool(Channel *output, int linkId));
  MOCK_METHOD1(SetObservations,
      void(const std::map<int, ObservationModule*>& observations));
  MOCK_METHOD0(GetInputLinks,
      std::map<int, Channel*>&());
  MOCK_METHOD0(GetOutputLinks,
      std::map<int, Channel*>&());
  MOCK_CONST_METHOD0(GetObservations, const std::map<int, ObservationInterface*>& ());
  MOCK_METHOD1(TriggerCycle,
      bool(int time));
  MOCK_METHOD2(AcquireOutputData,
      bool(int linkId, int time));
  MOCK_METHOD1(ReleaseOutputData,
      bool(int linkId));
  MOCK_METHOD2(UpdateInputData,
      bool(int linkId, int time));
  MOCK_METHOD1(CreateOutputBuffer,
      ChannelBuffer*(int linkId));
  MOCK_METHOD2(SetInputBuffer,
      bool(int linkId, ChannelBuffer *buffer));
  MOCK_CONST_METHOD0(GetInit,
      bool());
  MOCK_CONST_METHOD0(GetPriority,
      int());
  MOCK_CONST_METHOD0(GetOffsetTime,
      int());
  MOCK_CONST_METHOD0(GetResponseTime,
      int());
  MOCK_CONST_METHOD0(GetCycleTime,
      int());
  MOCK_METHOD1(SetModelLibrary,
      bool(ModelLibrary *modelLibrary));
  MOCK_METHOD0(ReleaseFromLibrary,
      bool());
  MOCK_CONST_METHOD0(GetImplementation,
      ModelInterface*());
  MOCK_CONST_METHOD0(GetParameter,
                     const ParameterInterface&());
  MOCK_CONST_METHOD0(GetName,
      std::string());
};

}  // namespace SimulationSlave
