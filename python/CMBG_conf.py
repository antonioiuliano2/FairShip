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
    
    CMBG.z0 = -TargetSize.Z()/2.  # center of detector, to be set before rotation
    
    TargetSize.RotateX(ship_geo.EmuTarget.TargetXRotation*ROOT.TMath.DegToRad()); #TVector3 wants radians

    # CMBG production area
    CMBG.xdist = ROOT.TMath.Abs(TargetSize.X()) # production area size [cm]
    CMBG.zdist = ROOT.TMath.Abs(TargetSize.Z()) # production area size [cm]
    # DetectorBox
    CMBG.yBox = ROOT.TMath.Abs(TargetSize.Y()) # box top layer [cm]
    CMBG.xBox = ROOT.TMath.Abs(TargetSize.X()) # box side layer [cm]
    CMBG.zBox = ROOT.TMath.Abs(TargetSize.Z()) # box length [cm]
    
    #setup
    CMBG.n_EVENTS = 323698; #horizontal configuration: 149.86m-2s-1 *1e-4 * 12.5cm2 * 10.0cm2 * 48h * 3600
    CMBG.minE = 0.5 #low energy limit for the Low-energy simulation [GeV]
