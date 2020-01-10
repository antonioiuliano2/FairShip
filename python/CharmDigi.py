import os
import ROOT
import shipunit as u

grandom = ROOT.TRandom3()

class CharmDigi:
    " convert FairSHiP MC hits / digitized hits to measurements"
    def __init__(self,fout):

        self.iEvent = 0

        outdir=os.getcwd()
        outfile=outdir+"/"+fout
        self.fn = ROOT.TFile(fout,'update')
        self.sTree = self.fn.cbmsim

        # event header
        self.header  = ROOT.FairEventHeader()
        self.eventHeader  = self.sTree.Branch("ShipEventHeader",self.header,32000,1)
        self.digiEmu = ROOT.TClonesArray("BoxPoint")
        self.digiEmuBranch = self.sTree.Branch("EmuBaseTrks",self.digiEmu,32000,1)
        # setup random number generator
        ROOT.gRandom.SetSeed()
        self.PDG = ROOT.TDatabasePDG.Instance()
        # for the digitizing and reconstruction step

        self.gMan  = ROOT.gGeoManager

    def SetSpill(self,nspill):
        self.nspill = nspill

    def digitize(self):

        #self.sTree.t0 = ROOT.gRandom.Rndm()*1*u.microsecond         
        pottime = grandom.Uniform()*4.8     
        self.sTree.t0 = self.nspill*100 + pottime
        self.header.SetEventTime( self.sTree.t0 )
        self.header.SetRunId( self.sTree.MCEventHeader.GetRunID() )
        #self.header.SetMCEntryNumber( self.sTree.MCEventHeader.GetEventID() )  # counts from 1
        self.header.SetMCEntryNumber( self.iEvent )  # counts from 0
        self.eventHeader.Fill()
        self.digiEmu.Delete()
        self.nspill = self.iEvent/2000 #set spill according to number of event
        self.digitizeEmulsion(self.nspill, pottime)
        self.digiEmuBranch.Fill()

    def digitizeEmulsion(self,nspill,pottime):
        index = 0
        spilldy = 2.0
        #casual generator for estimating detector resolution
        targetmoverspeed = 2.6 
         
        #retrieving hits in emulsion
        for emupoint in self.sTree.BoxPoint:
            # effect of the target mover along x
            x = emupoint.GetX() -12.5/2. + pottime * targetmoverspeed
            y = emupoint.GetY() - 9.9/2. + nspill * spilldy
            mom = ROOT.TVector3(emupoint.GetPx(), emupoint.GetPy(), emupoint.GetPz())
            pos = ROOT.TVector3(x,y,emupoint.GetZ())

            basetrack = ROOT.BoxPoint(emupoint.GetTrackID(),emupoint.GetDetectorID(),pos, mom,emupoint.GetTime()+self.sTree.t0,emupoint.GetLength(),emupoint.GetEnergyLoss(),emupoint.PdgCode())            

            #filling in the tclonesarray
            if index>0 and self.digiEmu.GetSize() == index: self.digiEmu.Expand(index+1000)
            self.digiEmu[index] = basetrack
            index = index + 1
    def finish(self):
        print 'finished writing tree'
        self.sTree.Write()
