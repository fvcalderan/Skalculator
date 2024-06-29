#!/bin/sh

# This script calls calc_sine.ska to calculate the sine of the argument
# Run it from Skalc's root folder. Program should open showing the result.
# Example execution: ./examples/run_example.sh 1.570795
# Skalculator should open showing 1.00001, which is ~sine(pi/2 = 1.570795).

sed "s/A/$1/g" examples/calc_sine.ska | xargs -0 ./skalculator
