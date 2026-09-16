#!/bin/bash

CONFIG="example/advanced/simpleFit/mainConfig.yaml"

for i in $(seq 0 2); do
    echo "Running toy ${i}..."
    gundamFitter -d -c "$CONFIG" -of ./example/advanced/simpleFit/overrides/toysSettings.yaml -t 4 --toy "$i"
done
