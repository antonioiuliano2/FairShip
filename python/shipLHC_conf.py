#!/usr/bin/env python
# -*- coding: latin-1 -*-

import ROOT,os
import shipunit as u
from ShipGeoConfig import ConfigRegistry
detectorList = []

def getParameter(x,ship_geo,latestGeo):
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
  a = getattr(latestGeo,top)
  return getattr(a,last)

def configure(run,ship_geo,Gfield=''):
 #latestCharmGeo = ConfigRegistry.loadpy("$FAIRSHIP/geometry/charm-geometry_config.py")
 latestGeo = ConfigRegistry.loadpy("$FAIRSHIP/geometry/shipLHC_geom_config.py")
# -----Create media-------------------------------------------------
 run.SetMaterials("media.geo")  # Materials
 
# -----Create geometry----------------------------------------------
 cave= ROOT.ShipCave("CAVE")
 cave.SetGeometryFileName("cave.geo")
 detectorList.append(cave)

 floor = ROOT.Floor("FLOOR")
 detectorList.append(floor)
 
 EmulsionDet = ROOT.EmulsionDet("EmulsionDet",ROOT.kTRUE)
 EmulsionDet.SetCenterZ(ship_geo.EmulsionDet.zC)
 EmulsionDet.SetNumberTargets(ship_geo.EmulsionDet.target)
 EmulsionDet.SetNumberBricks(ship_geo.EmulsionDet.col,ship_geo.EmulsionDet.row,ship_geo.EmulsionDet.wall)
 EmulsionDet.SetDetectorDimension(ship_geo.EmulsionDet.xdim, ship_geo.EmulsionDet.ydim, ship_geo.EmulsionDet.zdim)
 EmulsionDet.SetTargetWallDimension(ship_geo.EmulsionDet.WallXDim, ship_geo.EmulsionDet.WallYDim, ship_geo.EmulsionDet.WallZDim)
 EmulsionDet.SetEmulsionParam(ship_geo.EmulsionDet.EmTh, ship_geo.EmulsionDet.EmX, ship_geo.EmulsionDet.EmY, ship_geo.EmulsionDet.PBTh,ship_geo.EmulsionDet.EPlW, ship_geo.EmulsionDet.LeadTh, ship_geo.EmulsionDet.AllPW)
 EmulsionDet.SetBrickParam(ship_geo.EmulsionDet.BrX, ship_geo.EmulsionDet.BrY, ship_geo.EmulsionDet.BrZ, ship_geo.EmulsionDet.BrPackX, ship_geo.EmulsionDet.BrPackY, ship_geo.EmulsionDet.BrPackZ, ship_geo.EmulsionDet.n_plates)
 EmulsionDet.SetTTzdimension(ship_geo.EmulsionDet.TTz)
 EmulsionDet.SetDisplacement(ship_geo.EmulsionDet.ShiftX, ship_geo.EmulsionDet.ShiftY)
 detectorList.append(EmulsionDet)

 Scifi = ROOT.Scifi("Scifi",ship_geo.Scifi.xdim,ship_geo.Scifi.ydim,ship_geo.Scifi.zdim,ROOT.kTRUE)
 Scifi.SetTotZDimension(ship_geo.EmulsionDet.zdim)
 Scifi.SetDetectorDimension(ship_geo.Scifi.xdim,ship_geo.Scifi.ydim,ship_geo.Scifi.zdim) 
 Scifi.SetScifiNplanes(ship_geo.Scifi.nplanes)
 Scifi.SetGapBrick(ship_geo.Scifi.DZ)
 detectorList.append(Scifi)

 MuFilter = ROOT.MuFilter("MuFilter",ROOT.kTRUE)
 MuFilter.SetMuFilterDimensions(ship_geo.MuFilter.X, ship_geo.MuFilter.Y, ship_geo.MuFilter.Z)
 MuFilter.SetIronBlockDimensions(ship_geo.MuFilter.FeX, ship_geo.MuFilter.FeY, ship_geo.MuFilter.FeZ)
 MuFilter.SetTimingPlanesDimensions(ship_geo.MuFilter.TDetX, ship_geo.MuFilter.TDetY, ship_geo.MuFilter.TDetZ)
 MuFilter.SetNplanes(ship_geo.MuFilter.nplanes)
 MuFilter.SetCenterZ(ship_geo.MuFilter.Zcenter)
 MuFilter.SetXYDisplacement(ship_geo.MuFilter.ShiftX, ship_geo.MuFilter.ShiftY)
 MuFilter.SetYPlanesDisplacement(ship_geo.MuFilter.ShiftDY)
 detectorList.append(MuFilter)

 for x in detectorList:
   run.AddModule(x)
# return list of detector elements
 detElements = {}
 for x in run.GetListOfModules(): detElements[x.GetName()]=x
 return detElements
