//
//  EmuDESYTarget.cxx 
//  Target file: 6  different run configurations can be done, setting the number of the variable nrun.
//
//
//

#include "EmuDESYTarget.h"

#include "EmuDESYPoint.h"

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

#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream,etc
#include <string.h>

#include "TGeoTrd2.h"

using std::cout;
using std::endl;

using namespace ShipUnit;

EmuDESYTarget::EmuDESYTarget()
: FairDetector("EmuDESYTarget", "",kTRUE),
  fTrackID(-1),
fVolumeID(-1),
fPos(),
fMom(),
fTime(-1.),
fLength(-1.),
fELoss(-1),
fEmuDESYPointCollection(new TClonesArray("EmuDESYPoint"))
{
}

EmuDESYTarget::EmuDESYTarget(const char* name,const Double_t zEmuTarget, Bool_t Active,const char* Title)
: FairDetector(name, true, kEmuDESYTarget),
  fTrackID(-1),
fVolumeID(-1),
fPos(),
fMom(),
fTime(-1.),
fLength(-1.),
fELoss(-1),
fEmuDESYPointCollection(new TClonesArray("EmuDESYPoint"))
{
    zEmuTargetPosition = zEmuTarget;  
}

EmuDESYTarget::~EmuDESYTarget()
{
    if (fEmuDESYPointCollection) {
        fEmuDESYPointCollection->Delete();
        delete fEmuDESYPointCollection;
    }
}

void EmuDESYTarget::Initialize()
{
    FairDetector::Initialize();
}

void EmuDESYTarget::SetEmulsionParam(Double_t EmTh, Double_t EmX, Double_t EmY, Double_t PBTh, Double_t EPlW,Double_t PasSlabTh, Double_t AllPW)
{
    EmulsionThickness = EmTh;
    EmulsionX = EmX;
    EmulsionY = EmY;
    PlasticBaseThickness = PBTh;
    EmPlateWidth = EPlW;
    PassiveSlabThickness = PasSlabTh;
    AllPlateWidth = AllPW;
}

void EmuDESYTarget::SetBrickParam(Double_t BrX, Double_t BrY, Double_t BrZ, Double_t BrPackX, Double_t BrPackY, Double_t BrPackZ)
{
  BrickPackageX = BrPackX;
  BrickPackageY = BrPackY;
  BrickPackageZ = BrPackZ;
  BrickX = BrX;
  BrickY = BrY;
  BrickZ = BrZ;
}


void EmuDESYTarget::SetTargetParam(Double_t TX, Double_t TY, Double_t TZ){
  TargetX = TX; 
  TargetY = TY;
  TargetZ = TZ;
}


// -----   Private method InitMedium
Int_t EmuDESYTarget::InitMedium(const char* name)
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

void EmuDESYTarget::AddEmulsionFilm(Double_t zposition, Int_t nreplica, TGeoVolume * volTarget, TGeoVolume * volEmulsionFilm, TGeoVolume   * volEmulsionFilm2, TGeoVolume * volPlBase){
   //emulsion IDs now go from 1 to a maximum of 57 for top layers, from 10000 to a maximum of 10057 for bottom layers
	 volTarget->AddNode(volEmulsionFilm2, nreplica+10000, new TGeoTranslation(0,0,zposition + EmulsionThickness/2)); //BOTTOM
	 volTarget->AddNode(volEmulsionFilm, nreplica, new TGeoTranslation(0,0,zposition +3 * EmulsionThickness/2 +PlasticBaseThickness)); //TOP
	 volTarget->AddNode(volPlBase, nreplica, new TGeoTranslation(0,0, zposition + EmulsionThickness + PlasticBaseThickness/2));
}

