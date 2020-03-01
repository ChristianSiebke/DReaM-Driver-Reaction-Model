SET (SOURCES 
framework/agentBlueprintProvider.cpp
framework/agentFactory.cpp
framework/agentProfileSampler.cpp
framework/commandLineParser.cpp
framework/configurationContainer.cpp
framework/directories.cpp
framework/dynamicAgentTypeGenerator.cpp
framework/dynamicParametersSampler.cpp
framework/dynamicProfileSampler.cpp
framework/eventNetwork.cpp
framework/frameworkModuleContainer.cpp
framework/invocationControl.cpp
framework/observationModule.cpp
framework/observationNetwork.cpp
framework/runInstantiator.cpp
framework/sampler.cpp 
importer/connection.cpp
importer/csvParser.cpp
importer/eventDetectorImporter.cpp
importer/junction.cpp
importer/manipulatorImporter.cpp
importer/parameterImporter.cpp
importer/profiles.cpp
importer/profilesImporter.cpp
importer/road.cpp
importer/scenario.cpp
importer/scenarioImporter.cpp
importer/scenery.cpp
importer/sceneryImporter.cpp
importer/slaveConfig.cpp
importer/slaveConfigImporter.cpp
importer/systemConfig.cpp
importer/systemConfigImporter.cpp
importer/trajectory.cpp
importer/trajectoryImporter.cpp
importer/vehicleModels.cpp
importer/vehicleModelsImporter.cpp
importer/road/roadObject.cpp
importer/road/roadSignal.cpp
modelElements/agent.cpp
modelElements/agentBlueprint.cpp
modelElements/agentType.cpp
modelElements/channel.cpp
modelElements/component.cpp
modelElements/componentType.cpp
modelElements/eventDetectorNetwork.cpp
modelElements/manipulatorNetwork.cpp
modelElements/spawnPointNetwork.cpp
modelInterface/modelBinding.cpp
modelInterface/modelLibrary.cpp
observationInterface/observationBinding.cpp
observationInterface/observationLibrary.cpp
spawnPointInterface/spawnPointBinding.cpp
spawnPointInterface/spawnPointLibrary.cpp
stochasticsInterface/stochastics.cpp
stochasticsInterface/stochasticsBinding.cpp
stochasticsInterface/stochasticsLibrary.cpp
worldInterface/world.cpp
worldInterface/worldBinding.cpp
worldInterface/worldLibrary.cpp
scheduler/agentParser.cpp
scheduler/runResult.cpp
scheduler/scheduler.cpp
scheduler/schedulerTasks.cpp
scheduler/spawnControl.cpp
scheduler/taskBuilder.cpp
scheduler/tasks.cpp
#collisionDetectionInterface/collisionDetection.cpp
#collisionDetectionInterface/collisionDetectionBinding.cpp
#collisionDetectionInterface/collisionDetectionLibrary.cpp
manipulatorInterface/manipulatorBinding.cpp
manipulatorInterface/manipulatorLibrary.cpp
eventDetectorInterface/eventDetectorBinding.cpp
eventDetectorInterface/eventDetectorLibrary.cpp
)
SET (HEADERS 
framework/agentBlueprintProvider.h
framework/agentFactory.h
framework/agentProfileSampler.h
framework/commandLineParser.h
framework/configurationContainer.h
framework/directories.h
framework/dynamicAgentTypeGenerator.h
framework/dynamicParametersSampler.h
framework/dynamicProfileSampler.h
framework/eventNetwork.h
framework/frameworkModuleContainer.h
framework/invocationControl.h
framework/observationModule.h
framework/observationNetwork.h
framework/runInstantiator.h
framework/sampler.h 
OpenPassSlave.h
importer/configurationFiles.h
importer/connection.h
importer/csvParser.h
importer/eventDetectorImporter.h
importer/frameworkModules.h
importer/junction.h
importer/manipulatorImporter.h
importer/parameterImporter.h
importer/profiles.h
importer/profilesImporter.h
importer/road.h
importer/scenario.h
importer/scenarioImporter.h
importer/scenery.h
importer/sceneryImporter.h
importer/slaveConfig.h
importer/slaveConfigImporter.h
importer/systemConfig.h
importer/systemConfigImporter.h
importer/trajectory.h
importer/trajectoryImporter.h
importer/vehicleModels.h
importer/vehicleModelsImporter.h
importer/road/roadObject.h
importer/road/roadSignal.h
modelElements/agent.h
modelElements/agentBlueprint.h
modelElements/agentType.h
modelElements/channel.h
modelElements/channelBuffer.h
modelElements/component.h
modelElements/componentType.h
modelElements/eventDetector.h
modelElements/eventDetectorNetwork.h
modelElements/manipulator.h
modelElements/manipulatorNetwork.h
modelElements/spawnItemParameter.h
modelElements/spawnPoint.h
modelElements/spawnPointNetwork.h
modelInterface/modelBinding.h
modelInterface/modelLibrary.h
spawnPointInterface/spawnPointBinding.h
spawnPointInterface/spawnPointLibrary.h
stochasticsInterface/stochastics.h
stochasticsInterface/stochasticsBinding.h
stochasticsInterface/stochasticsLibrary.h
worldInterface/world.h
worldInterface/worldBinding.h
worldInterface/worldLibrary.h
scheduler/agentParser.h
scheduler/runResult.h
scheduler/scheduler.h
scheduler/schedulerTasks.h
scheduler/spawnControl.h
scheduler/taskBuilder.h
scheduler/tasks.h
observationInterface/observationBinding.h
observationInterface/observationLibrary.h
manipulatorInterface/manipulatorBinding.h
manipulatorInterface/manipulatorLibrary.h
eventDetectorInterface/eventDetectorBinding.h
eventDetectorInterface/eventDetectorLibrary.h
#collisionDetectionInterface/collisionDetection.h
#collisionDetectionInterface/collisionDetectionBinding.h
#collisionDetectionInterface/collisionDetectionLibrary.h
)




INCLUDE_DIRECTORIES(. collisionDetectionInterface observationInterface scheduler worldInterface stochasticsInterface spawnPointInterface modelInterface modelElements importer framework eventDetectorInterface manipulatorInterface)


OPENPASS_add_library(CoreSlave SHARED)
set_target_properties(CoreSlave PROPERTIES FOLDER "CoreFramework")



set(PCMLibs)

if(BUILD_PCM)
set(PCMLibs Common_PCM)
endif(BUILD_PCM)

target_link_libraries(CoreSlave CoreShare Common $(PCMLibs))

qt5_use_modules(CoreSlave Widgets Xml)

SET (SOURCES 
framework/main.cpp
)
SET (HEADERS 
)


OPENPASS_add_executable(OpenPassSlave )
set_target_properties(OpenPassSlave PROPERTIES FOLDER "CoreFramework")
target_link_libraries(OpenPassSlave CoreSlave)
