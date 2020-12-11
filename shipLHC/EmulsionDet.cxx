//
//  EmulsionDet.cxx
//  
//
//
//

#include "EmulsionDet.h"

#include "EmulsionDetPoint.h"

#include "TGeoManager.h"
#include "FairRun.h"                    // for FairRun
#include "FairRuntimeDb.h"              // for FairRuntimeDb
#include "TList.h"                      // for TListIter, TList (ptr only)
#include "TObjArray.h"                  // for TObjArray
#include "TString.h"                    // for TString

#include "TClonesArray.h"
#include "TVirtualMC.h"

#include "TGeoBBox.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"

#include "TParticle.h"
#include "TParticlePDG.h"
#include "TParticleClassPDG.h"
#include "TVirtualMCStack.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoTransform.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoBuilder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "ShipDetectorList.h"
#include "ShipUnit.h"
#include "ShipStack.h"

#include "TGeoUniformMagField.h"
#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream,etc
#include <string.h>

using std::cout;
using std::endl;

using namespace ShipUnit;

EmulsionDet::EmulsionDet()
: FairDetector("EmulsionDet", "",kTRUE),
  fTrackID(-1),
fVolumeID(-1),
fPos(),
fMom(),
fTime(-1.),
fLength(-1.),
fELoss(-1),
fEmulsionDetPointCollection(new TClonesArray("EmulsionDetPoint"))
{
}

EmulsionDet::EmulsionDet(const char* name, Bool_t Active,const char* Title)
: FairDetector(name, true, kEmulsionDet),
  fTrackID(-1),
fVolumeID(-1),
fPos(),
fMom(),
fTime(-1.),
fLength(-1.),
fELoss(-1),
fEmulsionDetPointCollection(new TClonesArray("EmulsionDetPoint"))
{
}

EmulsionDet::~EmulsionDet()
{
    if (fEmulsionDetPointCollection) {
        fEmulsionDetPointCollection->Delete();
        delete fEmulsionDetPointCollection;
    }
}

void EmulsionDet::Initialize()
{
    FairDetector::Initialize();
}

// -----   Private method InitMedium
Int_t EmulsionDet::InitMedium(const char* name)
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

void EmulsionDet::SetCenterZ(Double_t z)
{
  fCenterZ = z;
}

void EmulsionDet::SetDetectorDimension(Double_t xdim, Double_t ydim, Double_t zdim)
{
  XDimension = xdim;
  YDimension = ydim;
  ZDimension = zdim;
}

void EmulsionDet::SetNumberWalls(Double_t wall)
{
  fNWall = wall;
}

void EmulsionDet::SetNumberTargets(Int_t target)
{
  fNTarget = target;
}

void EmulsionDet::SetTargetWallDimension(Double_t WallXDim_, Double_t WallYDim_, Double_t WallZDim_)
{
  WallXDim = WallXDim_;
  WallYDim = WallYDim_;
  WallZDim = WallZDim_;
}


void EmulsionDet::SetEmulsionParam(Double_t EmTh, Double_t EmX, Double_t EmY, Double_t PBTh, Double_t EPlW,Double_t PassiveTh, Double_t AllPW)
{
  EmulsionThickness = EmTh;
  EmulsionX = EmX;
  EmulsionY = EmY;
  PlasticBaseThickness = PBTh;
  EmPlateWidth = EPlW;
  PassiveThickness = PassiveTh;
  AllPlateWidth = AllPW;
}


void EmulsionDet::SetBrickParam(Double_t BrX, Double_t BrY, Double_t BrZ, Double_t BrPackX, Double_t BrPackY, Double_t BrPackZ, Int_t number_of_plates_)
{
  BrickPackageX = BrPackX;
  BrickPackageY = BrPackY;
  BrickPackageZ = BrPackZ;
  BrickX = BrX;
  BrickY = BrY;
  BrickZ = BrZ;
  number_of_plates = number_of_plates_;
}

void EmulsionDet::SetTTzdimension(Double_t TTZ)
{
 TTrackerZ = TTZ;
}

