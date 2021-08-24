# What is openPASS

The openPASS (Open Platform for Assessment of Safety Systems) tool is an open source developed framework for the simulation of interaction between traffic participants to evaluate and parametrize active safety systems. The simulation is based on a specific situation configuration and can contain several simulation runs, which differ due to random parameters.

The software suite of openPASS started as a set of stand-alone applications, which can be installed and configured individually. Over time, especially the graphical user interface evolved to a single entry point, enabling the average user to use openPASS as a “monolithic” tool.

# Where to get it

As the program is still under development and is extended continuously, we advice you to use the latest sources from our [GitLab repository](https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass). Pleaes download from the master branch which consists the most stable and recent openPASS version. The servant branch contains current developments which are planned to be pushed to the master branch after a comprehensive review by the openPASS Working Group.

# Installation
An installation guide can be found [here](https://www.eclipse.org/simopenpass/content/html/index.html).

# FAQs
A list of frequently asked questions:
1. __Which standards does openPASS support?__

* ASAM OpenSCENARIO 1.0
* ASAM OpenDRIVE 1.6
* OSI 3.2.0 (a switch to OSI 3.3 is planned)
* FMI 1
* FMI 2

2. __Which traffic signs does openPASS support?__

[Here](https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass/-/blob/servant/sim/doc/DoxyGen/Function/Markdown/Simulation/Development/FrameworkModules.md#L310) is an overview over the supported traffic signs. 

3. __Can openPASS simulate parallel processes?__

Currently openPASS only computes sequential time steps because openPASS only compute on one core. Scheduler agents can be computed in parallel. Synchronization works only after all agents have been computed.

4. __Where do I find information about the accidents?__

The output of each simulation is written to a simulationOutput.xml and potentially additional files. They contain information on the agents of the simulation along with cyclic data from all timesteps. Further events are logged which will contain collision along with a lot of other information.

5. __Can openPASS calculate criticality KPIs?__

OpenPASS can calculate Time-To-Collision (TTC) and Time-To-Headway (THW). The output of those numbers is quite challenging. It is possible to extract those numbers over a FMU.

6. __Which probability distributions for parameter variations can be used in openPASS?__

Parameters can either be simple or stochastic. Simple parameters only have one value, while stochastic parameters have a minimum and maximum value as well as distribution specific parameters. If a parameter is stochastic a distribution can be choosen from [this list](https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass/-/blob/servant/sim/src/core/slave/modules/Stochastics/stochastics_implementation.h). In future (with OpenSCENARIO 1.1) the number of distributions will be extended.

# Contact

For more information, contact or subscribe to the [public WG mailing list](https://dev.eclipse.org/mailman/listinfo/openpass-wg).

For addressing especially developers of openPASS, contact or subscribe to the [developer mailing list]( simopenpass-dev@eclipse.org).
