//
//  Target.cxx
//
//
//  Created by Annarita Buonaura on 17/01/15.
//
//

#include "Target.h"

#include "TargetPoint.h"

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
#include "TGeoTrd1.h"
#include "TGeoArb8.h"
#include "TGeoShapeAssembly.h"

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
#include "EmulsionMagnet.h"

#include "TGeoUniformMagField.h"
#include "TVector3.h"

#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream,etc
#include <string.h>

using std::cout;
using std::endl;

using namespace ShipUnit;

Target::Target()
  : FairDetector("Target", "",kTRUE),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fTargetPointCollection(new TClonesArray("TargetPoint"))
{
}

Target::Target(const char* name, const Double_t Ydist, Bool_t Active, const char* Title)
  : FairDetector(name, true, ktauTarget),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fTargetPointCollection(new TClonesArray("TargetPoint"))
{
  Ydistance = Ydist;
}

Target::~Target()
{
  if (fTargetPointCollection) {
    fTargetPointCollection->Delete();
    delete fTargetPointCollection;
  }
}

void Target::Initialize()
{
  FairDetector::Initialize();
}

TString PrepareBooleanOperation(TGeoVolumeAssembly* TargetVolume, TGeoVolume * SensitiveVolume, const int nlayers, const char *sensname, const char* shieldname, Double_t dzshield, Double_t sensdz, Double_t passdz, Double_t dzoffset=0.){
    //cut shield inserts and place sensitive layers inside
    TGeoTranslation *cutT;
    TString BooleanUnionShapes("(");
    //union of the sensitive layers positions to be inserted
    for (int ilayer = 0; ilayer < nlayers; ilayer++){
        TString TranslationName("cutT");
        TranslationName += TString(shieldname);

        cutT = new TGeoTranslation(TranslationName.Data(),0,0,dzoffset+dzshield/2. - sensdz/2. - ilayer*(sensdz+passdz));
        cutT->RegisterYourself();
        BooleanUnionShapes += TString(sensname)+TString(":")+TranslationName;
        if (ilayer < nlayers-1) BooleanUnionShapes += " + ";

        TargetVolume->AddNode(SensitiveVolume,ilayer,cutT);
    }
    BooleanUnionShapes += ")";
    //cutting the shapes from the muon shield
    return BooleanUnionShapes;
    //return (TString(shieldname)+TString(" - ")+BooleanUnionShapes);
}

// -----   Private method InitMedium
Int_t Target::InitMedium(const char* name)
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

//--------------Options for detector construction
void Target::SetDetectorDesign(Int_t Design)
{
  fDesign = Design;
  Info("SetDetectorDesign"," to %i", fDesign);
}

void Target::MakeNuTargetPassive(Bool_t a)
{
  fPassive=a;
}

void Target::MergeTopBot(Bool_t SingleEmFilm)
{
  fsingleemulsionfilm=SingleEmFilm;
}

//--------------------------

void Target::SetNumberBricks(Double_t col, Double_t row, Double_t wall)
{
  fNCol = col;
  fNRow = row;
  fNWall = wall;
}

void Target::SetNumberTargets(Int_t target)
{
  fNTarget = target;
}

void Target::SetTargetWallDimension(Double_t WallXDim_, Double_t WallYDim_, Double_t WallZDim_)
{
  WallXDim = WallXDim_;
  WallYDim = WallYDim_;
  WallZDim = WallZDim_;
}

void Target::SetDetectorDimension(Double_t xdim, Double_t ydim, Double_t zdim)
{
  XDimension = xdim;
  YDimension = ydim;
  ZDimension = zdim;
}

void Target::SetEmulsionParam(Double_t EmTh, Double_t EmX, Double_t EmY, Double_t PBTh, Double_t EPlW,Double_t LeadTh, Double_t AllPW)
{
  EmulsionThickness = EmTh;
  EmulsionX = EmX;
  EmulsionY = EmY;
  PlasticBaseThickness = PBTh;
  EmPlateWidth = EPlW;
  LeadThickness = LeadTh;
  AllPlateWidth = AllPW;
}


void Target::SetBrickParam(Double_t BrX, Double_t BrY, Double_t BrZ, Double_t BrPackX, Double_t BrPackY, Double_t BrPackZ, Int_t number_of_plates_)
{
  BrickPackageX = BrPackX;
  BrickPackageY = BrPackY;
  BrickPackageZ = BrPackZ;
  BrickX = BrX;
  BrickY = BrY;
  BrickZ = BrZ;
  number_of_plates = number_of_plates_;
}

void Target::SetCESParam(Double_t RohG, Double_t LayerCESW,Double_t CESW, Double_t CESPack)
{
  CESPackageZ = CESPack;
  LayerCESWidth = LayerCESW;
  RohacellGap = RohG;
  CESWidth = CESW;
}

void Target::SetCellParam(Double_t CellW)
{
  CellWidth = CellW;
}

void Target::SetTTzdimension(Double_t TTZ)
{
  TTrackerZ = TTZ;
}

void Target::SetMagnetHeight(Double_t Y)
{
  fMagnetY=Y;
}

void Target::SetColumnHeight(Double_t Y)
{
  fColumnY=Y;
}

void Target::SetBaseHeight(Double_t Y)
{
  fMagnetBaseY=Y;
}

void Target::SetCoilUpHeight(Double_t H1)
{
  fCoilH1=H1;
}

void Target::SetCoilDownHeight(Double_t H2)
{
  fCoilH2=H2;
}

void Target::SetMagneticField(Double_t B)
{
  fField = B;
}

