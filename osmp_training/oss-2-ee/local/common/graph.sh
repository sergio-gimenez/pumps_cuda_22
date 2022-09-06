#!/bin/bash

export NANOS6_CONFIG_OVERRIDE="$NANOS6_CONFIG_OVERRIDE,version.instrument=graph,instrument.graph.show_dead_dependencies=true,version.dependencies=regions"

$*
