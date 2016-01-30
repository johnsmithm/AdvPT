#!/bin/bash

declare -A targetMap
targetMap[terran_rush_A]="marine"
targetMap[terran_rush_B]="marauder"
targetMap[terran_push_A]="battlecruser"
targetMap[terran_push_B]="siege_tank"

targetMap[protoss_rush_A]="zealot"
targetMap[protoss_rush_B]="stalker"
targetMap[protoss_push_A]="void_ray"
targetMap[protoss_push_B]="colossus"

targetMap[zerg_rush_A]="zergling"
targetMap[zerg_rush_B]="roach"
targetMap[zerg_push_A]="brood_lord"
targetMap[zerg_push_B]="ultralisk"

#supported targets as comma separated list
supported="colossus"

race=$(echo $1 | cut -d "_" -f 1)
mode=$(echo $1 | cut -d "_" -f 2)
target="${targetMap["$1"]}"

echo $supported | grep $target - >/dev/null
if [[ $? == 0 ]]; then
	$(dirname $0)/bin/SC2BuildOptimize optimize $race $mode $target
fi
