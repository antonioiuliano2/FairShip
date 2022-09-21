#!/usr/bin/env python
import ROOT,os,sys
import rootUtils as ut
import shipunit as u

A,B  = ROOT.TVector3(),ROOT.TVector3()
detector = "mufi-"
class Mufi_hitMaps(ROOT.FairTask):
   " produce hitmaps for MuFilter, Veto/US/DS"
   """
  veto system 2 layers with 7 bars and 8 sipm channels on both ends
  US system 5 layers with 10 bars and 8 sipm channels on both ends
  DS system horizontal(3) planes, 60 bars, readout on both sides, single channel
                          vertical(4) planes, 60 bar, readout on top, single channel
   """
   def Init(self,options,monitor):
       self.M = monitor
       sdict = self.M.sdict
       h = self.M.h
       run = ROOT.FairRunAna.Instance()
       self.trackTask = run.GetTask('simpleTracking')
       if not self.trackTask: self.trackTask = run.GetTask('houghTransform')
       ioman = ROOT.FairRootManager.Instance()
       self.OT = ioman.GetSink().GetOutTree()

       ut.bookHist(h,detector+'Noise','events with hits in single plane; s*10+l;',40,0.5,39.5)
       for s in monitor.systemAndPlanes:
            ut.bookHist(h,sdict[s]+'Mult','QDCs vs nr hits; #hits; QDC [a.u.]',200,0.,800.,200,0.,300.)
            for l in range(monitor.systemAndPlanes[s]):
                  ut.bookHist(h,detector+'hitmult_'+str(s*10+l),'hit mult / plane '+sdict[s]+str(l)+'; #hits',61,-0.5,60.5)
                  ut.bookHist(h,detector+'hit_'+str(s*10+l),'channel map / plane '+sdict[s]+str(l)+'; #channel',160,-0.5,159.5)
                  ut.bookHist(h,detector+'Xhit_'+str(s*10+l),'Xchannel map / plane '+sdict[s]+str(l)+'; #channel',160,-0.5,159.5)

                  if s==3:  ut.bookHist(h,detector+'bar_'+str(s*10+l),'bar map / plane '+sdict[s]+str(l)+'; #bar',60,-0.5,59.5)
                  else:       ut.bookHist(h,detector+'bar_'+str(s*10+l),'bar map / plane '+sdict[s]+str(l)+'; #bar',10,-0.5,9.5)
                  ut.bookHist(h,detector+'sig_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                  if s==2:    
                      ut.bookHist(h,detector+'sigS_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                      ut.bookHist(h,detector+'TsigS_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                  ut.bookHist(h,detector+'sigL_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                  ut.bookHist(h,detector+'sigR_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                  ut.bookHist(h,detector+'Tsig_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                  ut.bookHist(h,detector+'TsigL_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                  ut.bookHist(h,detector+'TsigR_'+str(s*10+l),'signal / plane '+sdict[s]+str(l)+'; QDC [a.u.]',200,0.0,200.)
                  # not used currently?
                  ut.bookHist(h,detector+'occ_'+str(s*10+l),'channel occupancy '+sdict[s]+str(l),100,0.0,200.)
                  ut.bookHist(h,detector+'occTag_'+str(s*10+l),'channel occupancy '+sdict[s]+str(l),100,0.0,200.)

                  ut.bookHist(h,detector+'leftvsright_1','Veto hits in left / right; Left: # hits; Right: # hits',10,-0.5,9.5,10,-0.5,9.5)
                  ut.bookHist(h,detector+'leftvsright_2','US hits in left / right; L: # hits; R: # hits',10,-0.5,9.5,10,-0.5,9.5)
                  ut.bookHist(h,detector+'leftvsright_3','DS hits in left / right; L: # hits; R: # hits',2,-0.5,1.5,2,-0.5,1.5)
                  ut.bookHist(h,detector+'leftvsright_signal_1','Veto signal in left / right; Left: QDC [a.u.]; Right: QDC [a.u.]',100,-0.5,200.,100,-0.5,200.)
                  ut.bookHist(h,detector+'leftvsright_signal_2','US signal in left / right; L: QDC [a.u.]; R: QDC [a.u.]',100,-0.5,200.,100,-0.5,200.)
                  ut.bookHist(h,detector+'leftvsright_signal_3','DS signal in left / right; L: QDC [a.u.]; R: QDC [a.u.]',100,-0.5,200.,100,-0.5,200.)

                  ut.bookHist(h,detector+'dtime','delta event time; dt [ns]',100,0.0,1000.)
                  ut.bookHist(h,detector+'dtimeu','delta event time; dt [us]',100,0.0,1000.)
                  ut.bookHist(h,detector+'dtimem','delta event time; dt [ms]',100,0.0,1000.)

                  ut.bookHist(h,detector+'bs','beam spot; x[cm]; y[cm]',100,-100.,10.,100,0.,80.)
                  ut.bookHist(h,detector+'bsDS','beam spot, #bar X, #bar Y',60,-0.5,59.5,60,-0.5,59.5)
                  ut.bookHist(h,detector+'slopes','muon DS track slopes; slope X [rad]; slope Y [rad]',150,-1.5,1.5,150,-1.5,1.5)
                  ut.bookHist(h,detector+'trackPos','muon DS track pos; x [cm]; y [cm]',100,-90,10.,80,0.,80.)
                  ut.bookHist(h,detector+'trackPosBeam','beam track pos slopes<0.1rad; x [cm]; y [cm]',100,-90,10.,80,0.,80.)
                  for bar in range(monitor.systemAndBars[s]):
                     ut.bookHist(h,detector+'chanmult_'+str(s*1000+100*l+bar),'channel mult / bar '+sdict[s]+str(l)+"-"+str(bar)+'; #channels',20,-0.5,19.5)
#
                  xmin = options.Mufixmin
                  xmax = -xmin
                  ut.bookHist(h,detector+'resX_'+sdict[s]+str(s*10+l),'residual X'+str(s*10+l)+'; [#cm]',
                      100,xmin,xmax,60,-60.,0.)
                  ut.bookHist(h,detector+'resY_'+sdict[s]+str(s*10+l),'residual  Y'+str(s*10+l)+'; [#cm]',
                      100,xmin,xmax,70,2.,68.)


       self.listOfHits = {1:[],2:[],3:[]}
   def ExecuteEvent(self,event):
       systemAndPlanes =self.M.systemAndPlanes
       sdict = self.M.sdict
       h = self.M.h
       mult = {}
       planes = {}
       for i in self.listOfHits:  self.listOfHits[i].clear()
       for s in systemAndPlanes:
           for l in range(systemAndPlanes[s]):   mult[s*10+l]=0

       self.beamSpot(event)
       withDSTrack = False
       for aTrack in self.M.Reco_MuonTracks:
           if aTrack.GetUniqueID()==3: withDSTrack = True

       for aHit in event.Digi_MuFilterHits:
           Minfo = self.M.MuFilter_PlaneBars(aHit.GetDetectorID())
           s,l,bar = Minfo['station'],Minfo['plane'],Minfo['bar']
           nSiPMs = aHit.GetnSiPMs()
           nSides  = aHit.GetnSides()
           for c in aHit.GetAllSignals(False,False):
                if aHit.isMasked(c.first):
                     channel = bar*nSiPMs*nSides + c.first
                     rc = h[detector+'Xhit_'+str(s)+str(l)].Fill( channel )

           if not aHit.isValid(): continue
           mult[s*10+l]+=1
           key = s*100+l
           if not key in planes: planes[key] = {}
           sumSignal = self.M.map2Dict(aHit,'SumOfSignals')
           planes[key][bar] = [sumSignal['SumL'],sumSignal['SumR']]
# check left/right
           allChannels = self.M.map2Dict(aHit,'GetAllSignals')
           for c in allChannels:
               self.listOfHits[s].append(allChannels[c])
           Nleft,Nright,Sleft,Sright = 0,0,0,0
           for c in allChannels:
              if  nSiPMs > c:  # left side
                    Nleft+=1
                    Sleft+=allChannels[c]
              else:
                    Nright+=1
                    Sright+=allChannels[c]
           rc = h[detector+'chanmult_'+str(s*1000+100*l+bar)].Fill(Nleft)
           rc = h[detector+'chanmult_'+str(s*1000+100*l+bar)].Fill(10+Nright)
           if not aHit.isVertical():  # vertical DS plane is read out only on one side
              rc = h[detector+'leftvsright_'+str(s)].Fill(Nleft,Nright)
              rc = h[detector+'leftvsright_signal_'+str(s)].Fill(Sleft,Sright)
#
           for c in allChannels:
               channel = bar*nSiPMs*nSides + c
               rc = h[detector+'hit_'+str(s)+str(l)].Fill( int(channel))
               rc = h[detector+'bar_'+str(s)+str(l)].Fill(bar)
               if s==2 and self.M.smallSiPMchannel(c) : 
                     rc  = h[detector+'sigS_'+str(s)+str(l)].Fill(allChannels[c])
                     if withDSTrack: rc  = h[detector+'TsigS_'+str(s)+str(l)].Fill(allChannels[c])
               elif c<nSiPMs: 
                     rc  = h[detector+'sigL_'+str(s)+str(l)].Fill(allChannels[c])
                     if withDSTrack: rc  = h[detector+'TsigL_'+str(s)+str(l)].Fill(allChannels[c])
               else             :             
                     rc  = h[detector+'sigR_'+str(s)+str(l)].Fill(allChannels[c])
                     if withDSTrack: rc  = h[detector+'sigR_'+str(s)+str(l)].Fill(allChannels[c])
               rc  = h[detector+'sig_'+str(s)+str(l)].Fill(allChannels[c])
               if withDSTrack:  rc  = h[detector+'sig_'+str(s)+str(l)].Fill(allChannels[c])
           allChannels.clear()
#
       # noise event with many hits in one plane
       onePlane = []
       for x in mult:
           if mult[x]>3: onePlane.append(x)
       if len(onePlane)==1:
           rc = h[detector+'Noise'].Fill(onePlane[0])

#
       for s in self.listOfHits:
           nhits = len(self.listOfHits[s])
           qcdsum = 0
           for i in range(nhits):
               rc = h[sdict[s]+'Mult'].Fill(nhits, self.listOfHits[s][i])
       for s in systemAndPlanes:
          for l in range(systemAndPlanes[s]):   
             rc = h[detector+'hitmult_'+str(s*10+l)].Fill(mult[s*10+l])
# mufi residuals with scifi tracks
       for aTrack in self.M.Reco_MuonTracks:
           if not aTrack.GetUniqueID()==1: continue
           fitStatus = aTrack.getFitStatus()
           if not fitStatus.isFitConverged(): continue
           posMom = {}
           fstate =  aTrack.getFittedState()
           posMom['first'] = [fstate.getPos(),fstate.getMom()]
           # fstate =  aTrack.getFittedState(aTrack.getNumPointsWithMeasurement()-1) does not make a difference
           posMom['last'] = [fstate.getPos(),fstate.getMom()]
           for aHit in event.Digi_MuFilterHits:
              if not aHit.isValid(): continue
              Minfo = self.M.MuFilter_PlaneBars(aHit.GetDetectorID())
              s,l,bar = Minfo['station'],Minfo['plane'],Minfo['bar']
              self.M.MuFilter.GetPosition(aHit.GetDetectorID(),A,B)
# calculate DOCA
              if s==1: pos,mom = posMom['first']
              else: pos,mom = posMom['last']
              zEx = self.M.zPos['MuFilter'][s*10+l]
              lam = (zEx-pos.z())/mom.z()
              xEx,yEx = pos.x()+lam*mom.x(),pos.y()+lam*mom.y()
              pq = A-pos
              uCrossv= (B-A).Cross(mom)
              doca = pq.Dot(uCrossv)/uCrossv.Mag()
              rc = h[detector+'resX_'+sdict[s]+str(s*10+l)].Fill(doca/u.cm,xEx)
              rc = h[detector+'resY_'+sdict[s]+str(s*10+l)].Fill(doca/u.cm,yEx)

   def beamSpot(self,event):
      if not self.trackTask: return
      h = self.M.h
      Xbar = -10
      Ybar = -10
      for aTrack in self.M.Reco_MuonTracks:
         if not aTrack.GetUniqueID()==3: continue
         state = aTrack.getFittedState()
         pos    = state.getPos()
         rc = h[detector+'bs'].Fill(pos.x(),pos.y())
         mom = state.getMom()
         slopeX= mom.X()/mom.Z()
         slopeY= mom.Y()/mom.Z()
         h[detector+'slopes'].Fill(slopeX,slopeY)
         if not Ybar<0 and not Xbar<0 and abs(slopeY)<0.01: rc = h[detector+'bsDS'].Fill(Xbar,Ybar)
         pos = state.getPos()
         rc = h[detector+'trackPos'].Fill(pos.X(),pos.Y())
         if abs(slopeX)<0.1 and abs(slopeY)<0.1:  rc = h[detector+'trackPosBeam'].Fill(pos.X(),pos.Y())

   def Plot(self):
       h = self.M.h
       sdict = self.M.sdict
       systemAndPlanes =self.M.systemAndPlanes
       S = {1:[1800,800,2,1],2:[1800,1500,2,3],3:[1800,1800,2,4]}
       for s in S:
           ut.bookCanvas(h,detector+'hitmaps' +sdict[s],'hitmaps' +sdict[s],S[s][0],S[s][1],S[s][2],S[s][3])
           ut.bookCanvas(h,detector+'Xhitmaps' +sdict[s],'Xhitmaps' +sdict[s],S[s][0],S[s][1],S[s][2],S[s][3])
           ut.bookCanvas(h,detector+'barmaps'+sdict[s],'barmaps'+sdict[s],S[s][0],S[s][1],S[s][2],S[s][3])

           for l in range(systemAndPlanes[s]):
              n = l+1
              if s==3 and n==7: n=8
              tc = h[detector+'hitmaps'+sdict[s]].cd(n)
              tag = str(s)+str(l)
              h[detector+'hit_'+tag].Draw()
              tc = h[detector+'Xhitmaps'+sdict[s]].cd(n)
              h[detector+'Xhit_'+tag].Draw()

              tc = h[detector+'barmaps'+sdict[s]].cd(n)
              h[detector+'bar_'+tag].Draw()

       ut.bookCanvas(h,detector+'hitmult','hit multiplicities per plane',2000,1600,4,3)
       k=1
       for s in systemAndPlanes:
           for l in range(systemAndPlanes[s]):
              tc = h[detector+'hitmult'].cd(k)
              tc.SetLogy(1)
              k+=1
              rc = h[detector+'hitmult_'+str(s*10+l)].Draw()
       ut.bookCanvas(h,'noise',' ',1200,1800,1,1)
       tc = h['noise'].cd()
       h[detector+'Noise'].Draw()

       ut.bookCanvas(h,'VETO',' ',1200,1800,1,2)
       for l in range(2):
          tc = h['VETO'].cd(l+1)
          hname = detector+'hit_'+str(1)+str(l)
          h[hname].SetStats(0)
          h[hname].Draw()
          for n in range(7):
                x = (n+1)*16-0.5
                y = h[detector+'hit_'+str(1)+str(l)].GetMaximum()
                lname = 'L'+str(n)+hname
                h[lname] = ROOT.TLine(x,0,x,y)
                h[lname].SetLineColor(ROOT.kRed)
                h[lname].SetLineStyle(9)
                h[lname].Draw('same')

       ut.bookCanvas(h,'USBars',' ',1200,900,1,1)
       colours = {0:ROOT.kOrange,1:ROOT.kRed,2:ROOT.kGreen,3:ROOT.kBlue,4:ROOT.kMagenta,5:ROOT.kCyan,
                  6:ROOT.kAzure,7:ROOT.kPink,8:ROOT.kSpring}
       for i in range(5): 
           h[detector+'bar_2'+str(i)].SetLineColor(colours[i])
           h[detector+'bar_2'+str(i)].SetLineWidth(2)
           h[detector+'bar_2'+str(i)].SetStats(0)
       h[detector+'bar_20'].Draw()
       h[detector+'bar_21'].Draw('same')
       h[detector+'bar_22'].Draw('same')
       h[detector+'bar_23'].Draw('same')
       h[detector+'bar_24'].Draw('same')
       h[detector+'lbar2']=ROOT.TLegend(0.6,0.6,0.99,0.99)
       for i in range(5): 
            h[detector+'lbar2'].AddEntry(h[detector+'bar_2'+str(i)],'plane '+str(i+1),"f")
            h[detector+'lbar2'].Draw()
       for i in range(7): 
            h[detector+'hit_3'+str(i)].SetLineColor(colours[i])
            h[detector+'hit_3'+str(i)].SetLineWidth(2)
            h[detector+'hit_3'+str(i)].SetStats(0)
       h[detector+'hit_30'].Draw()
       for i in range(1,7):
           h[detector+'hit_3'+str(i)].Draw('same')
       h[detector+'lbar3']=ROOT.TLegend(0.6,0.6,0.99,0.99)
       for i in range(7): 
           h[detector+'lbar3'].AddEntry(h[detector+'hit_3'+str(i)],'plane '+str(i+1),"f")
           h[detector+'lbar3'].Draw()

       ut.bookCanvas(h,detector+'LR',' ',1800,900,3,2)
       for i in range(1,4):
          h[detector+'LR'].cd(i)
          h[detector+'leftvsright_'+str(i)].Draw('textBox')
          h[detector+'LR'].cd(i+3)
          h[detector+'leftvsright_signal_'+str(i)].SetMaximum(h[detector+'leftvsright_signal_'+str(i)].GetBinContent(10,10))
          h[detector+'leftvsright_signal_'+str(i)].Draw('colz')

       ut.bookCanvas(h,detector+'LRinEff',' ',1800,450,3,1)
       for s in range(1,4):
           h[detector+'lLRinEff'+str(s)]=ROOT.TLegend(0.6,0.54,0.99,0.93)
           name = detector+'leftvsright_signal_'+str(s)
           h[name+'0Y'] = h[name].ProjectionY(name+'0Y',1,1)
           h[name+'0X'] = h[name].ProjectionX(name+'0X',1,1)
           h[name+'1X'] = h[name].ProjectionY(name+'1Y')
           h[name+'1Y'] = h[name].ProjectionX(name+'1X')
           tc = h[detector+'LRinEff'].cd(s)
           tc.SetLogy()
           h[name+'0X'].SetStats(0)
           h[name+'0Y'].SetStats(0)
           h[name+'1X'].SetStats(0)
           h[name+'1Y'].SetStats(0)
           h[name+'0X'].SetLineColor(ROOT.kRed)
           h[name+'0Y'].SetLineColor(ROOT.kGreen)
           h[name+'1X'].SetLineColor(ROOT.kMagenta)
           h[name+'1Y'].SetLineColor(ROOT.kCyan)
           h[name+'0X'].SetMaximum(max(h[name+'1X'].GetMaximum(),h[name+'1Y'].GetMaximum()))
           h[name+'0X'].Draw()
           h[name+'0Y'].Draw('same')
           h[name+'1X'].Draw('same')
           h[name+'1Y'].Draw('same')
   # Fill(Sleft,Sright)
           h[detector+'lLRinEff'+str(s)].AddEntry(h[name+'0X'],'left with no signal right',"f")
           h[detector+'lLRinEff'+str(s)].AddEntry(h[name+'0Y'],'right with no signal left',"f")
           h[detector+'lLRinEff'+str(s)].AddEntry(h[name+'1X'],'left all',"f")
           h[detector+'lLRinEff'+str(s)].AddEntry(h[name+'1Y'],'right all',"f")
           h[detector+'lLRinEff'+str(s)].Draw()

       for tag in ["","T"]:
        ut.bookCanvas(h,tag+'signalUSVeto',' ',1200,1600,3,7)
        s = 1
        l = 1
        for plane in range(2):
                for side in ['L','R','S']:
                   tc = h[tag+'signalUSVeto'].cd(l)
                   l+=1 
                   if side=='S': continue
                   h[detector+tag+'sig'+side+'_'+str( s*10+plane)].Draw()
        s=2
        for plane in range(5):
                for side in ['L','R','S']:
                   tc = h[tag+'signalUSVeto'].cd(l)
                   l+=1
                   h[detector+tag+'sig'+side+'_'+str( s*10+plane)].Draw()
        ut.bookCanvas(h,tag+'signalDS',' ',900,1600,2,7)
        s = 3
        l = 1
        for plane in range(7):
               for side in ['L','R']:
                  tc = h[tag+'signalDS'].cd(l)
                  l+=1
                  h[detector+tag+'sig'+side+'_'+str( s*10+plane)].Draw()
       ut.bookCanvas(h,detector+"chanbar",' ',1800,700,3,1)
       for s in self.M.systemAndPlanes:
            opt = ""
            if s==3: 
               ut.bookCanvas(h,sdict[s]+"chanbar",' ',1800,1800,12,15)
            else:   
               y = self.M.systemAndPlanes[s]
               ut.bookCanvas(h,sdict[s]+"chanbar",' ',1800,700,y,self.M.systemAndBars[s])
            h[sdict[s]+"chanbar"].cd(1)
            for l in range(self.M.systemAndPlanes[s]):
               if s==3 and (l==1 or l==3 or l==5 or l==6):continue
               maxN = 0
               for bar in range(self.M.systemAndBars[s]):
                   hname = detector+'chanmult_'+str(s*1000+100*l+bar)
                   nmax = h[hname].GetBinContent(h[hname].GetMaximumBin())
                   if nmax > maxN : maxN = nmax
               for bar in range(self.M.systemAndBars[s]):
                   hname = detector+'chanmult_'+str(s*1000+100*l+bar)
                   h[hname].SetStats(0)
                   # h[hname].SetMaximum(maxN*1.2)
                   h[detector+"chanbar"].cd(s)
                   h[hname].DrawClone(opt)
                   opt="same"
                   i = l+1 + (self.M.systemAndBars[s]-bar-1)*self.M.systemAndPlanes[s]
                   if s==3:
                        ix = bar//15 + 1 + (l//2)*4
                        iy = 15 - bar%15
                        i = ix+(iy-1)*12
                   h[sdict[s]+"chanbar"].cd(i)
                   h[hname].SetMaximum(h[hname].GetBinContent(h[hname].GetMaximumBin())*1.2)
                   h[hname].Draw()
                   
       for canvas in ['signalUSVeto','signalDS',detector+'LR','USBars',
                     "Vetochanbar","USchanbar","DSchanbar",'noise']:
              h[canvas].Update()
              self.M.myPrint(h[canvas],canvas,subdir='mufilter')
       for canvas in [detector+'hitmaps',detector+'Xhitmaps',detector+'barmaps']:
              for s in range(1,4):
                  h[canvas+sdict[s]].Update()
                  self.M.myPrint(h[canvas+sdict[s]],canvas+sdict[s],subdir='mufilter')

# tracking
       ut.bookCanvas(h,"muonDSTracks",' ',1200,1200,3,1)
       tc = h["muonDSTracks"].cd(1)
       h[detector+'slopes'].Draw('colz')
       tc = h["muonDSTracks"].cd(2)
       h[detector+'slopes'].ProjectionX("slopeX").Draw()
       tc = h["muonDSTracks"].cd(3)
       h[detector+'slopes'].ProjectionY("slopeY").Draw()
       self.M.myPrint(h["muonDSTracks"],"muonDSTrackdirection",subdir='mufilter')
       ut.bookCanvas(h,detector+'TtrackPos',"track position first state",1200,800,1,2)
       h[detector+'TtrackPos'].cd(1)
       rc = h[detector+'trackPosBeam'].Draw('colz')
       h[detector+'TtrackPos'].cd(2)
       rc = h[detector+'trackPos'].Draw('colz')
       self.M.myPrint(self.M.h[detector+'TtrackPos'],detector+'trackPos',subdir='mufilter')
# residuals
       ut.bookCanvas(h,detector+"residualsVsX",'residualsVsX ',1200,1200,2,7)
       ut.bookCanvas(h,detector+"residualsVsY",'residualsVsY ',1200,1200,2,7)
       ut.bookCanvas(h,detector+"residuals",'residuals',1200,1200,2,7)
# veto 2 H planes US 5 H planes DS 3 H planes  DS 4 V planes
       for p in ['resX_','resY_']:
          t = detector+"residualsVs"+p.replace('res','').replace('_','')
          i = 1
          for s in range(1,4):
             for l in range(7): 
                if s==1 and l>1: continue
                if s==2 and l>4: continue
                tc = h[t].cd(i)
                hname = detector+p+sdict[s]+str(s*10+l)
                h[hname].Draw('colz')
                if p.find('X')>0:
                    tc = h[detector+"residuals"].cd(i)
                    h[hname+'proj']=h[hname].ProjectionX(hname+'proj')
                    rc = h[hname+'proj'].Fit('gaus','SQ')
                    fitResult = rc.Get()
                    if fitResult: 
                        tc.Update()
                        stats = h[hname+'proj'].FindObject('stats')
                        stats.SetOptFit(1111111)
                        stats.SetX1NDC(0.68)
                        stats.SetY1NDC(0.31)
                        stats.SetX2NDC(0.98)
                        stats.SetY2NDC(0.94)
                        h[hname+'proj'].Draw()
                i+=1
       self.M.myPrint(self.M.h[detector+'residualsVsX'],detector+'residualsVsX',subdir='mufilter')
       self.M.myPrint(self.M.h[detector+'residualsVsY'],detector+'residualsVsY',subdir='mufilter')
       self.M.myPrint(self.M.h[detector+'residuals'],detector+'residuals',subdir='mufilter')

class Mufi_largeVSsmall(ROOT.FairTask):
   """
    make correlation plots of small and large sipms for US and Veto"
   """
   def Init(self,options,monitor):
       self.M = monitor
       sdict = self.M.sdict
       h = self.M.h
       run = ROOT.FairRunAna.Instance()
       for S in [1,2]:
           for l in range(monitor.systemAndPlanes[S]):
              ut.bookHist(h,'SVSl_'+str(l),'QDC large vs small sum',200,0.,200.,200,0.,200.)
              ut.bookHist(h,'sVSl_'+str(l),'QDC large vs small average',200,0.,200.,200,0.,200.)
              for side in ['L','R']:
                   for i1 in range(7):
                      for i2 in range(i1+1,8):
                         tag=''
                         if S==2 and monitor.smallSiPMchannel(i1): tag = 's'+str(i1)
                         else:                              tag = 'l'+str(i1)
                         if S==2 and monitor.smallSiPMchannel(i2): tag += 's'+str(i2)
                         else:                              tag += 'l'+str(i2)
                         ut.bookHist(h,sdict[S]+'cor'+tag+'_'+side+str(l),'QDC channel i vs channel j',200,0.,200.,200,0.,200.)
                         for bar in range(monitor.systemAndBars[S]):
                              ut.bookHist(h,sdict[S]+'cor'+tag+'_'+side+str(l)+str(bar),'QDC channel i vs channel j',200,0.,200.,200,0.,200.)

   def ExecuteEvent(self,event):
      M = self.M
      h = self.M.h
      sdict = self.M.sdict
      for aHit in event.Digi_MuFilterHits:
          if not aHit.isValid(): continue
          detID = aHit.GetDetectorID()
          s = detID//10000
          bar = (detID%1000)
          if s>2: continue
          l  = (detID%10000)//1000  # plane number
          sumL,sumS,SumL,SumS = 0,0,0,0
          allChannels = M.map2Dict(aHit,'GetAllSignals',mask=False)
          nS = 0
          nL = 0
          for c in allChannels:
              if s==2 and self.M.smallSiPMchannel(c) : 
                  sumS+= allChannels[c]
                  nS += 1
              else:                                              
                  sumL+= allChannels[c]
                  nL+=1
          if nL>0: SumL=sumL/nL
          if nS>0: SumS=sumS/nS
          rc = h['sVSl_'+str(l)].Fill(SumS,SumL)
          rc = h['SVSl_'+str(l)].Fill(sumS/4.,sumL/12.)
#
          for side in ['L','R']:
            offset = 0
            if side=='R': offset = 8
            for i1 in range(offset,offset+7):
               if not i1 in allChannels: continue
               qdc1 = allChannels[i1]
               for i2 in range(i1+1,offset+8):
                 if not (i2) in allChannels: continue
                 if s==2 and self.M.smallSiPMchannel(i1): tag = 's'+str(i1-offset)
                 else: tag = 'l'+str(i1-offset)
                 if s==2 and self.M.smallSiPMchannel(i2): tag += 's'+str(i2-offset)
                 else: tag += 'l'+str(i2-offset)
                 qdc2 = allChannels[i2]
                 rc = h[sdict[s]+'cor'+tag+'_'+side+str(l)].Fill(qdc1,qdc2)
                 rc = h[sdict[s]+'cor'+tag+'_'+side+str(l)+str(bar)].Fill(qdc1,qdc2)
          allChannels.clear()

   def Plot(self):
       h = self.M.h
       sdict = self.M.sdict
       systemAndPlanes = self.M.systemAndPlanes
       ut.bookCanvas(h,'TSL','',1800,1400,3,2)
       ut.bookCanvas(h,'STSL','',1800,1400,3,2)
       S=2
       for l in range(systemAndPlanes[S]):
          tc = h['TSL'].cd(l+1)
          tc.SetLogz(1)
          aHist = h['sVSl_'+str(l)]
          aHist.SetTitle(';small SiPM QCD av:large SiPM QCD av')
          nmax = aHist.GetBinContent(aHist.GetMaximumBin())
          aHist.SetMaximum( 0.1*nmax )
          tc = h['sVSl_'+str(l)].Draw('colz')
       self.M.myPrint(h['TSL'],"largeSiPMvsSmallSiPM",subdir='mufilter')
       for l in range(systemAndPlanes[S]):
          tc = h['STSL'].cd(l+1)
          tc.SetLogz(1)
          aHist = h['SVSl_'+str(l)]
          aHist.SetTitle(';small SiPM QCD sum/2:large SiPM QCD sum/6')
          nmax = aHist.GetBinContent(aHist.GetMaximumBin())
          aHist.SetMaximum( 0.1*nmax )
          tc = h['SVSl_'+str(l)].Draw('colz')
       self.M.myPrint(h['STSL'],"SumlargeSiPMvsSmallSiPM",subdir='mufilter')
       for S in [1,2]:
         for l in range(systemAndPlanes[S]):
          for side in ['L','R']:
             ut.bookCanvas(h,sdict[S]+'cor'+side+str(l),'',1800,1400,7,4)
             k=1
             for i1 in range(7):
                for i2 in range(i1+1,8):
                  tag=''
                  if S==2 and self.M.smallSiPMchannel(i1): tag = 's'+str(i1)
                  else:                              tag = 'l'+str(i1)
                  if S==2 and self.M.smallSiPMchannel(i2): tag += 's'+str(i2)
                  else:                              tag += 'l'+str(i2)
                  tc = h[sdict[S]+'cor'+side+str(l)].cd(k)
                  for bar in range(self.M.systemAndBars[S]):
                      if bar == 0: h[sdict[S]+'cor'+tag+'_'+side+str(l)+str(bar)].Draw('colz')
                      else: h[sdict[S]+'cor'+tag+'_'+side+str(l)+str(bar)].Draw('colzsame')
                  k+=1
             self.M.myPrint(h[sdict[S]+'cor'+side+str(l)],'QDCcor'+side+str(l),subdir='mufilter')
