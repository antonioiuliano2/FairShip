//
//  MuFilter.cxx
//
//  by A.Buonaura
//

#include "MuFilter.h"
#include "MuFilterPoint.h"

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

MuFilter::MuFilter()
: FairDetector("MuonFilter", "",kTRUE),
  fTrackID(-1),
fVolumeID(-1),
fPos(),
fMom(),
fTime(-1.),
fLength(-1.),
fELoss(-1),
fMuFilterPointCollection(new TClonesArray("MuFilterPoint"))
{
}

MuFilter::MuFilter(const char* name, Bool_t Active,const char* Title)
: FairDetector(name, true, kMuFilter),
  fTrackID(-1),
fVolumeID(-1),
fPos(),
fMom(),
fTime(-1.),
fLength(-1.),
fELoss(-1),
fMuFilterPointCollection(new TClonesArray("MuFilterPoint"))
{
}

MuFilter::~MuFilter()
{
    if (fMuFilterPointCollection) {
        fMuFilterPointCollection->Delete();
        delete fMuFilterPointCollection;
    }
}

void MuFilter::SetIronBlockDimensions(Double_t x, Double_t y, Double_t z)
{
	fFeBlockX = x;
	fFeBlockY = y;
	fFeBlockZ = z;
}

void MuFilter::SetTimingPlanesDimensions(Double_t x, Double_t y, Double_t z)
{
	fTDetX = x;
	fTDetY = y;
	fTDetZ = z;
}

void MuFilter::SetMuFilterDimensions(Double_t x, Double_t y, Double_t z)
{	
	fMuFilterX = x;
	fMuFilterY = y;
	fMuFilterZ = z;
}

void MuFilter::SetNplanes(Int_t n)
{
	fNplanes = n;
}

void MuFilter::SetCenterZ(Double_t z)
{
	fCenterZ = z;
}

void MuFilter::SetXYDisplacement(Double_t x, Double_t y)
{
	fShiftX = x;
	fShiftY = y;
}

void MuFilter::SetYPlanesDisplacement(Double_t y)
{
	fShiftDY = y;
}

void MuFilter::Initialize()
{
	FairDetector::Initialize();
}

// -----  Private method InitMedium
Int_t MuFilter::InitMedium(const char* name)
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

void MuFilter::ConstructGeometry()
{
	TGeoVolume *top=gGeoManager->GetTopVolume();
	if(top) cout<<" top volume found! "<<endl;
	gGeoManager->SetVisLevel(10);

	//Materials 
	InitMedium("air");
	TGeoMedium *air =gGeoManager->GetMedium("air");

	InitMedium("iron");
	TGeoMedium *Fe =gGeoManager->GetMedium("iron");

	InitMedium("polyvinyltoluene");
	TGeoMedium *Scint =gGeoManager->GetMedium("polyvinyltoluene");

	//Definition of the box containing Fe Blocks + Timing Detector planes 
	TGeoBBox *MuFilterBox = new TGeoBBox("MuFilterBox",fMuFilterX/2,fMuFilterY/2,fMuFilterZ/2);
	TGeoVolume *volMuFilter = new TGeoVolume("volMuFilter",MuFilterBox,air);

	//Iron blocks volume definition
	TGeoBBox *FeBlockBox = new TGeoBBox("FeBlockBox",fFeBlockX/2, fFeBlockY/2, fFeBlockZ/2);
	TGeoVolume *volFeBlock = new TGeoVolume("volFeBlock",FeBlockBox,Fe);
	volFeBlock->SetLineColor(19);

	//Timing Detector planes definition
	TGeoBBox *TDetBox = new TGeoBBox("TDetBox",fTDetX/2,fTDetY/2,fTDetZ/2);
	TGeoVolume *volTDet = new TGeoVolume("volTDet",TDetBox,Scint);
	volTDet->SetLineColor(kRed+2);
	AddSensitiveVolume(volTDet);

	top->AddNode(volMuFilter,1,new TGeoTranslation(fShiftX,fShiftY+fMuFilterY/2,fCenterZ));
	cout<<fShiftX<<"  "<<fShiftY+fMuFilterY/2<<" "<<fCenterZ<<endl;

	Double_t dy = 0;
	for(Int_t l=0; l<fNplanes; l++)
	{
		if(l==0)
		{
			volMuFilter->AddNode(volFeBlock,l,new TGeoTranslation(0,fMuFilterY/2-fFeBlockY/2,-fMuFilterZ/2+fFeBlockZ/2+l*(fFeBlockZ+fTDetZ)));
			volMuFilter->AddNode(volTDet,l,new TGeoTranslation(0,fMuFilterY/2-fFeBlockY/2,-fMuFilterZ/2+fFeBlockZ+fTDetZ/2+l*(fFeBlockZ+fTDetZ)));
		}
		if(l==1||l==4)
			dy+=fShiftDY;
		if(l==2||l==3)
			dy+= fShiftDY/2;
		volMuFilter->AddNode(volFeBlock,l,new TGeoTranslation(0,fMuFilterY/2-fFeBlockY/2-dy,-fMuFilterZ/2+fFeBlockZ/2+l*(fFeBlockZ+fTDetZ)));
		volMuFilter->AddNode(volTDet,l,new TGeoTranslation(0,fMuFilterY/2-fFeBlockY/2-dy,-fMuFilterZ/2+fFeBlockZ+fTDetZ/2+l*(fFeBlockZ+fTDetZ)));
	}
}

Bool_t  MuFilter::ProcessHits(FairVolume* vol)
{
	/** This method is called from the MC stepping */
	//Set parameters at entrance of volume. Reset ELoss.
	if ( gMC->IsTrackEntering() ) 
	{
		fELoss  = 0.;
		fTime   = gMC->TrackTime() * 1.0e09;
		fLength = gMC->TrackLength();
		gMC->TrackPosition(fPos);
		gMC->TrackMomentum(fMom);
	}
	// Sum energy loss for all steps in the active volume
	fELoss += gMC->Edep();

	// Create MuFilterPoint at exit of active volume
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
		stack->AddPoint(kMuFilter);
	}   

	return kTRUE;
}

void MuFilter::EndOfEvent()
{
    fMuFilterPointCollection->Clear();
}


void MuFilter::Register()
{

    /** This will create a branch in the output tree called
 *      TargetPoint, setting the last parameter to kFALSE means:
 *           this collection will not be written to the file, it will exist
 *                only during the simulation.
 *                     */

    FairRootManager::Instance()->Register("MuFilterPoint", "MuFilter",
                                          fMuFilterPointCollection, kTRUE);
}

TClonesArray* MuFilter::GetCollection(Int_t iColl) const
{
    if (iColl == 0) { return fMuFilterPointCollection; }
    else { return NULL; }
}

void MuFilter::Reset()
{
    fMuFilterPointCollection->Clear();
}


MuFilterPoint* MuFilter::AddHit(Int_t trackID,Int_t detID,
                           TVector3 pos, TVector3 mom,
                           Double_t time, Double_t length,
                            Double_t eLoss, Int_t pdgCode)
{
    TClonesArray& clref = *fMuFilterPointCollection;
    Int_t size = clref.GetEntriesFast();
    return new(clref[size]) MuFilterPoint(trackID,detID, pos, mom,
                                        time, length, eLoss, pdgCode);
}
ClassImp(MuFilter)
