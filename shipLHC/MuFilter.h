//  
//  MuFilter.h 
//  
//  by A. Buonaura

#ifndef MuFilter_H
#define MuFilter_H

#include "FairModule.h"                 // for FairModule
#include "FairDetector.h"

#include "Rtypes.h"                     // for ShipMuonShield::Class, Bool_t, etc

#include <string>                       // for string

#include "TVector3.h"
#include "TLorentzVector.h"

class MuFilterPoint;
class FairVolume;
class TClonesArray;

class MuFilter : public FairDetector
{
	public:
		MuFilter(const char* name, Bool_t Active, const char* Title="MuonFilter");
		MuFilter();
		virtual ~MuFilter();

		/**      Create the detector geometry        */
		void ConstructGeometry();

		/** Other functions **/
		void SetIronBlockDimensions(Double_t , Double_t, Double_t);
		void SetTimingPlanesDimensions(Double_t, Double_t, Double_t);
		void SetMuFilterDimensions(Double_t, Double_t, Double_t);
		void SetNplanes(Int_t);
		void SetCenterZ(Double_t);
		void SetDisplacement(Double_t , Double_t );


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

		/**      How to add your own point of type EmulsionDetPoint to the clones array */

		MuFilterPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
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

		MuFilter(const MuFilter&);
		MuFilter& operator=(const MuFilter&);

		ClassDef(MuFilter,1)

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
			TClonesArray*  fMuFilterPointCollection;

	protected:
			Double_t fMuFilterX;	//|MuonFilterBox dimensions
			Double_t fMuFilterY;	//|
			Double_t fMuFilterZ;	//|

			Double_t fFeBlockX;     //|Passive Iron blocks dimensions
			Double_t fFeBlockY;     //|
			Double_t fFeBlockZ;     //|

			Double_t fTDetX;	//|Timing detector planes dimensions
			Double_t fTDetY;	//|
			Double_t fTDetZ;	//|

			Double_t fCenterZ;	//Zposition of the muon filter
			Double_t fShiftX;	//|Shift in x-y wrt beam line
			Double_t fShiftY;	//|

			Int_t fNplanes;		//Number of planes

			Int_t InitMedium(const char* name);
};

#endif
