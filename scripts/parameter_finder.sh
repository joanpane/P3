#!/bin/bash

DIR_P3=$HOME/PAV/P3
DB=$DIR_P3/pitch_db/train
CMD="get_pitch "
BESTSCORE=0

param1=0.7 param2=0 param3=-40

#apt install dc

for param1 in $(seq .5 .005 .7); do 
#for param2 in $(seq 0 .3 1); do
#for param3 in $(seq 0 .010 0.2); do
    echo -e -n "\rPitchAnalyzer with param1=$param1, param2=$param2, param3=$param3"
    for filewav in pitch_db/train/*.wav; do
        ff0=${filewav/.wav/.f0}
        #echo "$CMD --param1 $param1 --param2 $param2 --param3 $param3 $filewav $ff0"
        $CMD --param1 $param1 --param2 $param2 --param3 $param3 $filewav $ff0 || (echo -e "\nError in $CMD $filewav $ff0" && exit 1) 
    done
    A=$(pitch_evaluate pitch_db/train/*.f0ref)
    if ! echo "$A $BESTSCORE -p" | dc | grep > /dev/null ^-; then
        BESTSCORE=$A
        
        clear
        echo -e "\nNew Best score $BESTSCORE with parameters  param1=$param1, param2=$param2, param3=$param3"
    fi
     done # done # done 
exit 0