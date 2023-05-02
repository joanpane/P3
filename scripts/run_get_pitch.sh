#!/bin/bash

# Put here the program (maybe with path)
GETF0="get_pitch --param1 0.3 --param2 0.8 --param3 -40 -w RECT"
#GETF0="get_pitch --param1 0.6"

# make release

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    ff0ref=${fwav/.wav/.f0ref}
    #echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 || (echo -e "\nError in $GETF0 $fwav $ff0" && exit 1)
    A=$(pitch_evaluate $ff0ref)
    echo "$fwav - $A"
done

echo "TOTAL: "
pitch_evaluate pitch_db/train/*.f0ref 

exit 0
