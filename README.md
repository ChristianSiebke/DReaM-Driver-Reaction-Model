# What is openPASS

OpenPASS (Open Platform for Assessment of Safety Systems) is an open source simulation platform for the effectiveness analysis of advanced driver and assistance systems (ADAS) and autonomous driving (AD) systems. OpenPASS takes a specific but configurable traffic situation and offers possibilities to repeat this situation under varying conditions, each generated from a diverse set of user defined probability distributions, such as for vehicle speeds. As a result, multiple simulation outputs are generated which can be evaluated in each case.

The software suite of openPASS started as a set of stand-alone applications, which can be installed and configured individually. Over time, especially the graphical user interface evolved to a single entry point, enabling the average user to use openPASS as a “monolithic” tool.

# Where to get it

As the program is still under development and is extended continuously, we advice you to use the latest sources from our [GitLab repository](https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass).
The `main` branch contains the most stable and recent openPASS version. 
The `develop` branch contains current developments which are planned to be pushed to the main branch after a comprehensive review by the openPASS Working Group.

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

3. __Can openPASS simulate multiple simulation runs in parallel?__

OpenPASS has a strong focus on performance. Simulations commonly run multiple times faster than real time, even though no parallelization is used within a simulation so far. As each simulation only uses a single core, multiple independent simulations are executed as parallel processes to speed up simulation campaigns.

4. __Where do I find information about the accidents?__

Depending on the chosen logger library, the output of each simulation is written to a single file (simulationOutput.xml) or a pair of files (Cyclics_Run_XXX.csv + simulationOutput.xml). They contain information on the agents of the simulation along with cyclic data from all timesteps. Further events are logged which will contain detailed data on collisions along with a lot of other information.

5. __Can openPASS calculate criticality KPIs?__

OpenPASS can calculate Time-To-Collision (TTC) and Time-To-Headway (THW). The output of those numbers is quite challenging. It is possible to extract those numbers over a FMU.

6. __Which probability distributions for parameter variations can be used in openPASS?__

Parameters can either be simple or stochastic. Simple parameters only have one value, while stochastic parameters have a minimum and maximum value as well as distribution specific parameters. If a parameter is stochastic, a distribution can be choosen from [this list](https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass/-/blob/servant/sim/src/core/opSimulation/modules/Stochastics/stochastics_implementation.h). In future (with OpenSCENARIO 1.1) the number of distributions will be extended.

# Contact

For more information, contact or subscribe to the [public WG mailing list](https://dev.eclipse.org/mailman/listinfo/openpass-wg).

For addressing especially developers of openPASS, contact or subscribe to the [developer mailing list]( simopenpass-dev@eclipse.org).
