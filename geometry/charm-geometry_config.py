import shipunit as u
from ShipGeoConfig import AttrDict, ConfigRegistry
# the following params should be passed through 'ConfigRegistry.loadpy' method
# none for the moment
with ConfigRegistry.register_config("basic") as c:

    c.MufluxSpectrometer = AttrDict(z = 0*u.cm)  
    # False = charm cross-section; True = muon flux measurement
    c.MufluxSpectrometer.muflux  = False

    if "targetOpt" not in globals():
       targetOpt = 18 # add extra 20cm of tungsten as per 13/06/2017
    
    c.target = AttrDict(z0=0*u.cm)

    #BOX (Brick!)
    c.Box = AttrDict(z=0*u.cm)
    c.Box.zBox = 0 * u.cm
    c.Box.EmTh = 0.0070 * u.cm
    c.Box.EmX = 12.5 * u.cm
    c.Box.EmY = 9.9 * u.cm
    c.Box.PBTh = 0.0175 * u.cm
    c.Box.MolybdenumTh = 0.3 * u.cm
    c.Box.EPlW = 2* c.Box.EmTh + c.Box.PBTh
    c.Box.AllPW = c.Box.MolybdenumTh + c.Box.EPlW
    c.Box.BrX = 12.9 *u.cm
    c.Box.BrY = 10.5 *u.cm
    c.Box.BrPackZ = 0.1 * u.cm
    c.Box.BrPackX = c.Box.BrX - c.Box.EmX
    c.Box.BrPackY = c.Box.BrY - c.Box.EmY
    c.Box.BrZ = 54 * c.Box.AllPW + c.Box.EPlW +c.Box.BrPackZ
    c.Box.CoolX = c.Box.EmX #all the passive layers have the same transverse dimensions of nuclear emulsion films 
    c.Box.CoolY = c.Box.EmY
    c.Box.CoolZ = 0.5 * u.cm
    c.Box.CoatX = c.Box.EmX 
    c.Box.CoatY = c.Box.EmY
    c.Box.CoatZ = 0.15 * u.cm
    c.Box.TX = c.Box.EmX
    c.Box.TY = c.Box.EmY

    #passive blocks thicknesses
    c.Box.Molblock1Z = 7.7 * u.cm 
    c.Box.Molblock2Z = 2.2 * u.cm 
    c.Box.Molblock3Z = 4.7 * u.cm
    c.Box.Molblock4Z = 6.2 * u.cm
    c.Box.Wblock1Z = 4.7 * u.cm
    c.Box.Wblock2Z = 7.7 * u.cm
    c.Box.Wblock3Z = 9.7 * u.cm
    c.Box.Wblock3_5Z = 19.7 * u.cm
    c.Box.Wblock4Z = 8.45 * u.cm

    #passive sampling
    c.Box.Passive3mmZ = 0.3 * u.cm
    c.Box.Passive2mmZ = 0.2 * u.cm
    c.Box.Passive1mmZ = 0.1 * u.cm

    #OPTIONS FOR CHARM XSEC DETECTOR
    c.Box.gausbeam = True
    c.Box.charmtarget = False
    #c.Box.Bvalue = 1 *u.tesla
    c.Box.Bvalue = 0 * u.tesla
    c.Box.GapInTargetTh = 0 * u.cm 
    #c.Box.GapPostTargetTh = 5 * u.cm
    c.Box.GapPostTargetTh = 1.2 * u.cm     
    #c.Box.GapPostTargetTh = 0*u.cm
    c.Box.RunNumber =  3 #run configuration for charm

    #TO CHANGE THE RUN (a greater number than 6 will make all target parts passive, number 0 will make the first 11 blocks active)   
    
    c.Box.TZ = 100 * u.cm #default value    
    
    if c.Box.RunNumber == 1:     
      c.Box.TZ = 4.55 * u.cm + c.Box.GapInTargetTh
    elif c.Box.RunNumber == 2:
      c.Box.TZ = 9.353 * u.cm + 2 * c.Box.GapInTargetTh
    elif c.Box.RunNumber == 3:
      c.Box.TZ = 14.7 * u.cm + 3 * c.Box.GapInTargetTh
    elif c.Box.RunNumber == 4:
      c.Box.TZ = 20.7 * u.cm + 4 * c.Box.GapInTargetTh
    elif c.Box.RunNumber == 5:
      c.Box.TZ = 26.7 * u.cm + 5 * c.Box.GapInTargetTh
    elif c.Box.RunNumber == 6:
      c.Box.TZ = 29.3 * u.cm + 6 * c.Box.GapInTargetTh
    elif c.Box.RunNumber == 0:
      c.Box.TZ = 211.2 *u.cm #All target (first 11 blocks active, with 10 cm gaps)
      #c.Box.TZ = 151.2 *u.cm #All target (without gaps, used for z distributions) 
    elif c.Box.RunNumber > 6:
      c.Box.TZ = 206.9 *u.cm #All passive target
    
    # target absorber muon shield setup, decayVolume.length = nominal EOI length, only kept to define z=0
    c.decayVolume            =  AttrDict(z=0*u.cm)
    c.decayVolume.length     =   50*u.m

    c.hadronAbsorber              =  AttrDict(z=0*u.cm)
    c.hadronAbsorber.length =  2.4*u.m    
    
    c.hadronAbsorber.z     =   - c.hadronAbsorber.length/2.

    c.target               =  AttrDict(z=0*u.cm)
    c.targetOpt            = targetOpt

    c.target.M1 = "molybdenummisis"
    c.target.L1 = 8.52*u.cm
    c.target.M2 = "molybdenummisis"
    c.target.L2 = 2.8*u.cm
    c.target.M3 = "molybdenummisis"
    c.target.L3 = 2.8*u.cm
    c.target.M4 = "molybdenummisis"
    c.target.L4 = 2.8*u.cm
    c.target.M5 = "molybdenummisis"
    c.target.L5 = 2.8*u.cm
    c.target.M6 = "molybdenummisis"
    c.target.L6 = 2.8*u.cm
    c.target.M7 = "molybdenummisis"
    c.target.L7 = 2.8*u.cm
    c.target.M8 = "molybdenummisis"
    c.target.L8 = 2.8*u.cm
    c.target.M9 = "molybdenummisis"
    c.target.L9 = 5.4*u.cm
    c.target.M10 = "molybdenummisis"
    c.target.L10 = 5.4*u.cm
    c.target.M11 = "molybdenummisis"
    c.target.L11 = 6.96*u.cm
    c.target.M12 = "molybdenummisis"
    c.target.L12 = 8.52*u.cm
    c.target.M13 = "molybdenummisis"
    c.target.L13 = 8.52*u.cm
    c.target.M14 = "tungstenmisis"
    c.target.L14 = 5.17*u.cm
    c.target.M15 = "tungstenmisis"
    c.target.L15 = 8.3*u.cm
    c.target.M16 = "tungstenmisis"
    c.target.L16 = 10.39*u.cm
    c.target.M17 = "tungstenmisis"
    c.target.L17 = 20.82*u.cm
    c.target.M18 = "tungstenmisis"
    c.target.L18 = 36.47*u.cm
    c.target.sl  =  0.5*u.cm  # H20 slit *17 times
    #c.target.xy  = 15.*u.cm   # diameter of muflux target
    c.target.xy  = 10.*u.cm   # new diameter of muflux target    
    # 5.0 cm is for front and endcaps
    c.target.length = 17*c.target.sl + c.target.L1 + 7*c.target.L2 + 3*c.target.L9 + c.target.L11 + 3*c.target.L12 + c.target.L16 + c.target.L17 + c.target.L18 + 5.0*u.cm
    
    # interaction point, start of target
    c.target.z   =  c.hadronAbsorber.z - c.hadronAbsorber.length/2. - c.target.length/2.
    c.target.z0  =  c.target.z - c.target.length/2.
       
    #Spectrometer
    c.Spectrometer = AttrDict(z = 0*u.cm)
    #Parameters for Goliath by Annarita
    c.Spectrometer.LS = 4.5*u.m
    c.Spectrometer.TS = 3.6*u.m
    c.Spectrometer.CoilR = 1*u.m
    c.Spectrometer.UpCoilH = 45*u.cm
    c.Spectrometer.LowCoilH = 30*u.cm
    c.Spectrometer.CoilD = 105*u.cm
    c.Spectrometer.BasisH = 57*u.cm
    c.Spectrometer.H = 2*c.Spectrometer.BasisH + c.Spectrometer.CoilD + c.Spectrometer.UpCoilH + c.Spectrometer.LowCoilH

    # -----Drift tube part --------
    #c.MufluxSpectrometer.v_drift = 1./(30*u.ns/u.mm) # for baseline NA62 5mm radius straws)
    c.MufluxSpectrometer.v_drift = 1./(72*u.ns/u.mm) # 1300 ns max for 36.3 mm drifttubes
    c.MufluxSpectrometer.sigma_spatial = 0.027*u.cm # from Daniel 8feb2018
    #c.MufluxSpectrometer.sigma_spatial = 0.035*u.cm # 25% worse   
    c.MufluxSpectrometer.TubeLength         = 160.*u.cm
    c.MufluxSpectrometer.TubeLength12       = 100.*u.cm    
    c.MufluxSpectrometer.tr12ydim           = 100.*u.cm
    c.MufluxSpectrometer.tr34xdim           = 200.*u.cm
    c.MufluxSpectrometer.tr12xdim           = 50.*u.cm
    c.MufluxSpectrometer.tr34ydim           = 160.*u.cm
    
    # parameters for drift tubes
    c.MufluxSpectrometer.InnerTubeDiameter  = 3.63*u.cm 
    c.MufluxSpectrometer.WallThickness      = 0.085*u.cm
    c.MufluxSpectrometer.OuterTubeDiameter  = (c.MufluxSpectrometer.InnerTubeDiameter + 2*c.MufluxSpectrometer.WallThickness)

    c.MufluxSpectrometer.TubePitch          = 4.2*u.cm
    c.MufluxSpectrometer.DeltazLayer        = 3.8*u.cm
    c.MufluxSpectrometer.DeltazPlane        = 8.*u.cm
    
    c.MufluxSpectrometer.TubesPerLayer      = 12
    c.MufluxSpectrometer.ViewAngle          = 60
    c.MufluxSpectrometer.WireThickness      = 0.0045*u.cm
    c.MufluxSpectrometer.DeltazView         = 16.*u.cm
    
    c.MufluxSpectrometer.T3T4_distance = 100 *u.cm
    c.MufluxSpectrometer.diststereo         = 16.*u.cm  
    c.MufluxSpectrometer.distT1T2           = 11.*u.cm   
    c.MufluxSpectrometer.distT3T4           = 1.6*u.m       
        
    if c.MufluxSpectrometer.muflux == True:    
       c.Spectrometer.DX = 2.*u.m
       c.Spectrometer.DY = 1.6*u.m   
       c.Spectrometer.DZ = 16.*u.cm
    else:        
       c.Spectrometer.DX = 1*u.m
       c.Spectrometer.DY = 0.5*u.m
       #c.Spectrometer.DZ = 6*u.cm
       c.Spectrometer.DZ = 13.5 * u.cm
    c.MufluxSpectrometer.DX = 2.*u.m
    c.MufluxSpectrometer.DY = 1.6*u.m
    c.MufluxSpectrometer.DZ = 16.*u.cm
    #These parameters are used only by the charm detector ---   
    c.Spectrometer.D1Short = 3.36 * u.cm / 2.;
    c.Spectrometer.D1Long = 4 * u.cm;   
    c.Spectrometer.DimZSi = 0.0200 * u.cm
    c.Spectrometer.PairSiDistance = 0.6 * u.cm +c.Spectrometer.DimZSi 
    c.Spectrometer.Sioverlap = 0.2*u.cm            

    c.Spectrometer.SX = c.Spectrometer.DX
    c.Spectrometer.SY = c.Spectrometer.DY    
    c.Spectrometer.Sidist = 5 * u.cm      
    
    #position of module centres
    c.Spectrometer.zSi0 = c.Spectrometer.PairSiDistance/2. + c.Spectrometer.DimZSi/2.
    c.Spectrometer.zSi1 = c.Spectrometer.zSi0 + 2.5 *u.cm
    c.Spectrometer.zSi2 = c.Spectrometer.zSi1 + 2.5 *u.cm
    c.Spectrometer.zSi3 = c.Spectrometer.zSi2 + 2.5 *u.cm
    c.Spectrometer.zSi4 = c.Spectrometer.zSi3 + 2.5 *u.cm
    c.Spectrometer.zSi5 = c.Spectrometer.zSi4 + 2.5 *u.cm    

    c.Spectrometer.DSciFi1X = 40 * u.cm;
    c.Spectrometer.DSciFi1Y = 40 * u.cm;
    c.Spectrometer.DSciFi2X = 40 * u.cm;
    c.Spectrometer.DSciFi2Y = 40 * u.cm;  

    c.Spectrometer.Bvalue = 1 * u.tesla;

    #-------------------------------------------------------
            
    
    #Scintillator
    c.Scintillator = AttrDict(z = 0*u.cm)
    c.Scintillator.Scoring1X           = 55.*u.cm
    c.Scintillator.Scoring1Y           = 110.*u.cm    
    c.Scintillator.DistT1               = 3.7*u.cm       
    c.Scintillator.DistT2               = 2*(2*c.Spectrometer.DZ +c.MufluxSpectrometer.diststereo) + c.MufluxSpectrometer.distT1T2 + 10*u.cm
                   
    if c.MufluxSpectrometer.muflux == True: 
       #these parameters are also used inside the MufluxSpectrometer.cxx
       c.Spectrometer.SZ = 2*(2*c.Spectrometer.DZ +c.MufluxSpectrometer.diststereo) + c.MufluxSpectrometer.distT1T2 +4.5*u.m + c.MufluxSpectrometer.distT3T4 + 2*(2*c.Spectrometer.DZ) + 2.5*u.cm
    else: 
       c.Spectrometer.SZ = c.Spectrometer.DZ*2 + c.Spectrometer.DimZSi*3 + 2 * c.Spectrometer.Sidist + 80 *u.cm + 4.5*u.m #4.5 m is the Goliath length
 
    c.Spectrometer.zBox = c.Spectrometer.SZ/2

    #position of SciFis 
    distGoliathSciFi1 = 10*u.cm
    c.Spectrometer.zSciFi1 = c.Spectrometer.zSi5 +c.Spectrometer.PairSiDistance/2.+c.Spectrometer.DimZSi/2. + c.Spectrometer.LS + distGoliathSciFi1 +  c.Spectrometer.DZ/2.
    c.Spectrometer.zSciFi2 = c.Spectrometer.zSciFi1 + c.Spectrometer.DZ/2. + c.Spectrometer.Sidist + c.Spectrometer.DZ/2.

    #Muon Filter
    
    c.MuonTagger = AttrDict(z = 0*u.cm)
    c.MuonTagger.concreteslabs = False; #if true, set Concrete as material of last three slabs 
    c.MuonTagger.PTh = 80 * u.cm;
    c.MuonTagger.PTh1 = 40 * u.cm #last 3 slabs' thickness
    c.MuonTagger.STh = 5.0 * u.cm
    c.MuonTagger.BX = 2.00 * u.m
    c.MuonTagger.BY = 1.30 * u.m
    c.MuonTagger.BZ = c.MuonTagger.PTh * 2 + c.MuonTagger.PTh1 * 3 + c.MuonTagger.STh * 6
    
    if c.MufluxSpectrometer.muflux == True:
       #for the muflux measurement the muontagger has to be moved back
       #c.MuonTagger.zBox = c.Spectrometer.SZ+ c.MuonTagger.BZ*1./2.- 45*u.cm
       c.MuonTagger.zBox = c.Spectrometer.SZ+ c.MuonTagger.BZ*3./2. + 5*u.cm -310*u.cm
    else:    
       #c.MuonTagger.zBox = c.Spectrometer.SZ+ c.MuonTagger.BZ/2 + 5*u.cm
       c.MuonTagger.zBox = c.Spectrometer.zSi5 +c.Spectrometer.PairSiDistance/2.+c.Spectrometer.DimZSi/2. + c.Spectrometer.LS +           c.MuonTagger.BZ/2. + 199.5*u.cm#starting from 223 cm from Goliath, like in muonflux measurement

    c.MuonTagger.PX = c.MuonTagger.BX
    c.MuonTagger.PY = c.MuonTagger.BY
    c.MuonTagger.SX = c.MuonTagger.BX
    c.MuonTagger.SY = c.MuonTagger.BY
    c.MuonTagger.HX = 5 * u.cm #dimensions of central hole
    c.MuonTagger.HY = 5 * u.cm
