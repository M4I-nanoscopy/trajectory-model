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
// $Id: B1PrimaryGeneratorAction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include <DetectorConstruction.hh>
#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Electron.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0),
  fEnvelopeBox(0)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);
    //fEnvelopeBox = new G4Box("Box",110*um,110*um,20*um);
  // Finally, the processes are added to the particles' process manager:
  G4ParticleDefinition* particle = G4Electron::Electron();
  // default particle kinematic
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,-1.));
  fParticleGun->SetParticleEnergy(200.*keV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of each event
  //

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get Envelope volume
  // from G4LogicalVolumeStore.

  //G4double envSizeXY = 0;
  //G4double envSizeZ = 0;
  G4LogicalVolume* envLV
          = G4LogicalVolumeStore::GetInstance()->GetVolume("Shape1");
  fEnvelopeBox = 0;
  if ( envLV ) fEnvelopeBox = dynamic_cast<G4Box*>(envLV->GetSolid());

  if ( fEnvelopeBox ) {
    //envSizeXY = fEnvelopeBox->GetXHalfLength();
    //envSizeZ = fEnvelopeBox->GetZHalfLength();
      /*fEnvelopeBox->SetXHalfLength(110*um);
      fEnvelopeBox->SetYHalfLength(110*um);
      fEnvelopeBox->SetZHalfLength(20*um);*/
  }
  else  {
    G4ExceptionDescription msg;
    msg << "Envelope volume of box shape not found.\n";
    msg << "Perhaps you have changed geometry.\n";
    msg << "The gun will be place at the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
     "MyCode0002",JustWarning,msg);
  }
  // let's choose some random points on the upper surface
  G4float x = static_cast <G4float> (rand()) /
          (static_cast <G4float> (RAND_MAX/(2*fEnvelopeBox->GetXHalfLength())));
  G4float y = static_cast <G4float> (rand()) /
          (static_cast <G4float> (RAND_MAX/(2*fEnvelopeBox->GetYHalfLength())));
  G4ThreeVector * vect = new G4ThreeVector(x,
                                           y,
                                           fEnvelopeBox->GetZHalfLength()+700*um);
  /*G4ThreeVector vect = fEnvelopeBox->GetPointOnSurface();
  vect.set(0.9*vect.getX()+(fEnvelopeBox->GetXHalfLength()),
           0.9*vect.getY()+(fEnvelopeBox->GetYHalfLength()),
           fEnvelopeBox->GetZHalfLength()+700*um);*/
  //vect.setZ(fEnvelopeBox->GetZHalfLength()+700);
  fParticleGun->SetParticlePosition(*vect);
  /*G4double x0 = 2.5 * 55 * um;
  G4double y0 = 2.5 * 55 * um;
  G4double z0 = 10 * um;

  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));*/
  /*G4double x0 = 0;
  G4double y0 = 0;
  G4double z0 = 700 * um;

  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));*/

  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
