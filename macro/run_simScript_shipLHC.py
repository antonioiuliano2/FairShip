#!/usr/bin/env python2
from __future__ import print_function
from __future__ import division
import os
import sys
import getopt
import ROOT
import shipunit as u
import shipRoot_conf
import rootUtils as ut
from ShipGeoConfig import ConfigRegistry
from argparse import ArgumentParser


debug = 0  # 1 print weights and field
           # 2 make overlap check

mcEngine     = "TGeant4"
simEngine    = "Genie"  # "Genie" # Ntuple
tag = simEngine+"-"+mcEngine
print(tag)

parser = ArgumentParser()
parser.add_argument("-n", "--nEvents",dest="nEvents",  help="Number of events to generate", required=False,  default=100, type=int)
parser.add_argument("--Genie",   dest="genie",   help="Genie for reading and processing neutrino interactions", required=False, action="store_true")
parser.add_argument("-f",        dest="inputFile",       help="Input file if not default file", required=False, default=False)
parser.add_argument("-o", "--output",dest="outputDir",  help="Output directory", required=False,  default=".")
parser.add_argument("-D", "--display", dest="eventDisplay", help="store trajectories", required=False, action="store_true")
parser.add_argument("-i", "--firstEvent",dest="firstEvent",  help="First event of input file to use", required=False,  default=0, type=int)	

options = parser.parse_args()
if options.inputFile:
  if options.inputFile == "none": options.inputFile = None
  inputFile = options.inputFile
  defaultInputFile = False
else:
  inputFile = "/eos/experiment/ship/data/GenieEvents/genie-nu_mu.root"

if options.eventDisplay: tag = tag+'_D'
if not os.path.exists(options.outputDir):
  os.makedirs(options.outputDir)
outFile = "%s/ship.%s.root" % (options.outputDir, tag)

# rm older files !!! 
for x in os.listdir(options.outputDir):
  if not x.find(tag)<0: os.system("rm %s/%s" % (options.outputDir, x) )
# Parameter file name
parFile="%s/ship.params.%s.root" % (options.outputDir, tag)
 


inclusive    = "c"    # True = all processes if "c" only ccbar -> HNL, if "b" only bbar -> HNL, if "bc" only Bc+/Bc- -> HNL, and for darkphotons: if meson = production through meson decays, pbrem = proton bremstrahlung, qcd = ffbar -> DP.

MCTracksWithHitsOnly   = False  # copy particles which produced a hit and their history
MCTracksWithEnergyCutOnly = True # copy particles above a certain kin energy cut
MCTracksWithHitsOrEnergyCut = False # or of above, factor 2 file size increase compared to MCTracksWithEnergyCutOnly

checking4overlaps = False
if debug>1 : checking4overlaps = True

ROOT.gRandom.SetSeed(0)  # this should be propagated via ROOT to Pythia8 and Geant4VMC
shipRoot_conf.configure(0)     # load basic libraries, prepare atexit for python

ship_geo = ConfigRegistry.loadpy("$FAIRSHIP/geometry/shipLHC_geom_config.py")

outFile = "%s/ship.%s.root" % (options.outputDir, tag)
parFile="%s/ship.params.%s.root" % (options.outputDir, tag)

# In general, the following parts need not be touched
# ========================================================================

# -----Timer--------------------------------------------------------
timer = ROOT.TStopwatch()
timer.Start()
# ------------------------------------------------------------------------
# -----Create simulation run----------------------------------------
run = ROOT.FairRunSim()
run.SetName(mcEngine)  # Transport engine
run.SetOutputFile(outFile)  # Output file
run.SetUserConfig("g4Config.C") # user configuration file default g4Config.C 
rtdb = run.GetRuntimeDb() 

# -----Create geometry----------------------------------------------
import shipLHC_conf	
modules = shipLHC_conf.configure(run,ship_geo)

# -----Create PrimaryGenerator--------------------------------------
primGen = ROOT.FairPrimaryGenerator()

# Genie
if options.genie:
 ut.checkFileExists(inputFile)
 primGen.SetTarget(0., 0.) # do not interfere with GenieGenerator
 Geniegen = ROOT.GenieGenerator()
 Geniegen.Init(inputFile,options.firstEvent) 
 Geniegen.SetPositions(ship_geo.EmulsionDet.BZ, -ship_geo.EmulsionDet.BZ/2,0)
 primGen.AddGenerator(Geniegen)
 options.nEvents = min(options.nEvents,Geniegen.GetNevents())
 run.SetPythiaDecayer("DecayConfigNuAge.C")
 print('Generate ',options.nEvents,' with Genie input', ' first event',options.firstEvent)
 run.SetGenerator(primGen)

