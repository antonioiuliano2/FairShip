import os,ROOT,shipVertex,charmDet_conf
if realPR == "Prev": import shipPatRec_prev as shipPatRec # The previous version of the pattern recognition
else: import shipPatRec
import shipunit as u
import rootUtils as ut
from array import array
import sys 
stop  = ROOT.TVector3()
start = ROOT.TVector3()

class ShipDigiReco:
 " convert FairSHiP MC hits / digitized hits to measurements"
 def __init__(self,fout,fgeo):
  self.fn = ROOT.TFile.Open(fout,'update')
  self.sTree     = self.fn.cbmsim
  if self.sTree.GetBranch("FitTracks"):
    print "remove RECO branches and rerun reconstruction"
    self.fn.Close()    
    # make a new file without reco branches
    f = ROOT.TFile(fout)
    sTree = f.cbmsim
    #if sTree.GetBranch("FitTracks"): sTree.SetBranchStatus("FitTracks",0)
    #if sTree.GetBranch("goodTracks"): sTree.SetBranchStatus("goodTracks",0)
    #if sTree.GetBranch("Particles"): sTree.SetBranchStatus("Particles",0)
    #if sTree.GetBranch("fitTrack2MC"): sTree.SetBranchStatus("fitTrack2MC",0)      
    if sTree.GetBranch("Digi_MuonTaggerHits"): sTree.SetBranchStatus("Digi_MuonTaggerHits",0)
    rawFile = fout.replace("_rec.root","_raw.root")
    recf = ROOT.TFile(rawFile,"recreate")
    newTree = sTree.CloneTree(0)
    for n in range(sTree.GetEntries()):
      sTree.GetEntry(n)
      rc = newTree.Fill()
    sTree.Clear()
    newTree.AutoSave()
    f.Close() 
    recf.Close() 
    os.system('cp '+rawFile +' '+fout)
    self.fn = ROOT.TFile(fout,'update')
    self.sTree     = self.fn.cbmsim     
#  check that all containers are present, otherwise create dummy version
  self.dummyContainers={}
  branch_class = {"BoxPoint":"BoxPoint","SpectrometerPoint":"SpectrometerPoint",\
                 "MufluxSpectrometerPoint":"MufluxSpectrometerPoint","MuonTaggerPoint":"MuonTaggerPoint"}
  for x in branch_class:
    if not self.sTree.GetBranch(x):
     self.dummyContainers[x+"_array"] = ROOT.TClonesArray(branch_class[x])
     self.dummyContainers[x] = self.sTree.Branch(x,self.dummyContainers[x+"_array"],32000,-1) 
     setattr(self.sTree,x,self.dummyContainers[x+"_array"])
     self.dummyContainers[x].Fill()
#   
  if self.sTree.GetBranch("GeoTracks"): self.sTree.SetBranchStatus("GeoTracks",0)
# prepare for output
# event header
  self.header  = ROOT.FairEventHeader()
  self.eventHeader  = self.sTree.Branch("ShipEventHeader",self.header,32000,-1)
# fitted tracks
  #self.fGenFitArray = ROOT.TClonesArray("genfit::Track") 
  #self.fGenFitArray.BypassStreamer(ROOT.kFALSE)
  #self.fitTrack2MC  = ROOT.std.vector('int')()
  #self.goodTracksVect  = ROOT.std.vector('int')()
  #self.mcLink      = self.sTree.Branch("fitTrack2MC",self.fitTrack2MC,32000,-1)
  #self.fitTracks   = self.sTree.Branch("FitTracks",  self.fGenFitArray,32000,-1)
  #self.goodTracksBranch      = self.sTree.Branch("goodTracks",self.goodTracksVect,32000,-1)
  #self.fTrackletsArray = ROOT.TClonesArray("Tracklet") 
  #self.Tracklets   = self.sTree.Branch("Tracklets",  self.fTrackletsArray,32000,-1)
  self.digiSciFi = ROOT.TClonesArray("SciFiHit")
  self.digiSciFiBranch = self.sTree.Branch("Digi_SciFiHits",self.digiSciFi,32000,-1)
  self.digiPIXEL = ROOT.TClonesArray("PIXELHit")
  self.digiPIXELBranch = self.sTree.Branch("Digi_PIXELHits",self.digiPIXEL,32000,-1)
  self.digiMufluxSpectro    = ROOT.TClonesArray("MufluxSpectrometerHit")
  self.digiMufluxSpectroBranch=self.sTree.Branch("Digi_MufluxSpectrometerHits",self.digiMufluxSpectro,32000,-1)
  self.digiMuon    = ROOT.TClonesArray("MuonTaggerHit")
  self.digiMuonBranch=self.sTree.Branch("Digi_MuonTaggerHits",self.digiMuon,32000,-1)
# for the digitizing ste
# prepare vertexing
  #self.Vertexing = shipVertex.Task(h,self.sTree)
# setup random number generator 
  self.random = ROOT.TRandom()
  ROOT.gRandom.SetSeed(13)
  self.PDG = ROOT.TDatabasePDG.Instance()
# access ShipTree
#
  self.geoMat =  ROOT.genfit.TGeoMaterialInterface()
# init geometry and mag. field
  gMan  = ROOT.gGeoManager
