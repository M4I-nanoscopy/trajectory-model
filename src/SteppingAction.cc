//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B1SteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"
#include "G4SteppingManager.hh"
#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(0)
{
    G4RunManager *fRM = G4RunManager::GetRunManager();
    myDet = (DetectorConstructionBase *)(fRM->GetUserDetectorConstruction());
    thickness = myDet->GetSensorThickness();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume) {
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detectorConstruction->GetScoringVolume();   
  }

  // Store the post step position in the event action
  G4StepPoint *stepPoint = step->GetPostStepPoint();
  G4ThreeVector stepPointPosition = stepPoint->GetPosition();
  EventAction *eventAction = (EventAction*) (G4RunManager::GetRunManager()->GetUserEventAction());
  eventAction->AddTrackStep(
    step->GetTrack()->GetCurrentStepNumber(),
    stepPointPosition.getX(),
    stepPointPosition.getY(),
    stepPointPosition.getZ(),
    stepPoint->GetGlobalTime(),
    stepPoint->GetKineticEnergy(),
    stepPoint->GetVelocity(),
    step->GetStepLength()
  );


  // get volume of the current step
  G4LogicalVolume* volume 
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
      
  // check if we are in scoring volume
  if (volume != fScoringVolume) { // the electron is out of the solid,
                                  // whether before or after the hit
    if (fEventAction->getHasAlreadyHit()) {
      if (!fEventAction->getIsOut()) { // it goes out
        fEventAction->setIsOut(true);
      }
    }
    return;
  }
  else {
    if (!fEventAction->getHasAlreadyHit()) { // it goes in
      fEventAction->setHasAlreadyHit(true);
    }
  }

  // collect energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();
    if (edepStep <= 0.) return;
  fEventAction->AddEdep(edepStep);







    //longitudinal profile of deposited energy
    G4ThreeVector prePoint  = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector postPoint = step->GetPostStepPoint()->GetPosition();
    G4ThreeVector point = prePoint + G4UniformRand() * (postPoint - prePoint);
    G4double r = point.z() + 0.5 * thickness; //FIXME get detector thickness


    // fill histograms
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    if( step->GetTrack()->GetVolume()->GetName() == "pixel_cell"){
        analysisManager->FillH1(1, r, edepStep);
    }



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

