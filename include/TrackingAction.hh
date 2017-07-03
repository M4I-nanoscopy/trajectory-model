#ifndef TRACKINGACTION_H
#define TRACKINGACTION_H

#include <G4LogicalVolume.hh>
#include "G4UserTrackingAction.hh"
#include "globals.hh"

class EventAction;

class TrackingAction : public G4UserTrackingAction
{
 public:
  TrackingAction(EventAction* eventAction);
  virtual ~TrackingAction();

  // method from the base class
  virtual void PostUserTrackingAction(const G4Track*);

 private:

  EventAction*  fEventAction;
};


#endif //TRACKINGACTION_H
