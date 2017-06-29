#ifndef B1_B1TRACKINGACTION_H
#define B1_B1TRACKINGACTION_H

#include <G4LogicalVolume.hh>
#include "G4UserTrackingAction.hh"
#include "globals.hh"

class B1EventAction;

class B1TrackingAction : public G4UserTrackingAction
{
 public:
  B1TrackingAction(B1EventAction* eventAction);
  virtual ~B1TrackingAction();

  // method from the base class
  virtual void PostUserTrackingAction(const G4Track*);

 private:

  B1EventAction*  fEventAction;
};


#endif //B1_B1TRACKINGACTION_H
