#!/bin/bash
brickIDs=(11 12 13 14 21 22 23 24 31 32 33 34 41 42 43 44 51 52 53 54) #ehi, I have written an array in BASH!
for ibrick in $(seq 0 19)
 do
  echo "processing brick ${brickIDs[ibrick]}"
  cd b0000${brickIDs[ibrick]}

   python /home/simsndlhc/macros-snd/FEDRA/add_brickID_number.py ${brickIDs[ibrick]}

  cd ..
 done
