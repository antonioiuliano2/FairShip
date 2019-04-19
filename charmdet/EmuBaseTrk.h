#ifndef EmuBaseTrk_H
#define EmuBaseTrk_H

#include "ShipHit.h"

/*! EMUBaseTrk: class to introduce real measurements effects in the simulation of tracks left in emulsion couples */

class EmuBaseTrk: public ShipHit {
    public:
     EmuBaseTrk(): ShipHit(){}
     ~EmuBaseTrk() = default;
     EmuBaseTrk(Int_t detID, Float_t digi);
     //accessors
     Double_t GetX() {return fx;};
     Double_t GetY() {return fy;};
     Double_t GetTX() {return fTX;};
     Double_t GetTY() {return fTY;};

     //modifiers
     void SetX(Double_t x){fx = x;};
     void SetY(Double_t y){fy = y;};
     void SetTX(Double_t TX){fTX = TX;};
     void SetTY(Double_t TY){fTY = TY;};

     void setInvalid() { flag = 0; };
     void setValid() { flag = 1; };
    private:
     EmuBaseTrk(const EmuBaseTrk & other);
     EmuBaseTrk operator= (const EmuBaseTrk &other);

     Double_t fx, fy; /*< average positions of the hit left by the track in the emulsion */
     Double_t fTX,fTY; /*< angles of the track*/
     

    ClassDef(EmuBaseTrk,1)
    int flag;
};

#endif
