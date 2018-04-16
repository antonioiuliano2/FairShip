//SciFiHit class, derived from muonHit class (on date 5 April 2018)

#include "SciFiHit.h"
#include "SpectrometerPoint.h"
#include "TVector3.h"
//
#include "TGeoManager.h"
#include "TGeoBBox.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "TRandom3.h"

using std::cout;
using std::endl;

bool SciFiHit::onlyOnce=false;
const Double_t TimeResSigma = 0.5; // ns

//Double_t speedOfLight = TMath::C() *100./1000000000.0 ; // from m/sec to cm/ns
// -----   Default constructor   -------------------------------------------
SciFiHit::SciFiHit()
  : ShipHit()
{
}
// -----   Standard constructor   ------------------------------------------
SciFiHit::SciFiHit(Int_t detID, Float_t digi, Bool_t isV)
  : ShipHit(detID,digi) 
{
  if (!onlyOnce) {
    Init();
    onlyOnce = true;
  }
  //
  SetDigi(digi);
  setValidity(isV);
}
// -----   constructor from muonPoint   ------------------------------------------
SciFiHit::SciFiHit(SpectrometerPoint* p, Double_t t0)
  : ShipHit()
{
  if (!onlyOnce) {
    Init();
    onlyOnce = true;
  }
//
  TVector3 truePosition = TVector3( p->GetX(), p->GetY(),p->GetZ());
  fdigi = t0 + p->GetTime(); // + drift time,  propagation inside tile + tdc    
  SetDetectorID(DetIDfromXYZ(truePosition));
  SetDigi(SetMuonTimeRes(fdigi));
}
// ----
Int_t SciFiHit::DetIDfromXYZ(TVector3 p)
{  
    // needs some code to produce a unique detector ID
  Int_t detID = 1;
  return detID;
}
// ----
TVector3 SciFiHit::XYZfromDetID(Int_t dID)
{
    TVector3 p = TVector3(0,0,0);
// The center of the tile XYZ coordinates are returned
    return p;
//
}
// ----
void SciFiHit::Init()
{
  //initialization
}
// -------------------------------------------------------------------------
Double_t SciFiHit::SetMuonTimeRes(Double_t mcTime) {
//
  TRandom3 *rand = new TRandom3(0);
  Double_t cTime = rand->Gaus(mcTime,TimeResSigma);
  delete rand;
  return cTime;
}
void SciFiHit::Print() const {
//
  cout << "-I- SciFiHit: hit " << " in SciFi " << fDetectorID << endl;
  cout << "  TDC " << fdigi << " ns" << endl;

}
void SciFiHit::setValidity(Bool_t isV){hisV = isV;}
// -----   Destructor   ----------------------------------------------------
SciFiHit::~SciFiHit() { }
// -------------------------------------------------------------------------

ClassImp(SciFiHit)
