#!/bin/bash

while :
do
    rm pitch_db/train/rl004.f0
    get_pitch --param1 0.7 pitch_db/train/rl004.wav pitch_db/train/rl004.f0
    pitch_evaluate pitch_db/train/rl004.f0ref
    sleep 1

done
exit 0