void EmuDESYTarget::ConstructGeometry()
{
    InitMedium("NuclearEmulsion");
    TGeoMedium *NEmu = gGeoManager->GetMedium("NuclearEmulsion");

    InitMedium("PlasticBase");
    TGeoMedium *PBase = gGeoManager->GetMedium("PlasticBase");

    InitMedium("lead");
    TGeoMedium *lead = gGeoManager->GetMedium("lead");    
    
    TGeoVolume *top= gGeoManager->GetTopVolume(); 

    TGeoBBox *EmulsionFilm = new TGeoBBox("EmulsionFilm", EmulsionX/2, EmulsionY/2, EmulsionThickness/2);
    TGeoVolume *volEmulsionFilm = new TGeoVolume("Emulsion",EmulsionFilm,NEmu); //TOP
    TGeoVolume *volEmulsionFilm2 = new TGeoVolume("Emulsion2",EmulsionFilm,NEmu); //BOTTOM
    volEmulsionFilm->SetLineColor(kBlue);
    volEmulsionFilm2->SetLineColor(kBlue);
    AddSensitiveVolume(volEmulsionFilm);
    //AddSensitiveVolume(volEmulsionFilm2);
	
    TGeoBBox *PlBase = new TGeoBBox("PlBase", EmulsionX/2, EmulsionY/2, PlasticBaseThickness/2);
    TGeoVolume *volPlBase = new TGeoVolume("PlasticBase",PlBase,PBase);
    volPlBase->SetLineColor(kYellow-4);
      //begin brick part (July testbeam)
      //Int_t NPlates = 19; //Number of doublets emulsion + Pb (two interaction lengths for 3 mm lead slabs)
    Int_t NPlates = 28; //when we consider 1 mm lead slabs
    const Int_t NBricks = 1;        
    Double_t zPasLead = NPlates * PassiveSlabThickness;  //amount of passive layer
    
    TargetZ = NPlates * AllPlateWidth + EmPlateWidth; //CH1       

    TGeoVolumeAssembly *volTarget = new TGeoVolumeAssembly("volTarget");
    volTarget->SetLineColor(kCyan);
    volTarget->SetTransparency(1);
      
    top->AddNode(volTarget,1,new TGeoTranslation(0,0,zEmuTargetPosition-TargetZ/2)); //Box ends at origin           
 
    TGeoVolume *volPasLead = NULL;
    
    TGeoBBox *Passiveslab = new TGeoBBox("Passiveslab", EmulsionX/2, EmulsionY/2, PassiveSlabThickness/2);
    TGeoVolume *volPassiveslab = new TGeoVolume("volPassiveslab",Passiveslab,lead);
    
    volPassiveslab->SetTransparency(1);
    volPassiveslab->SetLineColor(kGray);
      
    Int_t nfilm = 1, nlead = 1, npassiveslab = 1;
    Double_t zpoint = -TargetZ/2;

	  //adding emulsions
    for(Int_t n=0; n<NPlates+1; n++)
	    {
	      AddEmulsionFilm(zpoint + n*AllPlateWidth, nfilm, volTarget, volEmulsionFilm, volEmulsionFilm2, volPlBase);
	      nfilm++;
	    }           
	 for(Int_t n=0; n<NPlates; n++) //adding 1 mm lead plates
	    {
              volTarget->AddNode(volPassiveslab, npassiveslab, new TGeoTranslation(0,0,zpoint + EmPlateWidth + PassiveSlabThickness/2 + n*AllPlateWidth));
              npassiveslab++;
	    }	
	 zpoint = zpoint + NPlates *AllPlateWidth + EmPlateWidth;
	}      
     
    





Bool_t  EmuDESYTarget::ProcessHits(FairVolume* vol)
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
    
    // Create EmuDESYPoint at exit of active volume
    if ( gMC->IsTrackExiting()    ||
        gMC->IsTrackStop()       ||
        gMC->IsTrackDisappeared()   ) {
        fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
        gMC->CurrentVolID(fVolumeID);

	//gGeoManager->PrintOverlaps();		
	
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
        stack->AddPoint(kEmuDESYTarget);
    }
    
    return kTRUE;
}



void EmuDESYTarget::EndOfEvent()
{
    fEmuDESYPointCollection->Clear();
}


void EmuDESYTarget::Register()
{
    
    /** This will create a branch in the output tree called
     TargetPoint, setting the last parameter to kFALSE means:
     this collection will not be written to the file, it will exist
     only during the simulation.
     */
    
    FairRootManager::Instance()->Register("EmuDESYPoint", "EmuDESYTarget",
                                          fEmuDESYPointCollection, kTRUE);
}

TClonesArray* EmuDESYTarget::GetCollection(Int_t iColl) const
{
    if (iColl == 0) { return fEmuDESYPointCollection; }
    else { return NULL; }
}

void EmuDESYTarget::Reset()
{
    fEmuDESYPointCollection->Clear();
}


EmuDESYPoint* EmuDESYTarget::AddHit(Int_t trackID,Int_t detID,
                           TVector3 pos, TVector3 mom,
                           Double_t time, Double_t length,
			    Double_t eLoss, Int_t pdgCode)
{
    TClonesArray& clref = *fEmuDESYPointCollection;
    Int_t size = clref.GetEntriesFast();
    return new(clref[size]) EmuDESYPoint(trackID,detID, pos, mom,
					time, length, eLoss, pdgCode);
}
ClassImp(EmuDESYTarget)








