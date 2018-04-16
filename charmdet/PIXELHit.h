#ifndef PIXELHIT_H
#define PIXELHIT_H 1

#include "TObject.h"
#include "TVector3.h"

#include "ShipHit.h"
#include "SpectrometerPoint.h"


class PIXELHit : public ShipHit
{
  public:

    /** Default constructor **/
    PIXELHit();

    /** Constructor with arguments
     *@param detID    Detector ID
     *@param digi      digitized/measured TDC 
     *@param flag      True/False, false if there is another hit with smaller tdc 
     **/
    PIXELHit(Int_t detID, Float_t digi, Bool_t isValid);
    PIXELHit(SpectrometerPoint* p, Double_t t0);
    //I need to pass from the true MC position to the centre of tile
    Int_t DetIDfromXYZ(TVector3 p); //provide mapping, true xyz to detectorID
    TVector3 XYZfromDetID(Int_t detID);  // return centre of muon tile  
/** Destructor **/
    virtual ~PIXELHit();

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
    PIXELHit(const PIXELHit& point);
    PIXELHit operator=(const PIXELHit& point);

    Float_t flag;   ///< flag

    static bool onlyOnce;
    void Init(); // initialization if required
//
    Bool_t hisV;
//
ClassDef(PIXELHit,3)
};
#endif  //PIXELHIT.H
