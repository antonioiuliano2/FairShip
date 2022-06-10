import shipunit as u
from ShipGeoConfig import AttrDict, ConfigRegistry
# the following params should be passed through 'ConfigRegistry.loadpy' method
# none for the moment
with ConfigRegistry.register_config("basic") as c:
  
    if "cRun" not in globals():
     cRun = 1

    c.target = AttrDict(z0=0*u.cm)
       
    
    #EmuTarget (Brick!)
    c.EmuTarget = AttrDict(z=0*u.cm)
    c.EmuTarget.zEmuTarget = 0 * u.cm

    c.EmuTarget.PBTh = 0.0175 * u.cm
    c.EmuTarget.EmTh = 0.0070 * u.cm

    c.EmuTarget.EmX = 12.5 * u.cm
    c.EmuTarget.EmY = 9.9 * u.cm

    if cRun == 7:
        c.EmuTarget.PasSlabTh = 0.1 *u.cm #now tungsten is 1 mm (SNDLHC)
    else:
        c.EmuTarget.PasSlabTh = 0.1 * u.cm #passive slab in ECC (lead for July 2018 measurement, molybdenum/tungsten for SHiP target replica)
    
    c.EmuTarget.EPlW = 2* c.EmuTarget.EmTh + c.EmuTarget.PBTh
    c.EmuTarget.AllPW = c.EmuTarget.PasSlabTh + c.EmuTarget.EPlW
    
    c.EmuTarget.BrX = 12.9 *u.cm
    c.EmuTarget.BrY = 10.5 *u.cm
    
    c.EmuTarget.BrPackZ = 0.1 * u.cm
    c.EmuTarget.BrPackX = c.EmuTarget.BrX - c.EmuTarget.EmX
    c.EmuTarget.BrPackY = c.EmuTarget.BrY - c.EmuTarget.EmY
    
    c.EmuTarget.BrZ = 54 * c.EmuTarget.AllPW + c.EmuTarget.EPlW +c.EmuTarget.BrPackZ
    
    c.EmuTarget.NPlates = [56, 42, 28, 14, 28, 28, 4, 14] #number of passive slabs for each DESY19 RUN
    c.EmuTarget.NPlates_second = 29 #used only in RUN8
    
    c.EmuTarget.ECCdistance = 10 #distance between the two bricks
    c.EmuTarget.PassiveDZ = 50 *u.mm #thickness of passive brick between SciFis
    
    c.EmuTarget.cRun = cRun

    c.EmuTarget.eEnergy = [6,6,6,6,2,4,6,6] #electron energy for each DESY19 RUN

    c.EmuTarget.TX = c.EmuTarget.EmX
    c.EmuTarget.TY = c.EmuTarget.EmY
    c.EmuTarget.TZ = c.EmuTarget.NPlates[cRun-1] * c.EmuTarget.AllPW +  c.EmuTarget.EPlW


    ##SCIFIs
    # SciFi Modules
    c.SciFi = AttrDict(z=0*u.cm)
    # mother volume dimensions
    c.SciFi.DX = 13*u.cm
    c.SciFi.DY = 13*u.cm
    c.SciFi.DZ = 5.0 * u.cm

    # dimensions of stations
    c.SciFi.StatDX = c.SciFi.DX
    c.SciFi.StatDY = c.SciFi.DY
    c.SciFi.StatDZ = 1.0 * u.cm

    #positions along z

    c.SciFi.zpos1 = 2*u.cm+ c.SciFi.DZ/2.
    c.SciFi.zpos2 = c.SciFi.zpos1 + c.SciFi.DZ/2 + (15.5 * u.cm)





