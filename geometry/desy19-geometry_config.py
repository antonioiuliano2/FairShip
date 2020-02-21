import shipunit as u
from ShipGeoConfig import AttrDict, ConfigRegistry
# the following params should be passed through 'ConfigRegistry.loadpy' method
# none for the moment
with ConfigRegistry.register_config("basic") as c:
  
    c.target = AttrDict(z0=0*u.cm)
       
    
    #EmuTarget (Brick!)
    c.EmuTarget = AttrDict(z=0*u.cm)
    c.EmuTarget.zEmuTarget = 106.66 * u.cm
    c.EmuTarget.EmTh = 0.0070 * u.cm
    c.EmuTarget.EmX = 12.5 * u.cm
    c.EmuTarget.EmY = 9.9 * u.cm
    c.EmuTarget.PBTh = 0.0175 * u.cm
    c.EmuTarget.PasSlabTh = 0.1 * u.cm #passive slab in ECC (lead for July 2018 measurement, molybdenum/tungsten for SHiP target replica)
    c.EmuTarget.EPlW = 2* c.EmuTarget.EmTh + c.EmuTarget.PBTh
    c.EmuTarget.AllPW = c.EmuTarget.PasSlabTh + c.EmuTarget.EPlW
    c.EmuTarget.BrX = 12.9 *u.cm
    c.EmuTarget.BrY = 10.5 *u.cm
    c.EmuTarget.BrPackZ = 0.1 * u.cm
    c.EmuTarget.BrPackX = c.EmuTarget.BrX - c.EmuTarget.EmX
    c.EmuTarget.BrPackY = c.EmuTarget.BrY - c.EmuTarget.EmY
    c.EmuTarget.BrZ = 54 * c.EmuTarget.AllPW + c.EmuTarget.EPlW +c.EmuTarget.BrPackZ
    c.EmuTarget.TX = c.EmuTarget.EmX
    c.EmuTarget.TY = c.EmuTarget.EmY
    c.EmuTarget.TZ = 5.7 *u.cm #not used for July testbeam geometry





