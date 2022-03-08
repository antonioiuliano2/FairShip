import ROOT,os
import rootUtils as ut
from array import array
import shipunit as u

h={}
from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-r", "--runNumber", dest="runNumber", help="run number", type=int,required=False)
parser.add_argument("-p", "--path", dest="path", help="run number",required=False,default="")
parser.add_argument("-f", "--inputFile", dest="inputFile", help="input file MC",default="",required=False)
parser.add_argument("-g", "--geoFile", dest="geoFile", help="geofile", required=True)
options = parser.parse_args()
trans2local = False

import SndlhcGeo
geo = SndlhcGeo.GeoInterface(options.geoFile)

lsOfGlobals = ROOT.gROOT.GetListOfGlobals()
lsOfGlobals.Add(geo.modules['Scifi'])
lsOfGlobals.Add(geo.modules['MuFilter'])

mc = False
if options.inputFile=="":
  f=ROOT.TFile.Open(options.path+'sndsw_raw_'+str(options.runNumber).zfill(6)+'.root')
  eventTree = f.rawConv
else:
  f=ROOT.TFile.Open(options.path+options.inputFile)
  if f.FindKey('cbmsim'):
        eventTree = f.cbmsim
        mc = True
  else:   eventTree = f.rawConv

# backward compatbility for early converted events
eventTree.GetEvent(0)
if eventTree.GetBranch('Digi_MuFilterHit'): eventTree.Digi_MuFilterHits = eventTree.Digi_MuFilterHit

nav = ROOT.gGeoManager.GetCurrentNavigator()