void EmulsionDet::ConstructGeometry()
{   
	TGeoVolume *top=gGeoManager->GetTopVolume();
	if(top) cout<<" top volume found! "<<endl;
	 gGeoManager->SetVisLevel(10);

  cout<< "fCenterZ:   "<<fCenterZ <<endl;
  cout<< "    X: "<< XDimension<< "  "<< YDimension<<"   Z: "<<ZDimension<<endl;
	cout<<"  BrickX: "<< BrickX<<"  Y: "<< BrickY<< "  Z: "<< BrickZ<<endl;
	//Materials 
	InitMedium("vacuum");
	TGeoMedium *vacuum =gGeoManager->GetMedium("vacuum");
	
	InitMedium("air");
	TGeoMedium *air =gGeoManager->GetMedium("air");

	InitMedium("iron");
	TGeoMedium *Fe =gGeoManager->GetMedium("iron");

	InitMedium("CoilAluminium");
	TGeoMedium *Al  = gGeoManager->GetMedium("CoilAluminium");

	InitMedium("CoilCopper");
	TGeoMedium *Cu  = gGeoManager->GetMedium("CoilCopper");

	InitMedium("PlasticBase");
	TGeoMedium *PBase =gGeoManager->GetMedium("PlasticBase");

	InitMedium("NuclearEmulsion");
	TGeoMedium *NEmu =gGeoManager->GetMedium("NuclearEmulsion");
	
	TGeoMaterial *NEmuMat = NEmu->GetMaterial(); //I need the materials to build the mixture
	TGeoMaterial *PBaseMat = PBase->GetMaterial();

	TGeoMixture * emufilmmixture = new TGeoMixture("EmulsionFilmMixture", 2.00); // two nuclear emulsions separated by the plastic base
	Double_t frac_emu = NEmuMat->GetDensity() * 2 * EmulsionThickness /(NEmuMat->GetDensity() * 2 * EmulsionThickness + PBaseMat->GetDensity() * PlasticBaseThickness);

	emufilmmixture->AddElement(NEmuMat,frac_emu);
	emufilmmixture->AddElement(PBaseMat,1. - frac_emu);

	TGeoMedium *Emufilm = new TGeoMedium("EmulsionFilm",100,emufilmmixture);

	InitMedium("WNi");
	TGeoMedium *tungsten = gGeoManager->GetMedium("WNi");


	Int_t NPlates = number_of_plates; //Number of doublets emulsion + Pb
	cout<< " Number of plates: "<<number_of_plates<<endl;


	//TGeoVolume *top = gGeoManager->MakeBox("Top",vacuum,10.,10.,10.);
	//gGeoManager->SetTopVolume(top);

	//Definition of the target box containing emulsion bricks + target trackers (TT) 
	TGeoBBox *TargetBox = new TGeoBBox("TargetBox",XDimension/2, YDimension/2, ZDimension/2);
	TGeoVolume *volTarget = new TGeoVolume("volTarget",TargetBox, air);

	//
	//  //Volumes definition
	//    //

	//Brick
	TGeoBBox *Brick = new TGeoBBox("brick", BrickX/2, BrickY/2, BrickZ/2);
	TGeoVolume *volBrick = new TGeoVolume("Brick",Brick,air);
	volBrick->SetLineColor(kCyan);
	volBrick->SetTransparency(1);

	TGeoBBox *Passive = new TGeoBBox("Passive", EmulsionX/2, EmulsionY/2, PassiveThickness/2);
	TGeoVolume *volPassive = new TGeoVolume("volPassive",Passive,tungsten);
	volPassive->SetTransparency(1);
	volPassive->SetLineColor(kGray);

	for(Int_t n=0; n<NPlates; n++)
	{
		volBrick->AddNode(volPassive, n, new TGeoTranslation(0,0,-BrickZ/2+ EmPlateWidth + PassiveThickness/2 + n*AllPlateWidth)); //LEAD
	}

	TGeoBBox *EmulsionFilm = new TGeoBBox("EmulsionFilm", EmulsionX/2, EmulsionY/2, EmPlateWidth/2);
	TGeoVolume *volEmulsionFilm = new TGeoVolume("Emulsion",EmulsionFilm,Emufilm); //TOP
	volEmulsionFilm->SetLineColor(kBlue);
	AddSensitiveVolume(volEmulsionFilm);
	for(Int_t n=0; n<NPlates+1; n++)
	{
		volBrick->AddNode(volEmulsionFilm, n, new TGeoTranslation(0,0,-BrickZ/2+ EmPlateWidth/2 + n*AllPlateWidth));
	}

	volBrick->SetVisibility(kTRUE);

	top->AddNode(volTarget,1,new TGeoTranslation(ShiftX+XDimension/2,ShiftY+YDimension/2,fCenterZ));
	cout<<ShiftX<<"  "<<ShiftY+YDimension/2<<"  "<<fCenterZ<<endl; 
	
	TGeoVolumeAssembly *volWall = new TGeoVolumeAssembly("Wall");

	//Walls
	Double_t d_cl_z = - ZDimension/2 + TTrackerZ;
	Double_t d_tt = -ZDimension/2 + TTrackerZ/2;

	for(int l = 0; l < fNWall; l++)
	{
		volTarget->AddNode(volBrick,l,new TGeoTranslation(0, 0, d_cl_z +BrickZ/2));
		d_cl_z += BrickZ + TTrackerZ;
	}
}





