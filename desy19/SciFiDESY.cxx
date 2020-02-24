// SciFiDESY.cxx
//  SciFiDESY, twelve scifi modules physically connected two by two.

#include "SciFiDESY.h"
//#include "MagneticSciFiDESY.h"
#include "SciFiDESYPoint.h"
#include "TGeoManager.h"
#include "FairRun.h"                    // for FairRun
#include "FairRuntimeDb.h"              // for FairRuntimeDb
#include <iosfwd>                    // for ostream
#include "TList.h"                      // for TListIter, TList (ptr only)
#include "TObjArray.h"                  // for TObjArray
#include "TString.h"                    // for TString
#include "TClonesArray.h"
#include "TVirtualMC.h"

#include "TGeoBBox.h"
#include "TGeoTrd1.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoArb8.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TParticle.h"
#include "TVector3.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "ShipDetectorList.h"
#include "ShipUnit.h"
#include "ShipStack.h"

#include "TGeoUniformMagField.h"
#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using std::cout;
using std::endl;
using namespace ShipUnit;

SciFiDESY::SciFiDESY()
  : FairDetector("HighPrecisionTrackers",kTRUE, kSciFiDESY),
  fTrackID(-1),
  fPdgCode(),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fSciFiDESYPointCollection(new TClonesArray("SciFiDESYPoint"))
{
}

SciFiDESY::SciFiDESY(const char* name, Bool_t Active,const char* Title)
  : FairDetector(name, Active, kSciFiDESY),
  fTrackID(-1),
  fPdgCode(),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fSciFiDESYPointCollection(new TClonesArray("SciFiDESYPoint"))
{
}

SciFiDESY::~SciFiDESY()
{
  if (fSciFiDESYPointCollection) {
    fSciFiDESYPointCollection->Delete();
    delete fSciFiDESYPointCollection;
  }
}

void SciFiDESY::Initialize()
{
  FairDetector::Initialize();
}

// -----   Private method InitMedium
Int_t SciFiDESY::InitMedium(const char* name)
{
  static FairGeoLoader *geoLoad=FairGeoLoader::Instance();
  static FairGeoInterface *geoFace=geoLoad->getGeoInterface();
  static FairGeoMedia *media=geoFace->getMedia();
  static FairGeoBuilder *geoBuild=geoLoad->getGeoBuilder();

  FairGeoMedium *ShipMedium=media->getMedium(name);

  if (!ShipMedium)
  {
    Fatal("InitMedium","Material %s not defined in media file.", name);
    return -1111;
  }
  TGeoMedium* medium=gGeoManager->GetMedium(name);
  if (medium!=NULL)
    return ShipMedium->getMediumIndex();
  return geoBuild->createMedium(ShipMedium);
}
//dimensions of SciFiDESY stations
void SciFiDESY::SetStationDimensions(Double_t SciFiDESYStationDX, Double_t SciFiDESYStationDY, Double_t SciFiDESYStationDZ)
{
  DimX = SciFiDESYStationDX;
  DimY = SciFiDESYStationDY;
  DimZ = SciFiDESYStationDZ;
}


void SciFiDESY::SetStationPositions(Double_t posz1, Double_t posz2)
{
  fZposSciFi1 = posz1;
  fZposSciFi2 = posz2;
}

//
void SciFiDESY::ConstructGeometry()
{

  InitMedium("air");
  TGeoMedium *air = gGeoManager->GetMedium("air");

  TGeoVolume *top = gGeoManager->GetTopVolume();

  TGeoBBox *SciFiDESY = new TGeoBBox("SciFiDESY", DimX/2, DimY/2, DimZ/2); //long along y
  TGeoVolume *volSciFiDESY = new TGeoVolume("volSciFiDESY",SciFiDESY,air);
  volSciFiDESY->SetLineColor(kRed);
  AddSensitiveVolume(volSciFiDESY);
  //two SciFis, after each brick
  top->AddNode(volSciFiDESY, 1, new TGeoTranslation(0,0,fZposSciFi1)); //compensation for the Node offset
  top->AddNode(volSciFiDESY, 2, new TGeoTranslation(0,0,fZposSciFi2)); //compensation for the Node offset

}

Bool_t  SciFiDESY::ProcessHits(FairVolume* vol)
{
  /** This method is called from the MC stepping */
  //Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
  }
  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create muonPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
      gMC->IsTrackStop()       ||
      gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();

    if (fELoss == 0. ) { return kFALSE; }
    TParticle* p=gMC->GetStack()->GetCurrentTrack();
    Int_t pdgCode = p->GetPdgCode();
    //Int_t fMotherID =p->GetFirstMother();
    gMC->CurrentVolID(fVolumeID);

    TLorentzVector Pos;
    gMC->TrackPosition(Pos);
    Double_t xmean = (fPos.X()+Pos.X())/2. ;
    Double_t ymean = (fPos.Y()+Pos.Y())/2. ;
    Double_t zmean = (fPos.Z()+Pos.Z())/2. ;

    AddHit(fTrackID, fVolumeID, TVector3(xmean, ymean,  zmean), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,fELoss, pdgCode);

    // Increment number of muon det points in TParticle
    ShipStack* stack = (ShipStack*) gMC->GetStack();
    stack->AddPoint(kSciFiDESY);
  }

  return kTRUE;
}

void SciFiDESY::EndOfEvent()
{
  fSciFiDESYPointCollection->Clear();
}


void SciFiDESY::Register()
{

  /** This will create a branch in the output tree called
    SciFiDESYPoint, setting the last parameter to kFALSE means:
    this collection will not be written to the file, it will exist
    only during the simulation.
    */

  FairRootManager::Instance()->Register("SciFiDESYPoint", "SciFiDESY",
      fSciFiDESYPointCollection, kTRUE);
}

// -----   Public method to Decode volume info  -------------------------------------------

TClonesArray* SciFiDESY::GetCollection(Int_t iColl) const
{
  if (iColl == 0) { return fSciFiDESYPointCollection; }
  else { return NULL; }
}

void SciFiDESY::Reset()
{
  fSciFiDESYPointCollection->Clear();
}


SciFiDESYPoint* SciFiDESY::AddHit(Int_t trackID, Int_t detID,
    TVector3 pos, TVector3 mom,
    Double_t time, Double_t length,
    Double_t eLoss, Int_t pdgCode)

{
  TClonesArray& clref = *fSciFiDESYPointCollection;
  Int_t size = clref.GetEntriesFast();

  return new(clref[size]) SciFiDESYPoint(trackID, detID, pos, mom,time, length, eLoss, pdgCode);
}


ClassImp(SciFiDESY)
