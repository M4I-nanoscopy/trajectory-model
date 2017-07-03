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
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include <G4SIunits.hh>
#include "B1EventAction.hh"
#include "B1RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::B1EventAction(B1RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fEdep(0.)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::~B1EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::BeginOfEventAction(const G4Event*)
{    
  fEdep = 0.;

  // Initialize trajectory storage
  trajectory = new double[FSPACE_DIM1 * FSPACE_DIM2];
  std::fill_n(trajectory, FSPACE_DIM1 * FSPACE_DIM2, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::EndOfEventAction(const G4Event* event)
{   
  // accumulate statistics in run action
  fRunAction->AddEdep(fEdep);

  // Get output file
  B1RunAction *runAction = (B1RunAction*) (G4RunManager::GetRunManager()->GetUserRunAction());
  H5File* file = runAction->GetOutputFile();

  // Setup and write hdf5 dataset, using the EventID as dataset name
  hsize_t fDim[] = {(hsize_t) maxStep, FSPACE_DIM2};
  DataSpace fSpace( FSPACE_RANK, fDim );
  dataSet = new DataSet(file->createDataSet(std::to_string(event->GetEventID()), PredType::NATIVE_DOUBLE, fSpace));
  dataSet->write( trajectory, PredType::NATIVE_DOUBLE, fSpace, fSpace );

  delete trajectory;
  //delete dataSet;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::AddTrackStep(int step, G4double x, G4double y, G4double z, G4double t, G4double energy, G4double velocity, G4double length)
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
