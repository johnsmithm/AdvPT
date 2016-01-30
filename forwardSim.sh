#!/bin/bash

race=$(echo $1 | cut -c 10- )
buildList=$2

$(dirname $0)/bin/SC2BuildOptimize simulate $race $buildList