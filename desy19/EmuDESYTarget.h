//  
//EmuDESYTarget.h
//
//

#ifndef EmuDESYTarget_H
#define EmuDESYTarget_H

#include "FairModule.h"                 // for FairModule
#include "FairDetector.h"

#include "Rtypes.h"                     // for ShipMuonShield::Class, Bool_t, etc

#include <string>                       // for string

#include "TVector3.h"
#include "TLorentzVector.h"

class EmuDESYPoint;
class FairVolume;
class TClonesArray;

class EmuDESYTarget : public FairDetector
{
public:
  EmuDESYTarget(const char* name, const Double_t zEmuDESYTarget, Bool_t Active, const char* Title = "EmuDESYTarget");
    EmuDESYTarget();
    virtual ~EmuDESYTarget();
    
    /**      Create the detector geometry        */
    void ConstructGeometry();  
    void AddEmulsionFilm(Double_t zposition, Int_t nreplica, TGeoVolume * volTarget, TGeoVolume * volEmulsionFilm, TGeoVolume * volEmulsionFilm2, TGeoVolume * volPlBase); 

    void SetBrickParam(Double_t BrX, Double_t BrY, Double_t BrZ, Double_t BrPackX, Double_t BrPackY, Double_t BrPackZ);
    void SetEmulsionParam(Double_t EmTh, Double_t EmX, Double_t EmY, Double_t PBTh,Double_t EPlW, Double_t PasSlabTh, Double_t AllPW);
 
    void SetTargetParam(Double_t TX, Double_t TY, Double_t TZ);
    void SetECCDistance(Double_t ECCdistance);
    void SetPassiveDZ(Double_t PassiveDZ);

    void SetNPlates(Int_t NPlates, Int_t NPlates_second);
    void SetNRun(Int_t NRun);
    
    void SetTargetXRotation(Double_t TargetXRotation);
    
    /**      Initialization of the detector is done here    */
    virtual void Initialize();
    
    /**  Method called for each step during simulation (see FairMCApplication::Stepping()) */
    virtual Bool_t ProcessHits( FairVolume* v=0);
    
    /**       Registers the produced collections in FAIRRootManager.     */
    virtual void   Register();
    
    /** Gets the produced collections */
    virtual TClonesArray* GetCollection(Int_t iColl) const ;
    
    /**      has to be called after each event to reset the containers      */
    virtual void   Reset();
    
    /**      How to add your own point of type EmuDESYPoint to the clones array */

    EmuDESYPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		     Double_t time, Double_t length, Double_t eLoss, Int_t pdgCode);
    
        
    virtual void   CopyClones( TClonesArray* cl1,  TClonesArray* cl2 , Int_t offset) {;}
    virtual void   SetSpecialPhysicsCuts() {;}
    virtual void   EndOfEvent();
    virtual void   FinishPrimary() {;}
    virtual void   FinishRun() {;}
    virtual void   BeginPrimary() {;}
    virtual void   PostTrack() {;}
    virtual void   PreTrack() {;}
    virtual void   BeginEvent() {;}
    
       
    EmuDESYTarget(const EmuDESYTarget&);
    EmuDESYTarget& operator=(const EmuDESYTarget&);
    
    ClassDef(EmuDESYTarget,1)
    
private:
    
    /** Track information to be stored until the track leaves the active volume. */
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    
    /** container for data points */
    TClonesArray*  fEmuDESYPointCollection;
    
protected:

    //Target position
    Double_t zEmuTargetPosition;
    
    Int_t InitMedium(const char* name);
    
    //attributes for the brick
    Double_t EmulsionThickness;
    Double_t EmulsionX;
    Double_t EmulsionY;
  
    //attributes for the new target configuration, to simulate SHiP target
    Double_t TargetX;
    Double_t TargetY;
    Double_t TargetZ;

    Double_t PlasticBaseThickness;
    Double_t PassiveSlabThickness;
    Double_t EmPlateWidth; // Z dimension of the emulsion plates = 2*EmulsionThickness+PlasticBaseThickness
    Double_t AllPlateWidth; //PlateZ + LeadThickness

    Double_t fECCdistance;
    Double_t fPassiveDZ;

    Double_t BrickPackageX; //dimension of the brick package along X
    Double_t BrickPackageY; //dimension of the brick package along Y
    Double_t BrickPackageZ; //dimension of the brick package along Z
    Double_t BrickZ; //dimension of the brick + package along the Z axis
    Double_t BrickY;
    Double_t BrickX;

    Int_t fNRun;
    Int_t fNPlates;
    Int_t fNPlates_second;
    
    Double_t fTargetXRotation; //set rotation of target around x
};

#endif

