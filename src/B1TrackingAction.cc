
#include "B1TrackingAction.hh"
#include "B1EventAction.hh"

#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "H5Cpp.h"
#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif



B1TrackingAction::B1TrackingAction(B1EventAction* eventAction) : G4UserTrackingAction(), fEventAction(eventAction)
{
}
B1TrackingAction::~B1TrackingAction() {

}
void B1TrackingAction::PostUserTrackingAction(const G4Track*)
{
}
