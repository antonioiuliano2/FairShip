#ifndef SCIFIHIT_H
#define SCIFIHIT_H 1

#include "TObject.h"
#include "TVector3.h"

#include "ShipHit.h"
#include "SpectrometerPoint.h"


class SciFiHit : public ShipHit
{
  public:

    /** Default constructor **/
    SciFiHit();

    /** Constructor with arguments
     *@param detID    Detector ID
     *@param digi      digitized/measured TDC 
     *@param flag      True/False, false if there is another hit with smaller tdc 
     **/
    SciFiHit(Int_t detID, Float_t digi, Bool_t isValid);
    SciFiHit(SpectrometerPoint* p, Double_t t0);
    //I need to pass from the true MC position to the centre of tile
    Int_t DetIDfromXYZ(TVector3 p); //provide mapping, true xyz to detectorID
    TVector3 XYZfromDetID(Int_t detID);  // return centre of muon tile  
/** Destructor **/
    virtual ~SciFiHit();

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
    SciFiHit(const SciFiHit& point);
    SciFiHit operator=(const SciFiHit& point);

    Float_t flag;   ///< flag

    static bool onlyOnce;
    void Init(); // initialization if required
//
    Bool_t hisV;
//
ClassDef(SciFiHit,3)
};
#endif  //SCIFIHIT.H
