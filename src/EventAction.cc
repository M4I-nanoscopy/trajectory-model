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
// $Id: B1EventAction.cc 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include <G4SIunits.hh>
#include <hdf5_hl.h>
#include <G4DigiManager.hh>
#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "DetectorConstructionBase.hh"
#include "Run.hh"
#include "DetectorSD.hh"
#include "HistoManager.hh"

#include "DigitizerWeightField.hh"
#include "G4GenericMessenger.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fEdep(0.),
  isOut(false),
  hasAlreadyHit(false),
  fSensorHCID(-1),
  fPrintModulo(10000),
  digitizerName("")
{
  G4RunManager *fRM = G4RunManager::GetRunManager();
  DetectorConstructionBase *myDet = (DetectorConstructionBase *)(fRM->GetUserDetectorConstruction());

  digitizerName = myDet->GetDigitizerName();

  G4DigiManager *digiManager = G4DigiManager::GetDMpointer();

  //build the digitizer
  if (digitizerName == "WFDigitizer") {
    DigitizerWeightField *wfDigitizer = new DigitizerWeightField("WFDigitizer");
    digiManager->AddNewModule(wfDigitizer);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{
    G4int eventID = event->GetEventID();
    if (eventID % fPrintModulo == 0 && eventID != 0) {
        G4cout << "\n---> Begin of event: " << eventID << G4endl;
    }
    fEdep = 0.;
    isOut = false;
    hasAlreadyHit = false;
    // Initialize trajectory storage
    trajectory = new double[FSPACE_DIM1 * FSPACE_DIM2];
    std::fill_n(trajectory, FSPACE_DIM1 * FSPACE_DIM2, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
    // Print per event (modulo n)
    G4int eventID = event->GetEventID();
    if (eventID % fPrintModulo == 0 && eventID != 0) {
        G4cout << "---> End of event: " << eventID << G4endl;
    }

    if (eventID < 1) {
        fRunAction->InitFile(event->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy());
    }

    // call digitizer after every event
    G4DigiManager *digiManager = G4DigiManager::GetDMpointer();
    DigitizerWeightField *digiModule = static_cast<DigitizerWeightField *>(digiManager->FindDigitizerModule(digitizerName));
    if (digiModule) {
        digiModule->Digitize();
    }

    if (!isOut && hasAlreadyHit) {// if the electron track ends into the solid,
                                // we take this track into account
        const H5std_string DATASET_NAME_TRAJ(
              "/trajectories/" + std::to_string(fRunAction->GetKeptElectrons()));
        // accumulate statistics in run action
        fRunAction->AddKeptElectron();
        fRunAction->AddEdep(fEdep);
        // Get output file
        H5File * file = fRunAction->GetOutputFile();
        // Setup and write hdf5 dataset, using the EventID as dataset name
        hsize_t fDim[] = {(hsize_t) maxStep, FSPACE_DIM2};
        DataSpace fSpace(FSPACE_RANK, fDim);

        dataSet = new DataSet(file->createDataSet(DATASET_NAME_TRAJ.c_str(), PredType::NATIVE_DOUBLE, fSpace));

        dataSet->write(trajectory, PredType::NATIVE_DOUBLE, fSpace, fSpace);
    }
    delete trajectory;
    //delete dataSet;

    if (fEdep > 0.) {
        //fill histogram with total energy per event
        G4AnalysisManager::Instance()->FillH1(2, fEdep);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddTrackStep(int step, G4double x, G4double y, G4double z,
                               G4double t, G4double energy, G4double velocity, G4double length)
{
  // TODO: need to handle this overflow better, not sure when it happens
  if ( step > FSPACE_DIM1 ) return;

  // Position in trajectory array
  int p = (step - 1) * FSPACE_DIM2;

  // TODO: This unit conversion, not sure if is the GEANT4 way to do it
  trajectory[p + 0] = double(x * mm / nm);
  trajectory[p + 1] = double(y * mm / nm);
  trajectory[p + 2] = double(z * mm / nm);
  trajectory[p + 3] = double(energy * MeV / keV);
  trajectory[p + 4] = double(t); // ns
  trajectory[p + 5] = double(velocity * (mm/ns) / (um/ns) );
  trajectory[p + 6] = double(length * mm / nm );

  maxStep = step;
}

void EventAction::setIsOut(bool b) {
  isOut = b;
}

void EventAction::setHasAlreadyHit(bool c) {
  hasAlreadyHit = c;
}

bool EventAction::getHasAlreadyHit() {
  return hasAlreadyHit;
}

bool EventAction::getIsOut() {
  return isOut;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
