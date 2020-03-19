# Simulation for DESY19 configuration

## Status
Emulsion ECCs for all configurations inserted. Positions between them still arbitrary
Placeholders for SciFi detectors inserted. Just dummy air volumes for now

## Instructions

Launch with 

 python $FAIRSHIP/macro/run_simScript.py --desy19 nrun --PG --pID 11 -n nevents -o outputdir

Event display, copied from the standard FairSHIP EventDisplay

 python -i $FAIRSHIP/macro/desy19_eventDisplay.py -f ship.conical.PG_11-TGeant4.root -g geofile_full.conical.PG_11-TGeant4.root