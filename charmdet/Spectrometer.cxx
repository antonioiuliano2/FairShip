// Spectrometer.cxx
// Magnetic Spectrometer, four tracking stations in a magnetic field.

#include "Spectrometer.h"
//#include "MagneticSpectrometer.h" 
#include "SpectrometerPoint.h"
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

Spectrometer::Spectrometer()
  : FairDetector("HighPrecisionTrackers",kTRUE, kSpectrometer),
    fTrackID(-1),
    fPdgCode(),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpectrometerPointCollection(new TClonesArray("SpectrometerPoint"))
{
}

Spectrometer::Spectrometer(const char* name, const Double_t DX, const Double_t DY, const Double_t DZ, Bool_t Active,const char* Title)
  : FairDetector(name, Active, kSpectrometer),
    fTrackID(-1),
    fPdgCode(),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpectrometerPointCollection(new TClonesArray("SpectrometerPoint"))
{ 
  DimX = DX;
  DimY = DY;
  DimZ = DZ;
}

Spectrometer::~Spectrometer()
{
    if (fSpectrometerPointCollection) {
        fSpectrometerPointCollection->Delete();
        delete fSpectrometerPointCollection;
    }
}

void Spectrometer::Initialize()
{
    FairDetector::Initialize();
}

// -----   Private method InitMedium 
Int_t Spectrometer::InitMedium(const char* name)
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

void Spectrometer::SetBoxParam(Double_t SX, Double_t SY, Double_t SZ, Double_t zBox)
{
  SBoxX = SX;
  SBoxY = SY;
  SBoxZ = SZ;
  zBoxPosition = zBox;
}

void Spectrometer::SetMagneticField(Double_t Bvalue)
{
 Bfield = Bvalue;
}

void Spectrometer::SetSiliconZ(Double_t SiliconZ)
{
  DimZSi = SiliconZ;
}

void Spectrometer::SetTransverseSizes(Double_t D1X, Double_t D1Y, Double_t D2X, Double_t D2Y, Double_t D3X, Double_t D3Y, Double_t D4X, Double_t D4Y){
  Dim1X = D1X;
  Dim1Y = D1Y;
  Dim2X = D2X;
  Dim2Y = D2Y;
  Dim3X = D3X;
  Dim3Y = D3Y;
  Dim4X = D4X;
  Dim4Y = D4Y;
}   

void Spectrometer::ChooseGeometry(bool issilicon)
{
  silicongeometry = issilicon;
}

//Methods for Goliath by Annarita
void Spectrometer::SetGoliathSizes(Double_t H, Double_t TS, Double_t LS, Double_t BasisH)
{
    LongitudinalSize = LS;
    TransversalSize = TS;
    Height = H;
    BasisHeight = BasisH;
}

