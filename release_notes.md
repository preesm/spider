Spider Changelog
================

## Release version X.Y.Z
*XXXX.XX.XX*

### New Feature
* Complete Refacto of PThread Platform: all communications are now passively blocking, threads will not take 100% CPU for synchronization.
* SRDAG output shows persistent delay inormation
* Static graphs are now only scheduled on first iteration.
* Adding new greedy scheduler to scheduler list
* Adding new SR-DAG less Intermediate Representation (sr-dag remains the main IR for now however)
* Proper handling of SIGINT signal on Linux platforms.
* Adding proper Logging systems inside Spider for outputing messages.
* Adding spiderException method for throwing specific exception with printf like messages.

### Changes
* Enhance Special Actor Memory allocation allowing reduced footprint.
* Move some platform specific types (unsigned long, long, char, etc.) to standard types.
* Changed parameters implementation, reducing number of expression evaluation and making more explicit dependencies between parameters.
* Changed communication system. Communications are now based on an asynchronuous notification system.
* Optimized communications with less synchronisation required.
* Refactored all Rational class to more modern C++.
* Improved performance of BRV computation when using LCM.
* Improved performance of expression evaluation.
* Refactored Scheduler with less code duplication, using more POO.
* Memory allocation is now non page aligned by default (this reduce drastically the memory need of SPIDER)
* Changing Schedule structure.
* Overall clean up of the code base with clang tidy rules.
* Overall clean up of exception messages inside entire code base.
* Overall clean up of dead code and code smells.

### Bug fix
* Fix Graph transformation for actors with repetition is equal to 0.
* Fix Floor and Ceil Expression Resolution.
* Use float for Expression Resolution as Preesm does.
* Fix display of reserved memory usage.
* Avoid reallocation of already allocated fifo in memory allocation.
* Fix double allocation of lrt[0] and lrtCom_[0] in platform pthread.
* Fix memory allocation problem on 64bits systems.
* Fix SRLinker problems when an input interface needs a sink vertex.
* Fix memory allocation and free when size is 0 and pointer is null, respectively.

## Release version 1.2.1
*2018.10.01*

### New Feature

### Changes
* Add pre-compiled binaries for linux32/64 and win32 in release assets;
* Removed ganttDisplay from spider repository. See https://github.com/preesm/gantt-display. 
* Changed BRV computation from matrix topology to LCM method.

### Bug fix


## Release version 1.2.0
*2018.09.25*

### New Feature

### Changes
* Updating Papify to display warning when exceeding number of available counters.
* Adding support of heteregeneous hardware in Spider.
* Adding support of delays in hierarchical graphs.

### Bug fix
* Fixed reduce Join-End graph optimization and persistent delays behavior.

## Release version 1.1.4
*2018.06.11*

### New Feature

### Changes
* Fix release scripts;

### Bug fix
* Fix graph optims while using persitent delays;


## Release version 1.1.3
*2018.06.11*

### New Feature

### Changes
* Fix release scripts;

### Bug fix


## Release version 1.1.2
*2018.06.11*

### New Feature

### Changes
* Update release scripts;

### Bug fix


## Release version 1.1.1
*2018.06.11*

### New Feature

### Changes
* Add release scripts;
* Add travis file;

### Bug fix

## Release version 1.1.0
*2018.06.08*

### New Feature

### Changes
* Changing const char* exception to runtime_errors.
* Changing constructor of platform pthread to take SpiderConfig instead of passing every parameter one by one.
* Applying uniform coding rules to the entire SPiDER lib.
* Cleaning all useless and redundant includes.
* Adding persistent delays.
* Implementation of SAMOS contribution.
* Add Blocking wait on JobQueue
* Added Papify automatic instrumentation to spider
* Add handling of NullActors (with prod/cons equal to 0)
* Adding unitary test framework

### Bug fix
