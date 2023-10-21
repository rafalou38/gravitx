#!/bin/bash

set -xe

# Setup Variables
export NAME_PART="gravitx"
export FILES="./src/gravitx.cpp ./lib/tinyxml2.cpp"

g++ -o "gravitx" $FILES -g -lraylib -lGL -lm -lpthread -ldl -lrt

# ./gravitx