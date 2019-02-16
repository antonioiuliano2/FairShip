#include "UpstreamTagger.h"
#include "UpstreamTaggerPoint.h"
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

UpstreamTagger::UpstreamTagger()
  : FairDetector("UpstreamTagger",kTRUE, ktauRpc),
    fTrackID(-1),
    fPdgCode(),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fUpstreamTaggerPointCollection(new TClonesArray("UpstreamTaggerPoint"))
{
}

UpstreamTagger::UpstreamTagger(const char* name, const Double_t Zcenter,Bool_t Active,const char* Title)
  : FairDetector(name, Active, ktauRpc),
    fTrackID(-1),
    fPdgCode(),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fUpstreamTaggerPointCollection(new TClonesArray("UpstreamTaggerPoint"))
{
  fZcenter = Zcenter;
}

void UpstreamTagger::SetDesign(Int_t Design)
{  
  fDesign = Design;
  cout <<" Mag Spectro Design "<< fDesign<<endl;
}

void UpstreamTagger::SetTotDimensions(Double_t X, Double_t Y, Double_t Z)
{  
  fXtot = X;
  fYtot = Y;
  fZtot = Z;
}

void UpstreamTagger::SetFeDimensions(Double_t X, Double_t Y, Double_t Z)
{  
  fXFe = X;
  fYFe = Y;
  fZFe = Z;
}

void UpstreamTagger::SetRpcDimensions(Double_t X, Double_t Y, Double_t Z)
{  
  fXRpc = X;
  fYRpc = Y;
  fZRpc = Z;
}

void UpstreamTagger::SetRpcGasDimensions(Double_t X, Double_t Y, Double_t Z)
{  
  fXGas = X;
  fYGas = Y;
  fZGas = Z;
}

void UpstreamTagger::SetRpcPETDimensions(Double_t X, Double_t Y, Double_t Z)
{  
  fXPet = X;
  fYPet = Y;
  fZPet = Z;
}

void UpstreamTagger::SetRpcElectrodeDimensions(Double_t X, Double_t Y, Double_t Z)
{  
  fXEle = X;
  fYEle = Y;
  fZEle = Z;
}

void UpstreamTagger::SetRpcStripDimensions(Double_t X, Double_t Y, Double_t Z)
{  
  fXStrip = X;
  fYStrip = Y;
  fZStrip = Z;
}


void UpstreamTagger::SetGapDownstream(Double_t Gap)
{
  fGapDown = Gap;
}

void UpstreamTagger::SetGapMiddle(Double_t Gap)
{
  fGapMiddle = Gap;
}

void UpstreamTagger::SetZDimensionArm(Double_t Z)
{
  fZArm = Z;
}

void UpstreamTagger::SetNFeInArm(Int_t N)
{
  fNFe = N;
}

void UpstreamTagger::SetNRpcInArm(Int_t N)
{
  fNRpc = N;
}

void UpstreamTagger::SetPillarDimensions(Double_t X, Double_t Y, Double_t Z)
{
  fPillarX = X;
  fPillarY = Y;
  fPillarZ = Z;
}


UpstreamTagger::~UpstreamTagger()
{
  if (fUpstreamTaggerPointCollection) {
    fUpstreamTaggerPointCollection->Delete();
    delete fUpstreamTaggerPointCollection;
  }
}

void UpstreamTagger::Initialize()
{
  FairDetector::Initialize();
}

// -----   Private method InitMedium 
Int_t UpstreamTagger::InitMedium(const char* name)
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

