#!/bin/bash

mkdir $(dirname $0)/bin
cd $(dirname $0)/bin

cmake ../src
make