#!/bin/bash

while :
do
    get_pitch pitch_db/train/rl004.wav pitch_db/train/rl004.f0
    pitch_evaluate pitch_db/train/rl004.f0ref
    sleep 1

done
exit 0