void Target::SetCenterZ(Double_t z)
{
  fCenterZ = z;
}

void Target::SetBaseDimension(Double_t x, Double_t y, Double_t z)
{
  fBaseX=x;
  fBaseY=y;
  fBaseZ=z;
}


void Target::SetPillarDimension(Double_t x, Double_t y, Double_t z)
{
  fPillarX=x;
  fPillarY=y;
  fPillarZ=z;
}

void Target::SetHpTParam(Int_t n, Double_t dd, Double_t DZ) //need to know about HPT.cxx geometry to place additional targets
{
 fnHpT = n;
 fHpTDistance = dd;
 fHpTDZ = DZ;
}

void Target::ConstructGeometry()
{
  // cout << "Design = " << fDesign << endl;

  InitMedium("air");
  TGeoMedium *air =gGeoManager->GetMedium("air");

  InitMedium("PlasticBase");
  TGeoMedium *PBase =gGeoManager->GetMedium("PlasticBase");

  InitMedium("NuclearEmulsion");
  TGeoMedium *NEmu =gGeoManager->GetMedium("NuclearEmulsion");

  TGeoMaterial *NEmuMat = NEmu->GetMaterial(); //I need the materials to build the mixture
  TGeoMaterial *PBaseMat = PBase->GetMaterial();

  Double_t rho_film = (NEmuMat->GetDensity() * 2 * EmulsionThickness +  PBaseMat->GetDensity() * PlasticBaseThickness)/(2* EmulsionThickness  + PlasticBaseThickness);
  Double_t frac_emu = NEmuMat->GetDensity() * 2 * EmulsionThickness /(NEmuMat->GetDensity() * 2 * EmulsionThickness + PBaseMat->GetDensity() * PlasticBaseThickness);

  if (fsingleemulsionfilm && fDesign < 5) cout<<"TARGET PRINTOUT: Single volume for emulsion film chosen: average density: "<<rho_film<<" fraction in mass of emulsion "<<frac_emu<<endl;

  TGeoMixture * emufilmmixture = new TGeoMixture("EmulsionFilmMixture", 2.00); // two nuclear emulsions separated by the plastic base

  emufilmmixture->AddElement(NEmuMat,frac_emu);
  emufilmmixture->AddElement(PBaseMat,1. - frac_emu);

  TGeoMedium *Emufilm = new TGeoMedium("EmulsionFilm",100,emufilmmixture);

  InitMedium("lead");
  TGeoMedium *lead = gGeoManager->GetMedium("lead");

  InitMedium("tungsten");
  TGeoMedium *tungsten = gGeoManager->GetMedium("tungsten");

  InitMedium("Concrete");
  TGeoMedium *Conc =gGeoManager->GetMedium("Concrete");

  InitMedium("steel");
  TGeoMedium *Steel =gGeoManager->GetMedium("steel");

  InitMedium("silicon");
  TGeoMedium *Silicon = gGeoManager->GetMedium("silicon");

  InitMedium("iron");
  TGeoMedium *Iron =gGeoManager->GetMedium("iron");

  Int_t NPlates = number_of_plates; //Number of doublets emulsion + Pb
  Int_t NRohacellGap = 2;

  //Definition of the target box containing emulsion bricks + (CES if fDesign = 0 o 1) + target trackers (TT)
  TGeoBBox *TargetBox = new TGeoBBox("TargetBox",XDimension/2, YDimension/2, ZDimension/2);
  TGeoVolume *volTarget = new TGeoVolume("volTarget",TargetBox, air);

  // In fDesign 0, fDesign 1 and fDesign 3 the emulsion target is inserted within a magnet
  if(fDesign==0 || fDesign==1 || fDesign == 3)
    {
      TGeoVolume *MagnetVol = nullptr;

      //magnetic field in target
      TGeoUniformMagField *magField2 = new TGeoUniformMagField();

      if(fDesign==1) //TP
	{
	  magField2->SetFieldValue(fField,0,0.);
	  MagnetVol=gGeoManager->GetVolume("Davide");
	}
      if(fDesign==0) //NEW
	{
	  MagnetVol=gGeoManager->GetVolume("Goliath");
	  magField2->SetFieldValue(0.,fField,0.);
	}
      if(fDesign==3)
	{
	  magField2->SetFieldValue(fField,0,0.);
	  MagnetVol=gGeoManager->GetVolume("NudetMagnet");
	}

      //Definition of the target box containing emulsion bricks + CES + target trackers (TT)
      if (fDesign < 3) volTarget->SetField(magField2);
      volTarget->SetVisibility(1);
      volTarget->SetVisDaughters(1);
      if(fDesign==0) //TP
	MagnetVol->AddNode(volTarget,1,new TGeoTranslation(0,-fMagnetY/2+fColumnY+fCoilH2+YDimension/2,0));
      if(fDesign==1) //NEW
	MagnetVol->AddNode(volTarget,1,new TGeoTranslation(0,-fMagnetY/2+fColumnY+YDimension/2,0));
      if(fDesign==3){
        TGeoVolume *volMagRegion=gGeoManager->GetVolume("volMagRegion");
        Double_t ZDimMagnetizedRegion = ((TGeoBBox*) volMagRegion->GetShape())->GetDZ() * 2.; //n.d.r. DZ is the semidimension
        for (int i = 0; i < fNTarget; i++){
         volMagRegion->AddNode(volTarget,i+1,new TGeoTranslation(0,0, -ZDimMagnetizedRegion/2 + ZDimension/2. + i*(ZDimension + 3 * fHpTDZ + 2* fHpTDistance)));
        }
       }
    }







  //
  //Volumes definition
  //

  TGeoBBox *Cell = new TGeoBBox("cell", BrickX/2, BrickY/2, CellWidth/2);
  TGeoVolume *volCell = new TGeoVolume("Cell",Cell,air);

  //Brick
  TGeoBBox *Brick = new TGeoBBox("brick", BrickX/2, BrickY/2, BrickZ/2);
  TGeoVolume *volBrick = new TGeoVolume("Brick",Brick,air);
  volBrick->SetLineColor(kCyan);
  volBrick->SetTransparency(1);
  //need to separate the two cases, now with a ternary operator
  auto *Absorber = new TGeoBBox("Absorber", EmulsionX/2, EmulsionY/2, LeadThickness/2);
  auto *volAbsorber = new TGeoVolume("volAbsorber", Absorber, (fDesign < 4) ? lead : tungsten);

  volAbsorber->SetTransparency(1);
  volAbsorber->SetLineColor(kGray);

  for(Int_t n=0; n<NPlates; n++)
    {
      //decide to use lead or tungsten, according to fDesign
      volBrick->AddNode(volAbsorber, n, new TGeoTranslation(0,0,-BrickZ/2+BrickPackageZ/2+ EmPlateWidth + LeadThickness/2 + n*AllPlateWidth));
    }
  if (fsingleemulsionfilm){  //simplified configuration, unique sensitive layer for the whole emulsion plate
   TGeoBBox *EmulsionFilm;
   TGeoVolume *volEmulsionFilm;

   TGeoBBox *SensitiveLayer;
  
   EmulsionFilm = new TGeoBBox("EmulsionFilm", EmulsionX/2, EmulsionY/2, EmPlateWidth/2);
   volEmulsionFilm = new TGeoVolume("Emulsion",EmulsionFilm,Emufilm); //TOP
   volEmulsionFilm->SetLineColor(kBlue);
   
   if(fPassive==0)
    {
      AddSensitiveVolume(volEmulsionFilm);
    }

   for(Int_t n=0; n<NPlates+1; n++)
    {
      volBrick->AddNode(volEmulsionFilm, n, new TGeoTranslation(0,0,-BrickZ/2+BrickPackageZ/2+ EmPlateWidth/2 + n*AllPlateWidth));
    }
   }
  else { //more accurate configuration, two emulsion films divided by a plastic base
   TGeoBBox *EmulsionFilm = new TGeoBBox("EmulsionFilm", EmulsionX/2, EmulsionY/2, EmulsionThickness/2);
   TGeoVolume *volEmulsionFilm = new TGeoVolume("Emulsion",EmulsionFilm,NEmu); //TOP
   TGeoVolume *volEmulsionFilm2 = new TGeoVolume("Emulsion2",EmulsionFilm,NEmu); //BOTTOM
   volEmulsionFilm->SetLineColor(kBlue);
   volEmulsionFilm2->SetLineColor(kBlue);

   if(fPassive==0)
     {
       AddSensitiveVolume(volEmulsionFilm);
       AddSensitiveVolume(volEmulsionFilm2);
     }
   TGeoBBox *PlBase = new TGeoBBox("PlBase", EmulsionX/2, EmulsionY/2, PlasticBaseThickness/2);
   TGeoVolume *volPlBase = new TGeoVolume("PlasticBase",PlBase,PBase);
   volPlBase->SetLineColor(kYellow-4);
   for(Int_t n=0; n<NPlates+1; n++)
    {
      volBrick->AddNode(volEmulsionFilm2, n, new TGeoTranslation(0,0,-BrickZ/2+BrickPackageZ/2+ EmulsionThickness/2 + n*AllPlateWidth)); //BOTTOM
      volBrick->AddNode(volEmulsionFilm, n, new TGeoTranslation(0,0,-BrickZ/2+BrickPackageZ/2+3*EmulsionThickness/2+PlasticBaseThickness+n*AllPlateWidth)); //TOP
      volBrick->AddNode(volPlBase, n, new TGeoTranslation(0,0,-BrickZ/2+BrickPackageZ/2+EmulsionThickness+PlasticBaseThickness/2+n*AllPlateWidth)); //PLASTIC BASE
    }
 }

  volBrick->SetVisibility(kTRUE);

  //The CES is required only in the option with magnet surrounding the emulsion target
  if(fDesign==0 || fDesign==1 || fDesign == 3)
    {
      //CES

      TGeoBBox *CES = new TGeoBBox("ces", EmulsionX/2, EmulsionY/2, CESWidth/2);
      TGeoVolume *volCES = new TGeoVolume("CES", CES, air);
      volCES->SetTransparency(5);
      volCES->SetLineColor(kYellow-10);
      volCES->SetVisibility(kTRUE);

      TGeoBBox *RohGap = new TGeoBBox("RohGap", EmulsionX/2, EmulsionY/2, RohacellGap/2);
      TGeoVolume *volRohGap = new TGeoVolume("RohacellGap",RohGap,air); //using AIR for CES, not rohacell
      volRohGap->SetTransparency(1);
      volRohGap->SetLineColor(kYellow);

      for(Int_t n=0; n<NRohacellGap; n++)
	{
	  volCES->AddNode(volRohGap, n, new TGeoTranslation(0,0,-CESWidth/2 +CESPackageZ/2+  EmPlateWidth + RohacellGap/2 + n*LayerCESWidth)); //ROHACELL
	}
      if(fsingleemulsionfilm){ //simplified configuration, unique sensitive layer for the whole emulsion plate
       TGeoBBox *EmulsionFilmCES = new TGeoBBox("EmulsionFilmCES", EmulsionX/2, EmulsionY/2, EmPlateWidth/2);
       TGeoVolume *volEmulsionFilmCES = new TGeoVolume("EmulsionCES",EmulsionFilmCES,Emufilm); //TOP
       volEmulsionFilmCES->SetLineColor(kBlue);
       if(fPassive==0)
	{
	  AddSensitiveVolume(volEmulsionFilmCES);
	}

       for(Int_t n=0; n<NRohacellGap+1;n++)
	{
	  volCES->AddNode(volEmulsionFilmCES,n, new TGeoTranslation(0,0,-CESWidth/2+CESPackageZ/2+EmPlateWidth/2+n*LayerCESWidth));
	}

      }
      else{ //more accurate configuration, two emulsion films divided by a plastic base

       TGeoBBox *EmulsionFilmCES = new TGeoBBox("EmulsionFilmCES", EmulsionX/2, EmulsionY/2, EmulsionThickness/2);
       TGeoVolume *volEmulsionFilmCES = new TGeoVolume("EmulsionCES",EmulsionFilmCES,NEmu); //TOP
       TGeoVolume *volEmulsionFilm2CES = new TGeoVolume("Emulsion2CES",EmulsionFilmCES,NEmu); //BOTTOM
       volEmulsionFilmCES->SetLineColor(kBlue);
       volEmulsionFilm2CES->SetLineColor(kBlue);
       if(fPassive==0)
 	{
 	  AddSensitiveVolume(volEmulsionFilmCES);
 	  AddSensitiveVolume(volEmulsionFilm2CES);
 	}
       //CES PLASTIC BASE
       TGeoBBox *PlBaseCES = new TGeoBBox("PlBaseCES", EmulsionX/2, EmulsionY/2, PlasticBaseThickness/2);
       TGeoVolume *volPlBaseCES = new TGeoVolume("PlasticBaseCES",PlBaseCES,PBase);
       volPlBaseCES->SetLineColor(kYellow);
       for(Int_t n=0; n<NRohacellGap+1;n++)
 	{
 	  volCES->AddNode(volEmulsionFilm2CES,n, new TGeoTranslation(0,0,-CESWidth/2+CESPackageZ/2+EmulsionThickness/2+n*LayerCESWidth)); //BOTTOM
 	  volCES->AddNode(volEmulsionFilmCES, n, new TGeoTranslation(0,0,-CESWidth/2+CESPackageZ/2+3*EmulsionThickness/2+PlasticBaseThickness+n*LayerCESWidth)); //TOP
 	  volCES->AddNode(volPlBaseCES, n, new TGeoTranslation(0,0,-CESWidth/2+CESPackageZ/2+EmulsionThickness+PlasticBaseThickness/2+n*LayerCESWidth)); //PLASTIC BASE
 	  //	if(n == 2)
 	  // cout << "-CESWidth/2+3*EmulsionThickness/2+PlasticBaseThickness+n*LayerCESWidth = " << -CESWidth/2+3*EmulsionThickness/2+PlasticBaseThickness+n*LayerCESWidth << endl;
       }

      }

      volCell->AddNode(volBrick,1,new TGeoTranslation(0,0,-CellWidth/2 + BrickZ/2));
      volCell->AddNode(volCES,1,new TGeoTranslation(0,0,-CellWidth/2 + BrickZ + CESWidth/2));

      TGeoBBox *Row = new TGeoBBox("row",WallXDim/2, BrickY/2, CellWidth/2);
      TGeoVolume *volRow = new TGeoVolume("Row",Row,air);
      volRow->SetLineColor(20);

      Double_t d_cl_x = -WallXDim/2;
      for(int j= 0; j < fNCol; j++)
	{
	  volRow->AddNode(volCell,j,new TGeoTranslation(d_cl_x+BrickX/2, 0, 0));
	  d_cl_x += BrickX;
	}

      TGeoBBox *Wall = new TGeoBBox("wall",WallXDim/2, WallYDim/2, CellWidth/2);
      TGeoVolume *volWall = new TGeoVolume("Wall",Wall,air);

      Double_t d_cl_y = -WallYDim/2;
      for(int k= 0; k< fNRow; k++)
	{
	  volWall->AddNode(volRow,k,new TGeoTranslation(0, d_cl_y + BrickY/2, 0));

	  // 2mm is the distance for the structure that holds the brick
	  d_cl_y += BrickY + Ydistance;
	}

      //Columns

      Double_t d_cl_z = - ZDimension/2 + TTrackerZ;

      for(int l = 0; l < fNWall; l++)
	{
	  volTarget->AddNode(volWall,l,new TGeoTranslation(0, 0, d_cl_z +CellWidth/2));

	  //6 cm is the distance between 2 columns of consecutive Target for TT placement
	  d_cl_z += CellWidth + TTrackerZ;
	}
    }


  //in fDesign==2 and fDesign==4 the emulsion target is not surrounded by a magnet => no magnetic field inside
  //In the no Magnetic field option, no CES is needed => only brick walls + TT
  if(fDesign==2 || fDesign == 4 || fDesign==5)
    {
      TGeoVolume *tTauNuDet = gGeoManager->GetVolume("tTauNuDet");
      cout<< "Tau Nu Detector fMagnetConfig: "<< fDesign<<endl;

      if (fDesign != 5) tTauNuDet->AddNode(volTarget,1,new TGeoTranslation(0,0,fCenterZ));

      TGeoBBox *Row = new TGeoBBox("row",WallXDim/2, BrickY/2, WallZDim/2);
      TGeoVolume *volRow = new TGeoVolume("Row",Row,air);
      volRow->SetLineColor(20);

      Double_t d_cl_x = -WallXDim/2;
      for(int j= 0; j < fNCol; j++)
	{
	  volRow->AddNode(volBrick,j,new TGeoTranslation(d_cl_x+BrickX/2, 0, 0));
	  d_cl_x += BrickX;
	}
      TGeoBBox *Wall = new TGeoBBox("wall",WallXDim/2, WallYDim/2, WallZDim/2);
      TGeoVolume *volWall = new TGeoVolume("Wall",Wall,air);
      volWall->SetLineColor(kGreen);

      Double_t d_cl_y = -WallYDim/2;
      for(int k= 0; k< fNRow; k++)
	{
	  volWall->AddNode(volRow,k,new TGeoTranslation(0, d_cl_y + BrickY/2, 0));

	  // 2mm is the distance for the structure that holds the brick
	  d_cl_y += BrickY + Ydistance;
	}
       //Columns

      Double_t d_cl_z = - ZDimension/2 + TTrackerZ;

      for(int l = 0; l < fNWall; l++)
	{
	  volTarget->AddNode(volWall,l,new TGeoTranslation(0, 0, d_cl_z +BrickZ/2));

    if (fDesign==5){ //TT here for now (need to reactivate TT class!)
     TGeoBBox *TT = new TGeoBBox("TT", EmulsionX/2, EmulsionY/2, (TTrackerZ)/2);
     TGeoVolume *volTT = new TGeoVolume("TargetTracker",TT,air); //TOP
     volTT->SetLineColor(kBlue);
     if (fPassive == 0) AddSensitiveVolume(volTT);
     volTarget->AddNode(volTT,l,new TGeoTranslation(0, 0, d_cl_z +BrickZ+TTrackerZ/2));

    }

	  //6 cm is the distance between 2 columns of consecutive Target for TT placement
	  d_cl_z += BrickZ + TTrackerZ;
	}
      if(fDesign==2)
	{
    	TGeoBBox *Base = new TGeoBBox("Base", fBaseX/2, fBaseY/2, fBaseZ/2);
    	TGeoVolume *volBase = new TGeoVolume("volBase",Base,Conc);
    	volBase->SetLineColor(kYellow-3);
    	tTauNuDet->AddNode(volBase,1, new TGeoTranslation(0,-WallYDim/2 - fBaseY/2,fCenterZ));

    	TGeoBBox *PillarBox = new TGeoBBox(fPillarX/2,fPillarY/2, fPillarZ/2);
	  TGeoVolume *PillarVol = new TGeoVolume("PillarVol",PillarBox,Steel);
	  PillarVol->SetLineColor(kGreen+3);
	  tTauNuDet->AddNode(PillarVol,1, new TGeoTranslation(-XDimension/2+fPillarX/2,-YDimension/2-fBaseY-fPillarY/2, fCenterZ-ZDimension/2+fPillarZ/2));
	  tTauNuDet->AddNode(PillarVol,2, new TGeoTranslation(XDimension/2-fPillarX/2,-YDimension/2-fBaseY-fPillarY/2, fCenterZ-ZDimension/2+fPillarZ/2));
	  tTauNuDet->AddNode(PillarVol,3, new TGeoTranslation(-XDimension/2+fPillarX/2,-YDimension/2-fBaseY-fPillarY/2, fCenterZ+ZDimension/2-fPillarZ/2));
	  tTauNuDet->AddNode(PillarVol,4, new TGeoTranslation(XDimension/2-fPillarX/2,-YDimension/2-fBaseY-fPillarY/2, fCenterZ+ZDimension/2-fPillarZ/2));
    }
  }
  if (fDesign == 5){
    TGeoVolume *tTauNuDet = gGeoManager->GetVolume("tTauNuDet");
    //first, define our volumes
    //const Double_t XDimension = 40.;
    //const Double_t YDimension = 40.;
    //layers to be cut
    const Double_t SensX = EmulsionX;
    const Double_t SensY = EmulsionY;
    const Double_t SensZ = 2.;

    const Double_t FeZ = 5.;

    auto *volMagTargetHCAL = new TGeoVolumeAssembly("volMagTargetHCAL");
    tTauNuDet->AddNode(volMagTargetHCAL,0,new TGeoTranslation(0,0,-3432.0000)); //please modify the position to retrieve it from, well, somewhere in the program

    auto * SNDSensitiveLayer = new TGeoBBox("SNDSensitiveLayer", SensX/2., SensY/2., SensZ/2.);
    auto * volSNDSensitiveLayer = new TGeoVolume("volSNDSensitiveLayer",SNDSensitiveLayer, air); //TOP
    volSNDSensitiveLayer->SetLineColor(kCyan);

    //composition of sensitive volumes
    const Double_t SciFiX = EmulsionX;
    const Double_t SciFiY = EmulsionY;
    const Double_t SciFiZ = 0.5;

    const Double_t ScintX = EmulsionX;
    const Double_t ScintY = EmulsionY;
    const Double_t ScintZ = 1.5;

    auto * SNDSciFi = new TGeoBBox("SNDSciFi", SciFiX/2., SciFiY/2., SciFiZ/2.);
    auto * volSNDSciFi = new TGeoVolume("volSNDSciFi",SNDSciFi, Silicon); //TOP
    volSNDSciFi->SetLineColor(kGreen);
    if (fPassive == 0) AddSensitiveVolume(volSNDSciFi);

    auto * SNDScint = new TGeoBBox("SNDScint", ScintX/2., ScintY/2., ScintZ/2.);
    auto * volSNDScint = new TGeoVolume("volSNDScint",SNDScint, Silicon); //TOP
    volSNDScint->SetLineColor(kBlue);

    volSNDSensitiveLayer->AddNode(volSNDSciFi,0, new TGeoTranslation(0.,0.,-SensZ/2. + SciFiZ/2.));
    volSNDSensitiveLayer->AddNode(volSNDScint,0, new TGeoTranslation(0.,0.,-SensZ/2. + SciFiZ + ScintZ/2.));

    //***CUTTING THE MUON SHIELD HERE****//
    const Double_t dz_6R = 242. *cm *2; //better to define dZ as the full length, as usual
    const Double_t dz_6L = dz_6R; 
    //retrieving the muon shield shapes
    auto Magn6_MiddleMagR = gGeoManager->GetVolume("Magn6_MiddleMagR");
    TGeoArb8 * arb_6R = (TGeoArb8*) Magn6_MiddleMagR->GetShape();

    auto Magn6_MiddleMagL = gGeoManager->GetVolume("Magn6_MiddleMagL");
    TGeoArb8 * arb_6L = (TGeoArb8*) Magn6_MiddleMagR->GetShape();

    //computing the translations of the holes
    const Int_t nlayers_MagTargetHCAL = 50;
    TString BooleanUnionShapes = PrepareBooleanOperation(volMagTargetHCAL, volSNDSensitiveLayer, nlayers_MagTargetHCAL,  "SNDSensitiveLayer" , arb_6R->GetName(), dz_6R, SensZ, FeZ);
    //check how long it was in the end
    TGeoShapeAssembly * MagTargetHCAL = static_cast<TGeoShapeAssembly*> (volMagTargetHCAL->GetShape());
    MagTargetHCAL->ComputeBBox(); //for an assembly needs to be computed
    Double_t dZ_MagTargetHCAL = MagTargetHCAL->GetDZ();

    cout<<"Check of boolean operation "<<(TString(arb_6R->GetName())+TString(" - ")+BooleanUnionShapes).Data()<<endl;

    TGeoCompositeShape *cs6R = new TGeoCompositeShape("cs6R",(TString(arb_6R->GetName())+TString(" - ")+BooleanUnionShapes).Data());

    Magn6_MiddleMagR->SetShape(cs6R);

    TGeoCompositeShape *cs6L = new TGeoCompositeShape("cs6L",(TString(arb_6L->GetName())+TString(" - ")+BooleanUnionShapes).Data());
    Magn6_MiddleMagL->SetShape(cs6L);

    //Target In Magnet 5
    const Double_t SiX = XDimension;
    const Double_t SiY = YDimension;
    const Double_t SiZ = 0.8;

    auto * SNDTargetSiliconLayer = new TGeoBBox("SNDTargetSiliconLayer", SiX/2., SiY/2., SiZ/2.);
    auto * volSNDTargetSiliconLayer = new TGeoVolume("volSNDTargetSiliconLayer",SNDTargetSiliconLayer,Silicon);
    volSNDTargetSiliconLayer->SetLineColor(kGreen);
    //now MagnetHCAL is split between the two magnet sections
    auto *volMagHCAL_6 = new TGeoVolumeAssembly("volMagHCAL_6");
    auto *volMagHCAL_5 = new TGeoVolumeAssembly("volMagHCAL_5");

    const Int_t nlayers_MagHCAL = 34;
    Double_t dz_5L = 2*305. *cm;
    Double_t dz_5R = dz_5L;
    //building replicas of inner volumes of magnet 5 from FairShip
    auto Magn5_MiddleMagR = gGeoManager->GetVolume("Magn5_MiddleMagR");
    TGeoArb8 * arb_5R = (TGeoArb8*) Magn5_MiddleMagR->GetShape();

    auto Magn5_MiddleMagL = gGeoManager->GetVolume("Magn5_MiddleMagL");
    TGeoArb8 * arb_5L = (TGeoArb8*) Magn5_MiddleMagR->GetShape();

    //now also in magnet6!
    const Int_t nlayers_MagHCAL_magnet6 = 24; //in the magnet dowstream
    BooleanUnionShapes = PrepareBooleanOperation(volMagHCAL_6, volSNDTargetSiliconLayer, nlayers_MagHCAL_magnet6,  "SNDTargetSiliconLayer" , arb_6R->GetName(), dz_6R, SiZ, FeZ, -2*dZ_MagTargetHCAL-FeZ);

    //cutting them and inserting sensitive volumes
    BooleanUnionShapes = PrepareBooleanOperation(volMagHCAL_5, volSNDTargetSiliconLayer, nlayers_MagHCAL - nlayers_MagHCAL_magnet6, "SNDTargetSiliconLayer" , arb_5R->GetName(), dz_5R, SiZ, FeZ);
    cout<<"Check of boolean operation magnet5"<<(TString("arb_5R - ")+BooleanUnionShapes).Data()<<endl;
    TGeoCompositeShape *cs5R = new TGeoCompositeShape("cs5R",(TString(arb_5R->GetName())+TString(" - ")+BooleanUnionShapes).Data());

    TGeoCompositeShape *cs5L = new TGeoCompositeShape("cs5L",(TString(arb_5L->GetName())+TString(" - ")+BooleanUnionShapes).Data());

    tTauNuDet->AddNode(volMagHCAL_5,0,new TGeoTranslation(0,0,-3432.0000 -dz_6L/2.-dz_5R/2.-10.));
    tTauNuDet->AddNode(volMagHCAL_6,0,new TGeoTranslation(0,0,-3432.0000));

    //*****Silicon Target******//

    const Double_t TungstenX = EmulsionX;
    const Double_t TungstenY = EmulsionY;
    const Double_t TungstenZ = 0.7;

    const Int_t nlayers_SiTarget = 58;

    const Double_t SiTargetX = EmulsionX;
    const Double_t SiTargetY = EmulsionY;
    const Double_t SiTargetZ = nlayers_SiTarget * (SiZ + TungstenZ);

    auto *SiTargetBox = new TGeoBBox("SiTargetBox",SiTargetX/2.,SiTargetY/2.,SiTargetZ/2.);
    auto *volSiTarget = new TGeoVolume("volSiTarget",SiTargetBox,air);

    if (fPassive == 0) AddSensitiveVolume(volSNDTargetSiliconLayer); //uncomment when copying in actual class!

    auto * SNDTargetTungstenBlock = new TGeoBBox("SNDTargetTungstenBlock", TungstenX/2., TungstenY/2., TungstenZ/2.);
    auto * volSNDTargetTungstenBlock = new TGeoVolume("volSNDTargetTungstenBlock",SNDTargetTungstenBlock,tungsten); //TOP
    volSNDTargetTungstenBlock->SetLineColor(kGray);

    for(Int_t n=0; n<nlayers_SiTarget; n++)
    {
      volSiTarget->AddNode(volSNDTargetTungstenBlock, n, new TGeoTranslation(0,0, -SiTargetZ/2. + n *(SiZ+TungstenZ) + TungstenZ/2. )); //W
      volSiTarget->AddNode(volSNDTargetSiliconLayer, n+1e+2, new TGeoTranslation(0,0,-SiTargetZ/2. + n *(SiZ+TungstenZ) + TungstenZ + SiZ/2 )); //Silicon
    }

    //cutting the holes in the magnet for the big targets
    const Double_t dZ_MagHCAL_5 = 0.; //for now
    const Double_t SiTarget_MagHCAL_Gap = 10.; //gap with Silicon Target upstream
    const Double_t EmTarget_SiTarget_Gap = 10.; //gap with Emulsion Target upstream

    TGeoTranslation * T_SiTarget = new TGeoTranslation("T_SiTarget",0,0,+dz_5R/2.-dZ_MagHCAL_5-SiTarget_MagHCAL_Gap-SiTargetZ/2.); 
    T_SiTarget->RegisterYourself();

    TGeoTranslation * T_EmTarget = new TGeoTranslation("T_EmTarget",0,0,+dz_5R/2.-dZ_MagHCAL_5-SiTarget_MagHCAL_Gap-SiTargetZ-EmTarget_SiTarget_Gap-ZDimension/2.); 
    T_EmTarget->RegisterYourself();

    TGeoCompositeShape *cs5L_si = new TGeoCompositeShape("cs5L_si","cs5L-SiTargetBox:T_SiTarget");
    TGeoCompositeShape *cs5L_siem = new TGeoCompositeShape("cs5L_siem","cs5L_si-TargetBox:T_EmTarget");

    Magn5_MiddleMagL->SetShape(cs5L_siem);
    Magn5_MiddleMagL->SetTransparency(1);

    TGeoCompositeShape *cs5R_si = new TGeoCompositeShape("cs5R_si","cs5R-SiTargetBox:T_SiTarget");
    TGeoCompositeShape *cs5R_siem = new TGeoCompositeShape("cs5R_siem","cs5R_si-TargetBox:T_EmTarget");
    Magn5_MiddleMagR->SetShape(cs5R_siem);
    Magn5_MiddleMagR->SetTransparency(1);

    tTauNuDet->AddNode(volSiTarget,0,new TGeoTranslation(0,0,-3432.0000-dz_6L/2.-dz_5R/2.-10.+dz_5R/2.-dZ_MagHCAL_5-SiTarget_MagHCAL_Gap-SiTargetZ/2.));
    tTauNuDet->AddNode(volTarget,0,new TGeoTranslation(0,0,
                                  -3432.0000-dz_6L/2.-dz_5R/2.-10.+dz_5R/2.-dZ_MagHCAL_5-SiTarget_MagHCAL_Gap-SiTargetZ-EmTarget_SiTarget_Gap-ZDimension/2.));

  }
}//end construct geometry