#
  #self.bfield = ROOT.genfit.FairShipFields()
  #self.fM = ROOT.genfit.FieldManager.getInstance()
  #self.fM.init(self.bfield)
  ROOT.genfit.MaterialEffects.getInstance().init(self.geoMat)

 # init fitter, to be done before importing shipPatRec
  #fitter          = ROOT.genfit.KalmanFitter()
  #fitter          = ROOT.genfit.KalmanFitterRefTrack()
  #self.fitter      = ROOT.genfit.DAF()
  #self.fitter.setMaxIterations(50)
 # if debug: self.fitter.setDebugLvl(1) # produces lot of printout
  #set to True if "real" pattern recognition is required also
 # if debug == True: shipPatRec.debug = 1

# for 'real' PatRec
 # shipPatRec.initialize(fgeo)

 def reconstruct(self):
  print 'Reconstruction to be implemented here'

 def digitize(self):
  print 'Starting Digitizazion'
  self.sTree.t0 = self.random.Rndm()*1*u.microsecond
  self.header.SetEventTime( self.sTree.t0 )
  self.header.SetRunId( self.sTree.MCEventHeader.GetRunID() )
  self.header.SetMCEntryNumber( self.sTree.MCEventHeader.GetEventID() )  # counts from 1
  self.eventHeader.Fill()
  
  self.digiPIXEL.Delete()
  self.digiSciFi.Delete()
  self.digitizePIXELandSciFi()
  self.digiPIXELBranch.Fill()
  self.digiSciFiBranch.Fill()
  
  self.digiMuon.Delete()
  self.digitizeMuon()
  self.digiMuonBranch.Fill()
  self.digiMufluxSpectro.Delete()
  self.digitizeMufluxSpectro()
  self.digiMufluxSpectroBranch.Fill()

 def digitizePIXELandSciFi(self):
   indexPIXEL = 0
   indexSciFi = 0
   hitsPerDetIdPIXEL = {}
   hitsPerDetIdSciFi = {}
   for aMCPoint in self.sTree.SpectrometerPoint:
    if aMCPoint.GetDetectorID() > 100:
     aHit = ROOT.PIXELHit(aMCPoint,self.sTree.t0)
     if self.digiPIXEL.GetSize() == indexPIXEL: self.digiPIXEL.Expand(indexPIXEL+1000)
     self.digiPIXEL[indexPIXEL]=aHit
     detID = aHit.GetDetectorID()
     if aHit.isValid():
      if hitsPerDetIdPIXEL.has_key(detID):
       if self.digiPIXEL[hitsPerDetIdPIXEL[detID]].GetDigi() > aHit.GetDigi():
 # second hit with smaller tdc
        self.digiPIXEL[hitsPerDetIdPIXEL[detID]].setValidity(0)
        hitsPerDetIdPIXEL[detID] = indexPIXEL
     indexPIXEL+=1
    else:
     aHit = ROOT.SciFiHit(aMCPoint,self.sTree.t0)
     if self.digiSciFi.GetSize() == indexSciFi: self.digiSciFi.Expand(indexSciFi+1000)
     self.digiSciFi[indexSciFi]=aHit
     detID = aHit.GetDetectorID()
     if aHit.isValid():
      if hitsPerDetIdSciFi.has_key(detID):
       if self.digiSciFi[hitsPerDetIdSciFi[detID]].GetDigi() > aHit.GetDigi():
 # second hit with smaller tdc
        self.digiSciFi[hitsPerDetIdSciFi[detID]].setValidity(0)
        hitsPerDetIdSciFi[detID] = indexSciFi
     indexSciFi+=1      

 def digitizeMuon(self):
   index = 0
   hitsPerDetId = {}
   for aMCPoint in self.sTree.MuonTaggerPoint:
     aHit = ROOT.MuonTaggerHit(aMCPoint,self.sTree.t0)
     if self.digiMuon.GetSize() == index: self.digiMuon.Expand(index+1000)
     self.digiMuon[index]=aHit
     detID = aHit.GetDetectorID()
     if aHit.isValid():
      if hitsPerDetId.has_key(detID):
       if self.digiMuon[hitsPerDetId[detID]].GetDigi() > aHit.GetDigi():
 # second hit with smaller tdc
        self.digiMuon[hitsPerDetId[detID]].setValidity(0)
        hitsPerDetId[detID] = index
     index+=1

 def digitizeMufluxSpectro(self):
   index = 0
   hitsPerDetId = {}
   for aMCPoint in self.sTree.MufluxSpectrometerPoint:
     aHit = ROOT.MufluxSpectrometerHit(aMCPoint,self.sTree.t0)
     if self.digiMufluxSpectro.GetSize() == index: self.digiMufluxSpectro.Expand(index+1000)
     self.digiMufluxSpectro[index]=aHit
     detID = aHit.GetDetectorID()
     if aHit.isValid():
      if hitsPerDetId.has_key(detID):
       if self.digiMufluxSpectro[hitsPerDetId[detID]].GetDigi() > aHit.GetDigi():
 # second hit with smaller tdc
        self.digiMufluxSpectro[hitsPerDetId[detID]].setValidity(0)
        hitsPerDetId[detID] = index
     index+=1

 def finish(self):
  #del self.fitter
  print 'finished writing tree'
  self.sTree.Write()
  ut.errorSummary()
  ut.writeHists(h,"recohists.root")
 # if realPR: shipPatRec.finalize()
