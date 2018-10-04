Spider Changelog
================

## Release version X.Y.Z
*XXXX.XX.XX*

### New Feature

### Changes

### Bug fix


## Release version 1.2.1
*2018.10.01*

### New Feature

### Changes
* Add pre-compiled binaries for linux32/64 and win32 in release assets;
* Removed ganttDisplay from spider repository. See https://github.com/preesm/gantt-display. 

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
