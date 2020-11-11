#!/usr/bin/env bash
cmake .. -DCMAKE_PREFIX_PATH=$CONDA_PREFIX
make