void Spectrometer::SetCoilParameters(Double_t CoilR, Double_t UpCoilH, Double_t LowCoilH, Double_t CoilD)
{
    CoilRadius = CoilR;
    UpCoilHeight = UpCoilH;
    LowCoilHeight = LowCoilH;
    CoilDistance = CoilD;
}
//
void Spectrometer::ConstructGeometry()
{ 
    InitMedium("air");
  TGeoMedium *air = gGeoManager->GetMedium("air");

    InitMedium("iron");
    TGeoMedium *Fe =gGeoManager->GetMedium("iron");
    
    InitMedium("silicon");
    TGeoMedium *Silicon = gGeoManager->GetMedium("silicon");

    InitMedium("CoilCopper");
    TGeoMedium *Cu  = gGeoManager->GetMedium("CoilCopper");

    InitMedium("CoilAluminium");
    TGeoMedium *Al  = gGeoManager->GetMedium("CoilAluminium");

    InitMedium("TTmedium");
    TGeoMedium *TT  = gGeoManager->GetMedium("TTmedium");
    
    InitMedium("STTmix8020_2bar");
    TGeoMedium *sttmix8020_2bar   = gGeoManager->GetMedium("STTmix8020_2bar");
  
  TGeoVolume *top = gGeoManager->GetTopVolume();

  const Double_t MagneticField = Bfield;
  TGeoUniformMagField *magfield = new TGeoUniformMagField(0., MagneticField, 0.); //The magnetic field must be only in the air space between the stations

  TGeoBBox *ProvaBox = new TGeoBBox("ProvaBox", Dim4X/2, Dim4Y/2, SBoxZ/2);
  TGeoVolume *volProva = new TGeoVolume("volProva", ProvaBox, air);
  volProva->SetTransparency(1);

 // top->AddNode(volProva,1,new TGeoTranslation(0,0,zBoxPosition));
  
    TGeoBBox *SciFi1; //first declare, then define inside the if clause
    TGeoVolume *subvolSciFi1; 
    TGeoBBox *SciFi2;
    TGeoVolume *subvolSciFi2; 
  
    TGeoBBox *Pixel;
    TGeoVolume *volPixel;

    if (silicongeometry){
    Pixel = new TGeoBBox("Pixel", (Dim1X)/2, (Dim1Y)/2, DimZSi/2); 
    volPixel = new TGeoVolume("volPixel",Pixel,Silicon); 
    volPixel->SetLineColor(kBlue-5);
    AddSensitiveVolume(volPixel);
}
  else{
    SciFi1 = new TGeoBBox("SciFi1", (Dim1X)/2, (Dim1Y)/2, (4*DimZSi)/2); //the silicon blocks are united in single blocks
    subvolSciFi1 = new TGeoVolume("volSciFi1",SciFi1,Silicon);
    subvolSciFi1->SetLineColor(kBlue-5);
    AddSensitiveVolume(subvolSciFi1);
    

    SciFi2 = new TGeoBBox("SciFi2", (Dim2X)/2, (Dim2Y)/2, (2*DimZSi)/2);	
    subvolSciFi2 = new TGeoVolume("volSciFi2",SciFi2,Silicon);
    subvolSciFi2->SetLineColor(kBlue-5);
    AddSensitiveVolume(subvolSciFi2);
}  
    
    TGeoBBox *SciFi3 = new TGeoBBox("SciFi3", Dim3X/2, Dim3Y/2, DimZ/2); 
    TGeoVolume *subvolSciFi3 = new TGeoVolume("volSciFi3",SciFi3,sttmix8020_2bar);
    subvolSciFi3->SetLineColor(kBlue-5);
    AddSensitiveVolume(subvolSciFi3);
  		
    TGeoBBox *SciFi4 = new TGeoBBox("SciFi4", Dim4X/2, Dim4Y/2, DimZ/2);
    TGeoVolume *subvolSciFi4 = new TGeoVolume("volSciFi4",SciFi4,sttmix8020_2bar);
    subvolSciFi4->SetLineColor(kBlue-5);
    AddSensitiveVolume(subvolSciFi4);    

    Double_t z[4];
   if (silicongeometry){
    Double_t Sidist = 5*cm; //Distance between siliconDetectors

    z[0] = (DimZSi)/2.;
    z[1] = z[0] + 3 *DimZSi + 2 * Sidist;
    z[2] = z[1] + DimZ/2. + LongitudinalSize + 10 *cm;
    z[3] = z[1] + DimZ/2. + LongitudinalSize + 10* cm + 5*cm + DimZ;   

    for (int k = 0; k < 3; k++){ //3 silicon planes are the new detectors
      Double_t zreplica = 0 + k * Sidist * cm + DimZSi/2.;      
      top->AddNode(volPixel, 100 + k+1,  new TGeoTranslation(0,0, zBoxPosition -SBoxZ/2 + z[0] + zreplica)); //101, 102, 103
    }
}
   else{ //T1-T4 classical configuration
    z[0] = (4*DimZSi)/2.; 
    z[1] = (2*DimZSi)/2. +20*cm + 4*DimZSi;
    z[2] = DimZ/2. + LongitudinalSize +20*cm + 6 * DimZSi;
    z[3] = DimZ/2. + LongitudinalSize + 40*cm + 6 * DimZSi;
    volProva->AddNode(subvolSciFi1,1,new TGeoTranslation(0,0,-SBoxZ/2 + z[0]));
    volProva->AddNode(subvolSciFi2,2,new TGeoTranslation(0,0,-SBoxZ/2 + z[1])); //I need to recognize them with indexes
}
 //I am trying to activate only the first two slabs
    //volProva->AddNode(subvolSciFi3,3,new TGeoTranslation(0,0,-SBoxZ/2 + z[2]));
    //volProva->AddNode(subvolSciFi4,4,new TGeoTranslation(0,0,-SBoxZ/2 + z[3]));
    top->AddNode(subvolSciFi3,3,new TGeoTranslation(0,0,zBoxPosition-SBoxZ/2 + z[2]));
    top->AddNode(subvolSciFi4,4,new TGeoTranslation(0,0,zBoxPosition-SBoxZ/2 + z[3]));
}

Bool_t  Spectrometer::ProcessHits(FairVolume* vol)
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
	//Int_t fMotherID =p->GetFirstMother();
	Int_t detID=0;
	gMC->CurrentVolID(detID);

	if (fVolumeID == detID) {
	  return kTRUE; }
	fVolumeID = detID;

        TLorentzVector Pos; 
        gMC->TrackPosition(Pos); 
        Double_t xmean = (fPos.X()+Pos.X())/2. ;      
        Double_t ymean = (fPos.Y()+Pos.Y())/2. ;      
        Double_t zmean = (fPos.Z()+Pos.Z())/2. ;     

	AddHit(fTrackID, fVolumeID, TVector3(xmean, ymean,  zmean), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,fELoss, pdgCode);
        
        // Increment number of muon det points in TParticle
        ShipStack* stack = (ShipStack*) gMC->GetStack();
        stack->AddPoint(kSpectrometer);
    }
    
    return kTRUE;
}

void Spectrometer::EndOfEvent()
{
    fSpectrometerPointCollection->Clear();
}


void Spectrometer::Register()
{
    
    /** This will create a branch in the output tree called
     SpectrometerPoint, setting the last parameter to kFALSE means:
     this collection will not be written to the file, it will exist
     only during the simulation.
     */
    
    FairRootManager::Instance()->Register("SpectrometerPoint", "Spectrometer",
                                          fSpectrometerPointCollection, kTRUE);
}

// -----   Public method to Decode volume info  -------------------------------------------
// -----   returns hpt, arm, rpc numbers -----------------------------------
void Spectrometer::DecodeVolumeID(Int_t detID,int &nHPT)
{
  nHPT = detID;
}

TClonesArray* Spectrometer::GetCollection(Int_t iColl) const
{
    if (iColl == 0) { return fSpectrometerPointCollection; }
    else { return NULL; }
}

void Spectrometer::Reset()
{
    fSpectrometerPointCollection->Clear();
}


SpectrometerPoint* Spectrometer::AddHit(Int_t trackID, Int_t detID,
                        TVector3 pos, TVector3 mom,
                        Double_t time, Double_t length,
					    Double_t eLoss, Int_t pdgCode)

{
    TClonesArray& clref = *fSpectrometerPointCollection;
    Int_t size = clref.GetEntriesFast();

    return new(clref[size]) SpectrometerPoint(trackID, detID, pos, mom,time, length, eLoss, pdgCode);
}


ClassImp(Spectrometer)
