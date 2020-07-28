#!/usr/bin/env python
# -*- coding: latin-1 -*-
import ROOT,os
import shipunit as u
from ShipGeoConfig import ConfigRegistry
detectorList = []

def getParameter(x,ship_geo,latestCharmGeo):
  lv = x.split('.')
  last = lv[len(lv)-1]
  top = ''
  for l in range(len(lv)-1): 
    top += lv[l]
    if l<len(lv)-2: top +='.' 
  a = getattr(ship_geo,top)
  if hasattr(a,last): return getattr(a,last)
# not in the list of recorded parameteres. probably added after
# creation of file. Check newest geometry_config:
  a = getattr(latestCharmGeo,top)
  return getattr(a,last)

def configure(run,ship_geo,Gfield=''):
 latestCharmGeo = ConfigRegistry.loadpy("$FAIRSHIP/geometry/desy19-geometry_config.py")
# -----Create media-------------------------------------------------
 run.SetMaterials("media.geo")  # Materials
 
# -----Create geometry----------------------------------------------
 cave= ROOT.ShipCave("CAVE")
 cave.SetGeometryFileName("caveWithAir.geo")
 detectorList.append(cave)    
# === Emulsion Target 
 EmuTarget = ROOT.EmuDESYTarget("EmuDESYTarget",ship_geo.EmuTarget.zEmuTarget,ROOT.kTRUE)
 EmuTarget.SetEmulsionParam(ship_geo.EmuTarget.EmTh, ship_geo.EmuTarget.EmX, ship_geo.EmuTarget.EmY, ship_geo.EmuTarget.PBTh,ship_geo.EmuTarget.EPlW, ship_geo.EmuTarget.PasSlabTh, ship_geo.EmuTarget.AllPW)
 
 EmuTarget.SetNPlates(ship_geo.EmuTarget.NPlates[ship_geo.EmuTarget.cRun-1],ship_geo.EmuTarget.NPlates_second)
 EmuTarget.SetNRun(ship_geo.EmuTarget.cRun)

 EmuTarget.SetECCDistance(ship_geo.EmuTarget.ECCdistance)
 EmuTarget.SetPassiveDZ(ship_geo.EmuTarget.PassiveDZ)
 
 EmuTarget.SetBrickParam(ship_geo.EmuTarget.BrX, ship_geo.EmuTarget.BrY, ship_geo.EmuTarget.BrZ, ship_geo.EmuTarget.BrPackX, ship_geo.EmuTarget.BrPackY, ship_geo.EmuTarget.BrPackZ)
 EmuTarget.SetTargetParam(ship_geo.EmuTarget.TX, ship_geo.EmuTarget.TY, ship_geo.EmuTarget.TZ)
 detectorList.append(EmuTarget)
  
# === SciFi modules
 SciFi = ROOT.SciFiDESY("SciFiDESY",ROOT.kTRUE)
 SciFi.SetBoxDimensions(ship_geo.SciFi.DX,ship_geo.SciFi.DY,ship_geo.SciFi.DZ)
 SciFi.SetStationDimensions(ship_geo.SciFi.StatDX,ship_geo.SciFi.StatDY,ship_geo.SciFi.StatDZ)
 SciFi.SetStationPositions(ship_geo.SciFi.zpos1,ship_geo.SciFi.zpos2)
 detectorList.append(SciFi)

 #detectorList.append(MuonTagger)
 for x in detectorList:
  run.AddModule(x)
# return list of detector elements
 detElements = {}
 for x in run.GetListOfModules(): detElements[x.GetName()]=x 
 
 return detElements
