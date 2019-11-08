#include "EmuBaseTrk.h"

EmuBaseTrk::EmuBaseTrk(Int_t detID, Float_t digi) : ShipHit(detID, digi), 
                                                    fx(0.),
                                                    fy(0.),
                                                    fTX(0.),
                                                    fTY(0.),
						    fPdgCode(0),
                                                    fMCTrackID(-10),
                                                    fNFilm(0)
                                                    {}
