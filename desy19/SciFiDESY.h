#ifndef SCIFIDESY_H
#define SCIFIDESY_H

#include "FairModule.h"                 // for FairModule
#include "FairDetector.h"                  // for FairDetector

#include "Rtypes.h"                     // for ShipMuonShield::Class, Bool_t, etc

#include <string>                       // for string

#include "TVector3.h"
#include "TLorentzVector.h"

class SciFiDESYPoint;
class FairVolume;
class TClonesArray;
class SciFiDESY:public FairDetector
{
  public:
    SciFiDESY(const char* name,Bool_t Active, const char* Title="SciFiDESY");
    SciFiDESY();
    virtual ~SciFiDESY();

    void ConstructGeometry();

    void SetBoxDimensions(Double_t DX, Double_t DY, Double_t DZ);
    void SetStationDimensions(Double_t StationDX, Double_t DESYStationDY, Double_t DESYStationDZ);
    void SetStationPositions(Double_t posz1, Double_t posz2);

    /**      Initialization of the detector is done here    */
    virtual void Initialize();

    /**       this method is called for each step during simulation
     *       (see FairMCApplication::Stepping())
     */
    virtual Bool_t ProcessHits( FairVolume* v=0);

    /**       Registers the produced collections in FAIRRootManager.     */
    virtual void   Register();

    /** Gets the produced collections */
    virtual TClonesArray* GetCollection(Int_t iColl) const ;

    /**      has to be called after each event to reset the containers      */
    virtual void   Reset();

    /**      This method is an example of how to add your own point
     *       of type muonPoint to the clones array
     */
    SciFiDESYPoint* AddHit(Int_t trackID, Int_t detID,
        TVector3 pos, TVector3 mom,
        Double_t time, Double_t length,
        Double_t eLoss, Int_t pdgCode);

    /** The following methods can be implemented if you need to make
     *  any optional action in your detector during the transport.
     */

    virtual void   CopyClones( TClonesArray* cl1,  TClonesArray* cl2 ,
        Int_t offset) {;}
    virtual void   SetSpecialPhysicsCuts() {;}
    virtual void   EndOfEvent();
    virtual void   FinishPrimary() {;}
    virtual void   FinishRun() {;}
    virtual void   BeginPrimary() {;}
    virtual void   PostTrack() {;}
    virtual void   PreTrack() {;}
    virtual void   BeginEvent() {;}

  private:

    /** Track information to be stored until the track leaves the
      active volume.
      */
    Int_t          fTrackID;           //!  track index
    Int_t          fPdgCode;           //!  pdg code
    Int_t          fVolumeID;          //!  volume id
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    /** container for data points */
    TClonesArray*  fSciFiDESYPointCollection;

    Int_t InitMedium(const char* name);



  protected:

    Double_t DimX;
    Double_t DimY;
    Double_t DimZ;

    Double_t SciFiStatDX;
    Double_t SciFiStatDY;
    Double_t SciFiStatDZ;

    Double_t fZposSciFi1;
    Double_t fZposSciFi2;

    SciFiDESY(const SciFiDESY&);
    SciFiDESY& operator=(const SciFiDESY&);
    ClassDef(SciFiDESY,2)

};
#endif
