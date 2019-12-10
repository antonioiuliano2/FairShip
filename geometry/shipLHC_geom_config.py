import ROOT as r
import shipunit as u
from ShipGeoConfig import AttrDict, ConfigRegistry


with ConfigRegistry.register_config("basic") as c:
# cave parameters
	c.cave = AttrDict(z=0*u.cm)

	c.EmulsionDet = AttrDict(z=0*u.cm)
	c.EmulsionDet.zC = 0*u.m
	c.EmulsionDet.row=3
        c.EmulsionDet.col=4
        c.EmulsionDet.wall=4
	c.EmulsionDet.target = 1  #number of neutrino target volumes
	c.EmulsionDet.n_plates = 56
	c.EmulsionDet.EmTh = 0.0070 * u.cm
	c.EmulsionDet.EmX = 9.9 * u.cm
	c.EmulsionDet.EmY = 12.5 * u.cm
	c.EmulsionDet.PBTh = 0.0175 * u.cm
	c.EmulsionDet.LeadTh = 0.1 * u.cm
	c.EmulsionDet.EPlW = 2* c.EmulsionDet.EmTh + c.EmulsionDet.PBTh
	c.EmulsionDet.AllPW = c.EmulsionDet.LeadTh + c.EmulsionDet.EPlW
	c.EmulsionDet.BrX = 10.4 * u.cm
	c.EmulsionDet.BrY = 12.9 * u.cm
	c.EmulsionDet.BrPackZ = 0.1045 * u.cm
	c.EmulsionDet.BrPackX = c.EmulsionDet.BrX - c.EmulsionDet.EmX
	c.EmulsionDet.BrPackY = c.EmulsionDet.BrY - c.EmulsionDet.EmY
	c.EmulsionDet.BrZ = c.EmulsionDet.n_plates * c.EmulsionDet.AllPW + c.EmulsionDet.EPlW + c.EmulsionDet.BrPackZ
	c.EmulsionDet.xdim = c.EmulsionDet.col*c.EmulsionDet.BrX
	c.EmulsionDet.ydim = c.EmulsionDet.row*c.EmulsionDet.BrY
	c.EmulsionDet.WallXDim = c.EmulsionDet.col*c.EmulsionDet.BrX
	c.EmulsionDet.WallYDim = c.EmulsionDet.row*c.EmulsionDet.BrY
	c.EmulsionDet.WallZDim = c.EmulsionDet.BrZ
	c.EmulsionDet.TTz = 3.0*u.cm
        c.EmulsionDet.zdim = c.EmulsionDet.wall* c.EmulsionDet.WallZDim + (c.EmulsionDet.wall+1)*c.EmulsionDet.TTz
	c.EmulsionDet.ShiftX = 28.0*u.cm
	c.EmulsionDet.ShiftY = 13.95*u.cm

	c.Scifi = AttrDict(z=0*u.cm)
	c.Scifi.xdim = c.EmulsionDet.xdim
	c.Scifi.ydim = c.EmulsionDet.ydim
	c.Scifi.zdim = c.EmulsionDet.TTz
	c.Scifi.DZ = c.EmulsionDet.BrZ
	c.Scifi.nplanes = c.EmulsionDet.wall+1

	c.MuFilter = AttrDict(z=0*u.cm)
	c.MuFilter.ShiftDY = 2.0*u.cm
	c.MuFilter.ShiftDYTot = 6.0*u.cm
	c.MuFilter.X = c.EmulsionDet.xdim + 20*u.cm
        #c.MuFilter.Y = c.EmulsionDet.ydim + 20*u.cm+10.0*u.cm
        c.MuFilter.Y = 60.0*u.cm+c.MuFilter.ShiftDYTot
        c.MuFilter.FeX = c.MuFilter.X
        #c.MuFilter.FeY = c.EmulsionDet.ydim + 20*u.cm
        c.MuFilter.FeY = 60.0*u.cm
        c.MuFilter.FeZ = 20*u.cm
        c.MuFilter.TDetX = c.MuFilter.X
        c.MuFilter.TDetY = c.MuFilter.FeY
        c.MuFilter.TDetZ = 2*u.cm
        c.MuFilter.nplanes = 8
	c.MuFilter.Z = c.MuFilter.nplanes*(c.MuFilter.FeZ+c.MuFilter.TDetZ)
	c.MuFilter.Zcenter = c.EmulsionDet.zC+c.EmulsionDet.zdim/2+c.MuFilter.Z/2
	c.MuFilter.ShiftX = c.EmulsionDet.ShiftX
	c.MuFilter.ShiftY = 3.3*u.cm
