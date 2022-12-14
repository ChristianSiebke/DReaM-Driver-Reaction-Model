# Contributing to Eclipse sim@openpass

This guide provides all necessary information to enable [contributors and committers](https://www.eclipse.org/projects/dev_process/#2_3_1_Contributors_and_Committers) to contribute to Eclipse sim@openPASS. 

## Eclipse sim@openPASS  
Eclipse sim@openPASS provides a software platform that enables the simulation of traffic scenarios to predict the real-world effectiveness of advanced driver assistance systems or automated driving functions. 

## Developer resources

  * [Working Group Website](https://openpass.eclipse.org/)
  * [Developer Website](https://projects.eclipse.org/projects/automotive.simopenpass)
  * Mailing list: Join our [developer list](https://accounts.eclipse.org/mailing-list/simopenpass-dev)
  * Bugs can be reported in [GitLab](https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass/-/issues) by anybody who owns an Eclipse account. Please use type “Incident”
  * [Documentation](https://openpass.eclipse.org/resources/#documentation)

## Eclipse Contributor / Committer Agreement

Before your contribution can be accepted by the project team, contributors and committers must sign the correct agreement depending on their status. Please read on how to proceed on: https://www.eclipse.org/legal/committer_process/re-sign/.

For more information, please see the Eclipse Committer Handbook: https://www.eclipse.org/projects/handbook/#resources-commit.

## Contact

Contact the project developers via the project's "dev" list.

* simopenpass-dev@eclipse.org

## How to contribute

The sim@openpass source code can be found [here](https://gitlab.eclipse.org/eclipse/simopenpass/simopenpass).

To build the project, follow the guide in "pathToRepo\sim\doc\OSI World Setup Guide.pdf".

The branch 'servant' contains the contributions that will be included in the next release. The 'main' branch contains the latest stable release.

### Committer contribution process

1.  (you) Discuss your planned contribution with the project lead first.
2.  (you) If needed, create the Issue on GitLab and assign it to yourself.
3.  (you) In GitLab, create a merge request. The related branch will be created from the branch 'servant'.
4.  (you) Work on it and respect the following points:
    - Stick to the [coding guideline](https://wiki.eclipse.org/images/3/3f/OpenPASS_Conventions.pdf)
    - Amend the documentation if necessary
    - Provide unit-tests in googletest format
    - Make sure, the code base with your contribution compiles
5.	(you) Assign the issue to another committer.
6.	(other committer) Review the code.
7.	(other committer) To merge new branch into 'servant' you can close the merge request via GitLab.
8.	(you) Present your contribution to the openPASS WG
