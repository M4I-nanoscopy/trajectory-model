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
// $Id: B1DetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include <G4SolidStore.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#include <G4GeometryManager.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4UserRunAction.hh>
#include <G4PVReplica.hh>
#include <QtGui/QWidgetSet>
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"

#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"


#include "G4UserLimits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0),
  fPhysicalVolume(0),
  logicWorld(0),
  env_sizeXY(55*4*um),
  env_sizeZ(300*um),
  material("G4_Si")
{
    mes = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete mes;
    delete fScoringVolume;
    delete fPhysicalVolume;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Four by four pixels
  //G4double env_sizeXY = 55 * 4 * um, env_sizeZ = 300 * um;

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double density     = CLHEP::universe_mean_density;
  G4double pressure    = 1.e-9*pascal;
  G4double temperature = 293*kelvin;
  G4String name;
  G4double a,z;

  G4Material* vacuum = new G4Material(name="Galactic", z=1., a=1.01*g/mole, density,
                   kStateGas,temperature,pressure);

  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;

  G4Box* solidWorld =
    new G4Box("World",                       //its name
       world_sizeXY, world_sizeXY, world_sizeZ);     //its size

    /*G4Box* solidWorld1 =
            new G4Box("Volume",                       //its name
                      0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size*/

    logicWorld = new G4LogicalVolume(solidWorld,          //its solid
                        vacuum,           //its material
                        "World");            //its name

    /*G4LogicalVolume* logicWorld1 =
            new G4LogicalVolume(solidWorld1,          //its solid
                                vacuum,           //its material
                                "Volume");            //its name*/

    fPhysicalVolume = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,0,0),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  //
  // Shape 1: the timepix detector
  //
  G4Material* shape1_mat = nist->FindOrBuildMaterial(material);
  //G4ThreeVector pos1 = G4ThreeVector(0, 0, -10*um);
    G4ThreeVector pos1 = G4ThreeVector(0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ);
  // Box shape filling the envelope
  G4Box* solidShape1 =
    new G4Box("Shape1", 0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ);

  G4LogicalVolume* logicShape1 =
    new G4LogicalVolume(solidShape1,         //its solid
                        shape1_mat,          //its material
                        "Shape1");           //its name

  new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicShape1,             //its logical volume
                    "Shape1",                //its name
                    logicWorld,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking

  // Set
  G4double maxStep;
  logicShape1->SetUserLimits(new G4UserLimits(maxStep = 100*nm));

  // Set Shape1 as scoring volume
  //
  fScoringVolume = logicShape1;
    logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
  //
  //always return the physical World
  //
  return fPhysicalVolume;
}

void DetectorConstruction::SetZ(G4double d) {
    env_sizeZ = d;
    RefreshVis();
}

G4double DetectorConstruction::GetZ() {
    return env_sizeZ;
}

void DetectorConstruction::RefreshVis() {
    G4GeometryManager::GetInstance()->OpenGeometry(fPhysicalVolume);
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

void DetectorConstruction::SetMat(G4String mat) {
    material = mat;
    RefreshVis();
}

G4String DetectorConstruction::GetMat() {
    return material;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