void UpstreamTagger::ConstructGeometry()
{
  //getting tTauNuDet
  TGeoVolume *tTauNuDet = gGeoManager->GetVolume("tTauNuDet");


  InitMedium("RPCgas");
  TGeoMedium *RPCmat =gGeoManager->GetMedium("RPCgas");
   
  InitMedium("vacuum");
  TGeoMedium *vacuum =gGeoManager->GetMedium("vacuum");
    
  InitMedium("Bakelite");
  TGeoMedium *bakelite =gGeoManager->GetMedium("Bakelite");  

  InitMedium("iron");
  TGeoMedium *Iron =gGeoManager->GetMedium("iron");

  InitMedium("steel");
  TGeoMedium *Steel =gGeoManager->GetMedium("steel");
    
  InitMedium("copper");
  TGeoMedium *Cu =gGeoManager->GetMedium("copper");
    
  Double_t d = 0;

  if(fDesign!=3)
    {
     cout<<"Upstream Tagger: Upstream Tagger is defined only starting from Design number 3"<<endl;
    }
  if(fDesign==3)
    {
      //container for the Tagger volumes 
      TGeoBBox *TaggerBox = new TGeoBBox("TaggerBox", fXtot/2, fYtot/2, (2*fZFe+3*fZRpc)/2);    
      TGeoVolume *volTaggerBox = new TGeoVolume("volUpstreamTagger", TaggerBox, vacuum);

      tTauNuDet->AddNode(volTaggerBox, 1, new TGeoTranslation(0,0,fZcenter));
      //passive layers
      TGeoBBox *TaggerIronLayer = new TGeoBBox("TaggerIron",fXFe/2, fYFe/2, fZFe/2);
      TGeoVolume *volTaggerIron = new TGeoVolume("volTaggerIron",TaggerIronLayer,Iron);
      volTaggerIron->SetLineColor(kGray);
      for(Int_t i = 0; i < fNFe; i++)
	{
	    volTaggerBox->AddNode(volTaggerIron, i+1, new TGeoTranslation(0, 0,-fZtot/2+i*fZFe+fZFe/2+(i+1)*fZRpc));
	}
      //active layers
      TGeoBBox *TaggerRpcContainer = new TGeoBBox("TaggerRpcContainer", fXRpc/2, fYRpc/2, fZRpc/2);
      TGeoVolume *volTaggerRpcContainer = new TGeoVolume("volTaggerRpcContainer",TaggerRpcContainer,vacuum);
  
      TGeoBBox *TaggerStrip = new TGeoBBox("TaggerStrip",fXStrip/2, fYStrip/2, fZStrip/2);
      TGeoVolume *volTaggerStrip = new TGeoVolume("volTaggerStrip",TaggerStrip,Cu);
      volTaggerStrip->SetLineColor(kRed);
      volTaggerRpcContainer->AddNode(volTaggerStrip,1,new TGeoTranslation (0,0,-3.25*mm));
      volTaggerRpcContainer->AddNode(volTaggerStrip,2,new TGeoTranslation (0,0,3.25*mm));
      TGeoBBox *TaggerPETinsulator = new TGeoBBox("TaggerPETinsulator", fXPet/2, fYPet/2, fZPet/2);
      TGeoVolume *volTaggerPETinsulator = new TGeoVolume("volTaggerPETinsulator", TaggerPETinsulator, bakelite);
      volTaggerPETinsulator->SetLineColor(kYellow);
      volTaggerRpcContainer->AddNode(volTaggerPETinsulator,1,new TGeoTranslation(0,0,-3.1*mm));
      volTaggerRpcContainer->AddNode(volTaggerPETinsulator,2,new TGeoTranslation(0,0, 3.1*mm));
      TGeoBBox *TaggerElectrode = new TGeoBBox("TaggerElectrode",fXEle/2, fYEle/2, fZEle/2);
      TGeoVolume *volTaggerElectrode = new TGeoVolume("volTaggerElectrode",TaggerElectrode,bakelite);
      volTaggerElectrode->SetLineColor(kGreen);
      volTaggerRpcContainer->AddNode(volTaggerElectrode,1,new TGeoTranslation(0,0,-2*mm));
      volTaggerRpcContainer->AddNode(volTaggerElectrode,2,new TGeoTranslation(0,0, 2*mm));
      TGeoBBox *TaggerRpcGas = new TGeoBBox("TaggerRpcGas", fXGas/2, fYGas/2, fZGas/2);
      TGeoVolume *volTaggerRpc = new TGeoVolume("volTaggerRpc",TaggerRpcGas,RPCmat);
      volTaggerRpc->SetLineColor(kCyan);
      volTaggerRpcContainer->AddNode(volTaggerRpc,1,new TGeoTranslation(0,0,0));
   
      AddSensitiveVolume(volTaggerRpc);
    
      for(Int_t i = 0; i < fNRpc; i++)
	{
	    volTaggerBox->AddNode(volTaggerRpcContainer, i+1,new TGeoTranslation(0, 0, -fZtot/2+i*fZFe + i*fZRpc +fZRpc/2));        
	}
    
     // TGeoBBox *PillarBox = new TGeoBBox(fPillarX/2,fPillarY/2, fPillarZ/2);
      //TGeoVolume *PillarVol = new TGeoVolume("TaggerPillar",PillarBox,Steel);
      //Pillar1Vol->SetLineColor(kGreen+3);

     // tTauNuDet->AddNode(PillarVol,1, new TGeoTranslation(-fXtot/2+fPillarX/2,-fYtot/2-fPillarY/2,fZcenter-fZtot/2+fPillarZ/2));
     // tTauNuDet->AddNode(PillarVol,2, new TGeoTranslation(fXtot/2-fPillarX/2,-fYtot/2-fPillarY/2,fZcenter-fZtot/2 +fPillarZ/2));
    }

}