Bool_t  Target::ProcessHits(FairVolume* vol)
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
    //Int_t fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    gMC->CurrentVolID(fVolumeID);
    Int_t detID = fVolumeID;
    //gGeoManager->PrintOverlaps();

    //cout<< "detID = " << detID << endl;
    Int_t MaxLevel = gGeoManager->GetLevel();
    const Int_t MaxL = MaxLevel;
    //cout << "MaxLevel = " << MaxL << endl;
    //cout << gMC->CurrentVolPath()<< endl;


    Int_t motherV[MaxL];
//   Bool_t EmTop = 0, EmBot = 0, EmCESTop = 0, EmCESBot = 0;
    Bool_t EmBrick = false;
    Bool_t EmTop = false;
    Int_t NPlate =0;
    const char *name;

    name = gMC->CurrentVolName();

    if(strcmp(name, "volSNDTargetSiliconLayer") == 0){
      detID = detID +1e+4;
    }
    
    else if(strcmp(name, "volSNDSciFi") == 0){ 
      //retrieve detector ID of mother volume
      for(Int_t i = 0; i < MaxL;i++)
      {     
       motherV[i] = gGeoManager->GetMother(i)->GetNumber();
       const char *mumname = gMC->CurrentVolOffName(i);
       if(strcmp(mumname, "volSNDSensitiveLayer") == 0){ 
        detID = motherV[i] + 2e+4;      
       }
      }
    }

    else{
    //cout << name << endl;

    if(strcmp(name, "Emulsion") == 0)
      {
	EmBrick=1;
	NPlate = detID;
        EmTop=1;
      }
    if(strcmp(name, "Emulsion2") == 0)
      {
	EmBrick=1;
	NPlate = detID;
        EmTop=0;
      }
    if(strcmp(name, "EmulsionCES") == 0)
      {
	NPlate = detID;
        EmTop=1;
      }
    if(strcmp(name, "Emulsion2CES") == 0)
      {
	NPlate = detID;
        EmTop=0;
      }

    Int_t  NWall = 0, NColumn =0, NRow =0;

    for(Int_t i = 0; i < MaxL;i++)
      {
	motherV[i] = gGeoManager->GetMother(i)->GetNumber();
	const char *mumname = gMC->CurrentVolOffName(i);
	if(motherV[0]==1 && motherV[0]!=detID)
	  {
	    if(strcmp(mumname, "Brick") == 0 ||strcmp(mumname, "CES") == 0) NColumn = motherV[i];
	    if(strcmp(mumname, "Cell") == 0) NRow = motherV[i];
	    if(strcmp(mumname, "Row") == 0) NWall = motherV[i];
            if((strcmp(mumname, "Wall") == 0)&& (motherV[i]==2)) NWall += fNWall;
	  }
	else
	  {

	    if(strcmp(mumname, "Cell") == 0) NColumn = motherV[i];
	    if(strcmp(mumname, "Row") == 0) NRow = motherV[i];
	    if(strcmp(mumname, "Wall") == 0) NWall = motherV[i];
             if((strcmp(mumname, "volTarget") == 0) && (motherV[i]==2)) NWall += fNWall;
	  }
	//cout << i << "   " << motherV[i] << "    name = " << mumname << endl;
      }

    Bool_t BrickorCES = EmBrick == 1;

    detID = (NWall+1) *1E7 + (NRow+1) * 1E6 + (NColumn+1)*1E4 + BrickorCES *1E3 + (NPlate+1)*1E1 + EmTop*1 ;
    }//end else about detector name

    fVolumeID = detID;
    if (fELoss == 0. ) { return kFALSE; }
    TParticle* p=gMC->GetStack()->GetCurrentTrack();
    //Int_t MotherID =gMC->GetStack()->GetCurrentParentTrackNumber();
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
    stack->AddPoint(ktauTarget);
  }

  return kTRUE;
}