Nlimit = 4
onlyScifi = False
def goodEvent():
           stations = {'Scifi':{},'Mufi':{}}
           for d in eventTree.Digi_ScifiHits:
               stations['Scifi'][d.GetDetectorID()//1000000] = 1
           for d in eventTree.Digi_MuFilterHits:
               plane = d.GetDetectorID()//1000
               stations['Mufi'][plane] = 1
           totalN = len(stations['Mufi'])+len(stations['Scifi'])
           if onlyScifi and len(stations['Scifi'])>Nlimit: return True
           elif not onlyScifi  and totalN >  Nlimit: return True
           else: False

def loopEvents(start=0,save=False,goodEvents=False,withTrack=-1,Setup=''):
 if 'simpleDisplay' not in h: ut.bookCanvas(h,key='simpleDisplay',title='simple event display',nx=1200,ny=1600,cx=1,cy=2)
 h['simpleDisplay'].cd(1)
 zStart = 250. # TI18 coordinate system
 if Setup == 'H6': zStart = 60.
 if Setup == 'TP': zStart = -50. # old coordinate system with origin in middle of target
 ut.bookHist(h,'xz','x vs z',500,zStart,zStart+320.,100,-100.,10.)
 ut.bookHist(h,'yz','y vs z',500,zStart,zStart+320.,100,0.,80.)
 proj = {1:'xz',2:'yz'}
 h['xz'].SetStats(0)
 h['yz'].SetStats(0)

 N = -1
 Tprev = -1
 A,B = ROOT.TVector3(),ROOT.TVector3()
 ptext={0:'   Y projection',1:'   X projection'}
 text = ROOT.TLatex()
 for sTree in eventTree:
    N+=1
    if N<start: continue
    if goodEvents and not goodEvent(): continue
    print( "event ->",N )

    digis = []
    if sTree.FindBranch("Digi_ScifiHits"): digis.append(sTree.Digi_ScifiHits)
    if sTree.FindBranch("Digi_MuFilterHits"): digis.append(sTree.Digi_MuFilterHits)
    if sTree.FindBranch("Digi_MuFilterHit"): digis.append(sTree.Digi_MuFilterHit)
    empty = True
    for x in digis:
       if x.GetEntries()>0: empty = False
    if empty: continue
    h['hitCollectionX']= {'Scifi':[0,ROOT.TGraph()],'US':[0,ROOT.TGraph()],'DS':[0,ROOT.TGraph()]}
    h['hitCollectionY']= {'Veto':[0,ROOT.TGraph()],'Scifi':[0,ROOT.TGraph()],'US':[0,ROOT.TGraph()],'DS':[0,ROOT.TGraph()]}
    systems = {1:'Veto',2:'US',3:'DS',0:'Scifi'}
    for collection in ['hitCollectionX','hitCollectionY']:
       for c in h[collection]:
          rc=h[collection][c][1].SetName(c)
          rc=h[collection][c][1].Set(0)

    if sTree.FindBranch("EventHeader"):
       T = sTree.EventHeader.GetEventTime()
       dT = 0
       if Tprev >0: dT = T-Tprev
       Tprev = T
    for p in proj:
       rc = h[ 'simpleDisplay'].cd(p)
       h[proj[p]].SetStats(0)
       h[proj[p]].Draw('b')

    for D in digis:
      for digi in D:
         detID = digi.GetDetectorID()
         if digi.GetName()  == 'MuFilterHit':
            system = digi.GetSystem()
            geo.modules['MuFilter'].GetPosition(detID,A,B)
            if trans2local:
                curPath = nav.GetPath()
                tmp = curPath.rfind('/')
                nav.cd(curPath[:tmp])
         else:
            geo.modules['Scifi'].GetSiPMPosition(detID,A,B)
            if trans2local:
                curPath = nav.GetPath()
                tmp = curPath.rfind('/')
                nav.cd(curPath[:tmp])
            system = 0
         globA,locA = array('d',[A[0],A[1],A[2]]),array('d',[A[0],A[1],A[2]])
         if trans2local:   nav.MasterToLocal(globA,locA)
         Z = A[2]
         if digi.isVertical():
                   collection = 'hitCollectionX'
                   Y = locA[0]
         else:                         
                   collection = 'hitCollectionY'
                   Y = locA[1]
         c = h[collection][systems[system]]
         rc = c[1].SetPoint(c[0],Z,Y)
         c[0]+=1 
    h['hitCollectionY']['Veto'][1].SetMarkerColor(ROOT.kRed)
    h['hitCollectionY']['Scifi'][1].SetMarkerColor(ROOT.kBlue)
    h['hitCollectionX']['Scifi'][1].SetMarkerColor(ROOT.kBlue)
    h['hitCollectionY']['US'][1].SetMarkerColor(ROOT.kGreen)
    h['hitCollectionY']['DS'][1].SetMarkerColor(ROOT.kCyan)
    h['hitCollectionX']['DS'][1].SetMarkerColor(ROOT.kCyan)
    k = 1
    for collection in ['hitCollectionX','hitCollectionY']:
       h[ 'simpleDisplay'].cd(k)
       k+=1
       for c in h[collection]:
          print(collection.split('ion')[1],c, h[collection][c][1].GetN())
          if h[collection][c][1].GetN()<1: continue
          h[collection][c][1].SetMarkerStyle(20+k)
          h[collection][c][1].SetMarkerSize(1.5)
          rc=h[collection][c][1].Draw('sameP')
          h['display:'+c]=h[collection][c][1]
    if goodEvent():
         if withTrack == 1: addTrack()
         if withTrack == 2: addTrack(True)
         dumpVeto()
    h[ 'simpleDisplay'].Update()
    if save: h['simpleDisplay'].Print('event_'+"{:04d}".format(N)+'.png')
    rc = input("hit return for next event or q for quit: ")
    if rc=='q': break
 if save: os.system("convert -delay 60 -loop 0 *.png animated.gif")

def addTrack(scifi=False):
   xax = h['xz'].GetXaxis()
   if scifi:  Scifi_track()
   else:     trackTask.ExecuteTask()
   zEx = xax.GetBinCenter(1)
   for   aTrack in eventTree.fittedTracks:
      for p in [0,1]:
          h['aLine'+str(p)] = ROOT.TGraph()
      mom    = aTrack.getFittedState().getMom()
      pos      = aTrack.getFittedState().getPos()
      lam      = (zEx-pos.z())/mom.z()
      Ex        = [pos.x()+lam*mom.x(),pos.y()+lam*mom.y()]
      for p in [0,1]:   h['aLine'+str(p)].SetPoint(0,zEx,Ex[p])
      for i in range(aTrack.getNumPointsWithMeasurement()):
         state = aTrack.getFittedState(i)
         pos    = state.getPos()
         for p in [0,1]:
             h['aLine'+str(p)].SetPoint(i+1,pos[2],pos[p])

      for p in [0,1]:
             tc = h[ 'simpleDisplay'].cd(p+1)
             h['aLine'+str(p)].SetLineColor(ROOT.kRed)
             h['aLine'+str(p)].SetLineWidth(2)
             h['aLine'+str(p)].Draw('same')
             tc.Update()
             h[ 'simpleDisplay'].Update()

def Scifi_track():
# check for low occupancy and enough hits in Scifi
    eventTree.fittedTracks = []
    clusters = trackTask.scifiCluster()
    stations = {}
    for s in range(1,6):
       for o in range(2):
          stations[s*10+o] = []
    for cl in clusters:
         detID = cl.GetFirst()
         s  = detID//1000000
         o = (detID//100000)%10
         stations[s*10+o].append(detID)
    nclusters = 0
    check = {}
    for s in range(1,6):
       for o in range(2):
            if len(stations[s*10+o]) > 0: check[s*10+o]=1
            nclusters+=len(stations[s*10+o])
    if len(check)<8 or nclusters > 9: return -1
# build trackCandidate
    hitlist = {}
    for k in range(len(clusters)):
           hitlist[k] = clusters[k]
    theTrack = trackTask.fitTrack(hitlist)
    eventTree.ScifiClusters = clusters
    eventTree.fittedTracks.append(theTrack)
def dumpVeto():
    muHits = {10:[],11:[]}
    for aHit in eventTree.Digi_MuFilterHits:
         if not aHit.isValid(): continue
         s = aHit.GetDetectorID()//10000
         if s>1: continue
         p = (aHit.GetDetectorID()//1000)%10
         bar = (aHit.GetDetectorID()%1000)%60
         plane = s*10+p
         muHits[plane].append(aHit)
    for plane in [10,11]:
        for aHit in muHits[plane]:
          S =aHit.GetAllSignals()
          txt = ""
          for x in S:
              if x[1]>0: txt+=str(x[1])+" "
          print(plane, (aHit.GetDetectorID()%1000)%60, txt)
import SndlhcTracking
trackTask = SndlhcTracking.Tracking() 
trackTask.InitTask(eventTree)