Bool_t  UpstreamTagger::ProcessHits(FairVolume* vol)
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
    fVolumeID = vol->getMCid();
    if (fELoss == 0. ) { return kFALSE; }
    TParticle* p=gMC->GetStack()->GetCurrentTrack();
    Int_t pdgCode = p->GetPdgCode();
    Int_t detID=0;
    gMC->CurrentVolID(detID);

    // cout<< "detID = " << detID << endl;
    Int_t MaxLevel = gGeoManager->GetLevel();
    const Int_t MaxL = MaxLevel;
    //cout << "MaxLevel = " << MaxL << endl;
    //cout << gMC->CurrentVolPath()<< endl;
    Int_t NRpc =0;
    const char *name;
    name = gMC->CurrentVolName();
    //cout << name << endl;
    Int_t motherID = gGeoManager->GetMother(0)->GetNumber();
    const char *mumname = gMC->CurrentVolOffName(0);
    //cout<<mumname<<"   "<< motherID<<endl;
    detID = motherID;
    //cout<< "detID = " << detID << endl;
    //cout<<endl;
    fVolumeID = detID;

    TLorentzVector Pos; 
    gMC->TrackPosition(Pos); 
    Double_t xmean = (fPos.X()+Pos.X())/2. ;      
    Double_t ymean = (fPos.Y()+Pos.Y())/2. ;      
    Double_t zmean = (fPos.Z()+Pos.Z())/2. ;     
     
    AddHit(fTrackID, fVolumeID, TVector3(xmean, ymean,  zmean), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,fELoss, pdgCode);
        
    // Increment number of muon det points in TParticle
    ShipStack* stack = (ShipStack*) gMC->GetStack();
    stack->AddPoint(ktauRpc);
  }
    
  return kTRUE;
}

void UpstreamTagger::EndOfEvent()
{
  fUpstreamTaggerPointCollection->Clear();
}


void UpstreamTagger::Register()
{
    
  /** This will create a branch in the output tree called
      UpstreamTaggerPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */
    
  FairRootManager::Instance()->Register("UpstreamTaggerPoint", "UpstreamTagger",
					fUpstreamTaggerPointCollection, kTRUE);
}

TClonesArray* UpstreamTagger::GetCollection(Int_t iColl) const
{
  if (iColl == 0) { return fUpstreamTaggerPointCollection; }
  else { return NULL; }
}

void UpstreamTagger::Reset()
{
  fUpstreamTaggerPointCollection->Clear();
}


UpstreamTaggerPoint* UpstreamTagger::AddHit(Int_t trackID, Int_t detID,
					   TVector3 pos, TVector3 mom,
					   Double_t time, Double_t length,
					   Double_t eLoss, Int_t pdgCode)

{
  TClonesArray& clref = *fUpstreamTaggerPointCollection;
  Int_t size = clref.GetEntriesFast();
  //cout << "UpstreamTagger hit called"<< pos.z()<<endl;
  //    return new(clref[size]) UpstreamTaggerPoint(trackID, detID, pos, mom,time, length, eLoss, pdgCode,NArm, NRpc, NHpt);
  return new(clref[size]) UpstreamTaggerPoint(trackID, detID, pos, mom,time, length, eLoss, pdgCode);
}


ClassImp(UpstreamTagger)
