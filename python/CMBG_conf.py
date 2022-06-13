from __future__ import division
import ROOT, os
import shipunit as u

def configure(CMBG, ship_geo):
    #handles external variables for the Cosmic Muon Backgorund Generator within FairShip


    #target size (before rotation)
    DxTarget = 12.5
    DyTarget = 10.0
    DzTarget = 0.43
    TargetSize = ROOT.TVector3(DxTarget, DyTarget, DzTarget);
    
    rotationanglex = 90. #degrees
    TargetSize.RotateX(rotationanglex*ROOT.TMath.DegToRad()); #TVector3 wants radians

    # CMBG production area
    CMBG.xdist = ROOT.TMath.Abs(TargetSize.X()) # production area size [cm]
    CMBG.zdist = ROOT.TMath.Abs(TargetSize.Z()) # production area size [cm]
    CMBG.z0 = -TargetSize.Z()/2.  # middle production area
    # DetectorBox
    CMBG.yBox = ROOT.TMath.Abs(TargetSize.Y()) # box top layer [cm]
    CMBG.xBox = ROOT.TMath.Abs(TargetSize.X()) # box side layer [cm]
    CMBG.zBox = ROOT.TMath.Abs(TargetSize.Z()) # box length [cm]
    zmiddle = -TargetSize.Z()/2.; #middle box
    
    #setup
    CMBG.n_EVENTS = 400000; # #simulated events per "spill"
    CMBG.minE = 1.0 #low energy limit for the Low-energy simulation [GeV]
