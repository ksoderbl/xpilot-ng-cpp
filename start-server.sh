#!/bin/sh

# Takes map as argument, for example:
# bash start-server.sh lib/maps/j-a-r-n-o.nl/newdarkhellteams.xp 
# Needs to have xpilot-ng-cpp-server installed.

# Build and install xpilot-ng-cpp-server using:
# bash cmake-build.sh
# sudo bash cmake-install.sh

xpilot-ng-cpp-server -fps 50 +reporttometaserver -map "$1"
