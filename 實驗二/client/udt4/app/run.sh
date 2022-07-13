#!/bin/bash

test=`pwd`;
test="LD_LIBRARY_PATH=$test"
export $test
echo $LD_LIBRARY_PATH
echo $test
