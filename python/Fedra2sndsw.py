'''Conversion tools from FEDRA to SNDSW'''

import ROOT as r
import fedrarootlogon
import numpy as np

#getting class instance
emureader = r.EmulsionDet()

def importgeofile(geofile):
    '''importing geometry file, can be replaced with sndsw config function if needed'''
    r.gGeoManager.Import(geofile)


def convertseg(seg,brickID,refplate = 60):
 '''convert position and angles from an EdbSegP into the global system
        Seg: EdbSegP instance;
        brickID: number of the brick the track was reconstructed in (ex. 31);
        returns a 6-values array (x,y,z,Vx,Vy,Vz)
        refplate: which is our reference plate
 '''
 detID = int(brickID * 1e3 + refplate)
 

 localarr = np.array([seg.X(), seg.Y(), seg.Z()])
 globalarr = np.array([0., 0., 0.])

 emureader.GetPosition(detID,localarr,globalarr)

 anglocalarr = np.array([seg.TX(), seg.TY(), 1.])
 angglobalarr = np.array([0., 0., 0.])

 emureader.GetAngles(detID,anglocalarr,angglobalarr)


 return np.concatenate([globalarr,angglobalarr])

def converttrack(track,brickID, refplate=60, fittedsegments = False):
 '''convert all segments from a volumetrack:
        track: EdbTrackP instance;
        brickID: number of the brick the track was reconstructed in (ex. 31);
        fittedsegments: used fitted segments instead of true segments (default False)
        refplate: which is our reference plate
 '''

 nseg = track.N()
 globaltracklist = []
 for iseg in range(nseg):
        myseg = track.GetSegment(iseg)
        if fittedsegments: #replace true segment with fittedsegment
            myseg = track.GetSegmentF(iseg)
        globalsegarr = convertseg(myseg,brickID,refplate)
        globaltracklist.append(globalsegarr)

 globaltrackarr = np.array(globaltracklist)
 return globaltrackarr