void Target::DecodeBrickID(Int_t detID, Int_t &NWall, Int_t &NRow, Int_t &NColumn, Int_t &NPlate, Bool_t &EmCES, Bool_t &EmBrick, Bool_t &EmTop)
{
  Bool_t BrickorCES = false;

  NWall = detID/1E7;
  NRow = (detID - NWall*1E7)/1E6;
  NColumn = (detID - NWall*1E7 -NRow*1E6)/1E4;
  Double_t b = (detID - NWall*1E7 -NRow*1E6 - NColumn*1E4)/1.E3;
  if(b < 1)
    {
      BrickorCES = 0;
      NPlate = (detID - NWall*1E7 -NRow*1E6 - NColumn*1E4 - BrickorCES*1E3)/1E1;
//      NPlate = detID - NWall*1E7 -NRow*1E6 - NColumn*1E4 - BrickorCES*1E3;
    }
  if(b >= 1)
    {
      BrickorCES = 1;
      NPlate = (detID - NWall*1E7 -NRow*1E6 - NColumn*1E4 - BrickorCES*1E3)/1E1;
//      NPlate = detID - NWall*1E7 -NRow*1E6 - NColumn*1E4 - BrickorCES*1E3;
    }
  EmTop = (detID - NWall*1E7 -NRow*1E6 - NColumn*1E4- BrickorCES*1E3- NPlate*1E1)/1E0;
  if(BrickorCES == 0)
    {
      EmCES = 1; EmBrick =0;
    }
  if(BrickorCES == 1)
    {
      EmBrick = 1; EmCES =0;
    }

  // cout << "NPlate = " << NPlate << ";  NColumn = " << NColumn << ";  NRow = " << NRow << "; NWall = " << NWall << endl;
  // cout << "BrickorCES = " << BrickorCES <<endl;
  // cout << "EmCES = " << EmCES << ";    EmBrick = " << EmBick << endl;
  // cout << endl;
}


void Target::EndOfEvent()
{
  fTargetPointCollection->Clear();
}


void Target::Register()
{

  /** This will create a branch in the output tree called
      TargetPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */

  FairRootManager::Instance()->Register("TargetPoint", "Target",
					fTargetPointCollection, kTRUE);
}

TClonesArray* Target::GetCollection(Int_t iColl) const
{
  if (iColl == 0) { return fTargetPointCollection; }
  else { return NULL; }
}

void Target::Reset()
{
  fTargetPointCollection->Clear();
}


TargetPoint* Target::AddHit(Int_t trackID,Int_t detID,
			    TVector3 pos, TVector3 mom,
			    Double_t time, Double_t length,
			    Double_t eLoss, Int_t pdgCode)
{
  TClonesArray& clref = *fTargetPointCollection;
  Int_t size = clref.GetEntriesFast();
  //cout << "brick hit called"<< pos.z()<<endl;
  return new(clref[size]) TargetPoint(trackID,detID, pos, mom,
				      time, length, eLoss, pdgCode);
}
