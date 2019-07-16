import os
import ROOT
import shipunit as u

emuefficiency = 0.9

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
        self.digiEmu = ROOT.TClonesArray("EmuBaseTrk")
        self.digiEmuBranch = self.sTree.Branch("EmuBaseTrks",self.digiEmu,32000,1)
        # setup random number generator
        ROOT.gRandom.SetSeed()
        self.PDG = ROOT.TDatabasePDG.Instance()
        # for the digitizing and reconstruction step

        self.gMan  = ROOT.gGeoManager

    def digitize(self):

        #self.sTree.t0 = ROOT.gRandom.Rndm()*1*u.microsecond
        self.sTree.t0 = grandom.Uniform()*4.8
        self.header.SetEventTime( self.sTree.t0 )
        self.header.SetRunId( self.sTree.MCEventHeader.GetRunID() )
        self.header.SetMCEntryNumber( self.sTree.MCEventHeader.GetEventID() )  # counts from 1
        self.eventHeader.Fill()
        self.digiEmu.Delete()
        self.digitizeEmulsion()
        self.digiEmuBranch.Fill()

    def digitizeEmulsion(self):
        index = 0
        #casual generator for estimating detector resolution
        energycut = 0.1 #energy cut to form a base track, 100 MeV
        angres = 0.003 #angular resolution assumed to be 3 mrads
        targetmoverspeed = 2.6 
        #pottime = grandom.Uniform()*4.8 #must go from 0 to 12.5 cm    
        pottime = self.sTree.t0   
        #retrieving hits in emulsion
        for emupoint in self.sTree.BoxPoint:
            basetrack = ROOT.EmuBaseTrk(emupoint.GetDetectorID(),self.sTree.t0)
            # effect of the angular resolution
            tx = emupoint.GetPx()/emupoint.GetPz()
            ty = emupoint.GetPy()/emupoint.GetPz()
            tx = tx + grandom.Gaus(0.,angres)
            ty = ty + grandom.Gaus(0.,angres)
            tantheta = pow(pow(tx,2) + pow(ty,2),0.5)
            # effect of the target mover along x
            x = emupoint.GetX() -12.5/2. + pottime * targetmoverspeed
            y = emupoint.GetY()
            basetrack.SetX(x)
            basetrack.SetY(y)
            basetrack.SetTX(tx)
            basetrack.SetTY(ty)

            nfilmhit = emupoint.GetDetectorID() #
            pdgcode = emupoint.PdgCode()
            pdgparticle = self.PDG.GetParticle(pdgcode)

            basetrack.SetNFilm(nfilmhit)
            basetrack.SetMCTrackID(emupoint.GetTrackID())   
            
            basetrack.setValid()

            if (pdgparticle):
             charge = pdgparticle.Charge()
	    else:
             charge = 0.

	    if (charge == 0.): #only charged particles are considered valid
             basetrack.setInvalid()            

            if (tantheta > 1.): #out of scanning regime
             basetrack.setInvalid()          
            
            prob = grandom.Uniform()

            if (prob > emuefficiency):
             basetrack.setInvalid()

            #filling in the tclonesarray
            if index>0 and self.digiEmu.GetSize() == index: self.digiEmu.Expand(index+1000)
            if nfilmhit < 100 :
             self.digiEmu[index] = basetrack #only one of two emulsions volume per film
             index = index + 1
    def finish(self):
        print 'finished writing tree'
        self.sTree.Write()
