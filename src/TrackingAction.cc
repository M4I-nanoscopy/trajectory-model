
#include "TrackingAction.hh"
#include "EventAction.hh"

#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "H5Cpp.h"
#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif



TrackingAction::TrackingAction(EventAction* eventAction) : G4UserTrackingAction(), fEventAction(eventAction)
{
}
TrackingAction::~TrackingAction() {

}
void TrackingAction::PostUserTrackingAction(const G4Track*)
{
}