#---Store the visualiztion info of the tracks, this make the output file very large!!
#--- Use it only to display but not for production!
if options.eventDisplay: run.SetStoreTraj(ROOT.kTRUE)
else:            run.SetStoreTraj(ROOT.kFALSE)
# -----Initialize simulation run------------------------------------
#run.Init()
'''
if options.dryrun: # Early stop after setting up Pythia 8
 sys.exit(0)
gMC = ROOT.TVirtualMC.GetMC()
fStack = gMC.GetStack()
if MCTracksWithHitsOnly:
 fStack.SetMinPoints(1)
 fStack.SetEnergyCut(-100.*u.MeV)
elif MCTracksWithEnergyCutOnly:
 fStack.SetMinPoints(-1)
 fStack.SetEnergyCut(100.*u.MeV)
elif MCTracksWithHitsOrEnergyCut: 
 fStack.SetMinPoints(1)
 fStack.SetEnergyCut(100.*u.MeV)
elif options.deepCopy: 
 fStack.SetMinPoints(0)
 fStack.SetEnergyCut(0.*u.MeV)

if options.eventDisplay:
 # Set cuts for storing the trajectories, can only be done after initialization of run (?!)
  trajFilter = ROOT.FairTrajFilter.Instance()
  trajFilter.SetStepSizeCut(1*u.mm);  
  trajFilter.SetVertexCut(-20*u.m, -20*u.m,ship_geo.target.z0-1*u.m, 20*u.m, 20*u.m, 200.*u.m)
  trajFilter.SetMomentumCutP(0.1*u.GeV)
  trajFilter.SetEnergyCut(0., 400.*u.GeV)
  trajFilter.SetStorePrimaries(ROOT.kTRUE)
  trajFilter.SetStoreSecondaries(ROOT.kTRUE)
'''

# Print VMC fields and associated geometry objects
if debug > 0:
 geomGeant4.printVMCFields()
 geomGeant4.printWeightsandFields(onlyWithField = True,\
             exclude=['DecayVolume','Tr1','Tr2','Tr3','Tr4','Veto','Ecal','Hcal','MuonDetector','SplitCal'])
# Plot the field example
#fieldMaker.plotField(1, ROOT.TVector3(-9000.0, 6000.0, 50.0), ROOT.TVector3(-300.0, 300.0, 6.0), 'Bzx.png')
#fieldMaker.plotField(2, ROOT.TVector3(-9000.0, 6000.0, 50.0), ROOT.TVector3(-400.0, 400.0, 6.0), 'Bzy.png')

# -----Start run----------------------------------------------------
run.Run(options.nEvents)
# -----Runtime database---------------------------------------------
kParameterMerged = ROOT.kTRUE
parOut = ROOT.FairParRootFileIo(kParameterMerged)
parOut.open(parFile)
rtdb.setOutput(parOut)
rtdb.saveOutput()
rtdb.printParamContexts()
getattr(rtdb,"print")()
# ------------------------------------------------------------------------
print('Creating geometry file!')

run.CreateGeometryFile("%s/geofile_full.%s.root" % (options.outputDir, tag))
# save ShipGeo dictionary in geofile
import saveBasicParameters
saveBasicParameters.execute("%s/geofile_full.%s.root" % (options.outputDir, tag),ship_geo)

# checking for overlaps
if checking4overlaps:
 fGeo = ROOT.gGeoManager
 fGeo.SetNmeshPoints(10000)
 fGeo.CheckOverlaps(0.1)  # 1 micron takes 5minutes
 fGeo.PrintOverlaps()
 # check subsystems in more detail
 for x in fGeo.GetTopNode().GetNodes(): 
   x.CheckOverlaps(0.0001)
   fGeo.PrintOverlaps()

# -----Finish-------------------------------------------------------
timer.Stop()
rtime = timer.RealTime()
ctime = timer.CpuTime()
print(' ') 
print("Macro finished succesfully.") 

print("Output file is ",  outFile) 
print("Parameter file is ",parFile)
print("Real time ",rtime, " s, CPU time ",ctime,"s")
