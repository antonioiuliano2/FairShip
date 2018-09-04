#ifndef PIXELHIT_H
#define PIXELHIT_H 1

#include "TObject.h"
#include "TVector3.h"

#include "ShipHit.h"


class PIXELHit : public ShipHit {
public:
   PIXELHit() : ShipHit() {}
   ~PIXELHit() = default;
   PIXELHit(Int_t detID, Float_t digi);
   void EndPoints(TVector3 &vbot, TVector3 &vtop);

private:
    PIXELHit(const PIXELHit& other); 
    PIXELHit operator=(const PIXELHit& other); 

   ClassDef(PIXELHit, 1)
};

#endif
