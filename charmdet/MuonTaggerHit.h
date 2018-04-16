#ifndef MUONTAGGERHIT_H
#define MUONTAGGERHIT_H 1

#include "TObject.h"
#include "TVector3.h"

#include "ShipHit.h"
#include "MuonTaggerPoint.h"


class MuonTaggerHit : public ShipHit
{
  public:

    /** Default constructor **/
    MuonTaggerHit();

    /** Constructor with arguments
     *@param detID    Detector ID
     *@param digi      digitized/measured TDC 
     *@param flag      True/False, false if there is another hit with smaller tdc 
     **/
    MuonTaggerHit(Int_t detID, Float_t digi, Bool_t isValid);
    MuonTaggerHit(MuonTaggerPoint* p, Double_t t0);
    //I need to pass from the true MC position to the centre of tile
    Int_t DetIDfromXYZ(TVector3 p); //provide mapping, true xyz to detectorID
    TVector3 XYZfromDetID(Int_t detID);  // return centre of muon tile  
/** Destructor **/
    virtual ~MuonTaggerHit();

    /** Output to screen **/
    virtual void Print() const;
//
    TVector3 getPos() {return XYZfromDetID(fDetectorID);}
    Bool_t isValid() const {return hisV;}
//
    Double_t SetMuonTimeRes(Double_t mcTime); // return tdc
    void setValidity(Bool_t isValid);
//
  private:
    /** Copy constructor **/
    MuonTaggerHit(const MuonTaggerHit& point);
    MuonTaggerHit operator=(const MuonTaggerHit& point);

    Float_t flag;   ///< flag

    static bool onlyOnce;
    void stInit(); // stations init
//
    Bool_t hisV;
//
ClassDef(MuonTaggerHit,3)
};
#endif  //MUONTAGGERHIT.H
