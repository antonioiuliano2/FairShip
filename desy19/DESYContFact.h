#ifndef DESYContFact_H
#define DESYContFact_H

#include "FairContFact.h"               // for FairContFact, etc

#include "Rtypes.h"                     // for ShipPassiveContFact::Class, etc

class FairParSet;

class DESYContFact : public FairContFact
{
  private:
    void setAllContainers();
  public:
    DESYContFact();
    ~DESYContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef(DESYContFact,0) // Factory for all Passive parameter containers
};

#endif  /* !PNDPASSIVECONTFACT_H */
