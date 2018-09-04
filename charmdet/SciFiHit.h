#ifndef SCIFIHIT_H
#define SCIFIHIT_H 1

#include "TObject.h"
#include "TVector3.h"

#include "ShipHit.h"

class SciFiHit : public ShipHit {
public:
   SciFiHit() : ShipHit() {}
   ~SciFiHit() = default;
   SciFiHit(Int_t detID, Float_t digi);
   void EndPoints(TVector3 &vbot, TVector3 &vtop);

private:
    SciFiHit(const SciFiHit& other); 
    SciFiHit operator=(const SciFiHit& other); 

   ClassDef(SciFiHit, 1)
};

#endif  //SCIFIHIT.H