Bool_t  EmulsionDet::ProcessHits(FairVolume* vol)
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
    
    // Create EmulsionDetPoint at exit of active volume
    if ( gMC->IsTrackExiting()    ||
        gMC->IsTrackStop()       ||
        gMC->IsTrackDisappeared()   ) {
        fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();
	Int_t detID=0;
	gMC->CurrentVolID(detID);

	if (fVolumeID == detID) {
	  return kTRUE; }
	fVolumeID = detID;

	gGeoManager->PrintOverlaps();		
	
	if (fELoss == 0. ) { return kFALSE; }
        TParticle* p=gMC->GetStack()->GetCurrentTrack();
	Int_t pdgCode = p->GetPdgCode();
	
        TLorentzVector Pos; 
        gMC->TrackPosition(Pos); 
        Double_t xmean = (fPos.X()+Pos.X())/2. ;      
        Double_t ymean = (fPos.Y()+Pos.Y())/2. ;      
        Double_t zmean = (fPos.Z()+Pos.Z())/2. ;     
        
	
	AddHit(fTrackID,fVolumeID, TVector3(xmean, ymean,  zmean),
               TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,
               fELoss, pdgCode);
	
        // Increment number of muon det points in TParticle
        ShipStack* stack = (ShipStack*) gMC->GetStack();
        stack->AddPoint(kEmulsionDet);
    }
    
    return kTRUE;
}



void EmulsionDet::EndOfEvent()
{
    fEmulsionDetPointCollection->Clear();
}


void EmulsionDet::Register()
{
    
    /** This will create a branch in the output tree called
     TargetPoint, setting the last parameter to kFALSE means:
     this collection will not be written to the file, it will exist
     only during the simulation.
     */
    
    FairRootManager::Instance()->Register("EmulsionDetPoint", "EmulsionDet",
                                          fEmulsionDetPointCollection, kTRUE);
}

TClonesArray* EmulsionDet::GetCollection(Int_t iColl) const
{
    if (iColl == 0) { return fEmulsionDetPointCollection; }
    else { return NULL; }
}

void EmulsionDet::Reset()
{
    fEmulsionDetPointCollection->Clear();
}


EmulsionDetPoint* EmulsionDet::AddHit(Int_t trackID,Int_t detID,
                           TVector3 pos, TVector3 mom,
                           Double_t time, Double_t length,
			    Double_t eLoss, Int_t pdgCode)
{
    TClonesArray& clref = *fEmulsionDetPointCollection;
    Int_t size = clref.GetEntriesFast();
    return new(clref[size]) EmulsionDetPoint(trackID,detID, pos, mom,
					time, length, eLoss, pdgCode);
}
ClassImp(EmulsionDet)








