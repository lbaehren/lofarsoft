#!/bin/bash

echo $1
./test/feedfile.py $1 | VHECRtest
