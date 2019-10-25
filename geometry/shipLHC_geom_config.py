import ROOT as r
import shipunit as u
from ShipGeoConfig import AttrDict, ConfigRegistry


with ConfigRegistry.register_config("basic") as c:
# cave parameters
	c.cave = AttrDict(z=0*u.cm)

	c.EmulsionDet = AttrDict(z=0*u.cm)
	c.EmulsionDet.zC = 0*u.m
	c.EmulsionDet.row=4
        c.EmulsionDet.col=4
        c.EmulsionDet.wall=12
	c.EmulsionDet.target = 1  #number of neutrino target volumes
	c.EmulsionDet.n_plates = 56
	c.EmulsionDet.Ydist = 0.2*u.cm
	c.EmulsionDet.EmTh = 0.0070 * u.cm
	c.EmulsionDet.EmX = 12.5 * u.cm
	c.EmulsionDet.EmY = 9.9 * u.cm
	c.EmulsionDet.PBTh = 0.0175 * u.cm
	c.EmulsionDet.LeadTh = 0.1 * u.cm
	c.EmulsionDet.EPlW = 2* c.EmulsionDet.EmTh + c.EmulsionDet.PBTh
	c.EmulsionDet.AllPW = c.EmulsionDet.LeadTh + c.EmulsionDet.EPlW
	c.EmulsionDet.BrX = 12.9 * u.cm
	c.EmulsionDet.BrY = 10.5 * u.cm
	c.EmulsionDet.BrPackZ = 0.1045 * u.cm
	c.EmulsionDet.BrPackX = c.EmulsionDet.BrX - c.EmulsionDet.EmX
	c.EmulsionDet.BrPackY = c.EmulsionDet.BrY - c.EmulsionDet.EmY
	c.EmulsionDet.BrZ = c.EmulsionDet.n_plates * c.EmulsionDet.AllPW + c.EmulsionDet.EPlW + c.EmulsionDet.BrPackZ
	c.EmulsionDet.xdim = c.EmulsionDet.col*c.EmulsionDet.BrX
	c.EmulsionDet.ydim = c.EmulsionDet.row*c.EmulsionDet.BrY
	c.EmulsionDet.WallXDim = c.EmulsionDet.col*c.EmulsionDet.BrX
	c.EmulsionDet.WallYDim = c.EmulsionDet.row*c.EmulsionDet.BrY+(c.EmulsionDet.row-1)*c.EmulsionDet.Ydist
	c.EmulsionDet.WallZDim = c.EmulsionDet.BrZ
	c.EmulsionDet.TTz = 2.5*u.cm
        c.EmulsionDet.zdim = c.EmulsionDet.wall* c.EmulsionDet.WallZDim + (c.EmulsionDet.wall+1)*c.EmulsionDet.TTz
	c.EmulsionDet.ShiftX = -c.EmulsionDet.xdim/2
	c.EmulsionDet.ShiftY = 24*u.cm

	c.Scifi = AttrDict(z=0*u.cm)
	c.Scifi.xdim = c.EmulsionDet.xdim
	c.Scifi.ydim = c.EmulsionDet.ydim
	c.Scifi.zdim = c.EmulsionDet.TTz
	c.Scifi.DZ = c.EmulsionDet.BrZ
	c.Scifi.nplanes = c.EmulsionDet.wall+1

	c.MuFilter = AttrDict(z=0*u.cm)
	c.MuFilter.X = c.EmulsionDet.xdim + 20*u.cm
        c.MuFilter.Y = c.EmulsionDet.ydim + 20*u.cm
        c.MuFilter.FeX = c.MuFilter.X
        c.MuFilter.FeY = c.MuFilter.Y
        c.MuFilter.FeZ = 20*u.cm
        c.MuFilter.TDetX = c.MuFilter.X
        c.MuFilter.TDetY = c.MuFilter.Y
        c.MuFilter.TDetZ = 2*u.cm
        c.MuFilter.nplanes = 4
	c.MuFilter.Z = c.MuFilter.nplanes*(c.MuFilter.FeZ+c.MuFilter.TDetZ)
	c.MuFilter.Zcenter = c.EmulsionDet.zC+c.EmulsionDet.zdim/2+c.MuFilter.Z/2
	c.MuFilter.ShiftX = c.EmulsionDet.ShiftX
	c.MuFilter.ShiftY = c.EmulsionDet.ShiftY -10*u.cm
