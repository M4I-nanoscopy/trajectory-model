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
// $Id: B1EventAction.hh 93886 2015-11-03 08:28:26Z gcosmo $
//
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "H5Cpp.h"
#ifndef H5_NO_NAMESPACE
  using namespace H5;
#endif

const int   FSPACE_RANK = 2;    // Dataset rank as it is stored in the file
const int   FSPACE_DIM1 = 4096;    // Dimension sizes of the dataset as it is
const int   FSPACE_DIM2 = 7;

class RunAction;

/// Event action class
///

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddEdep(G4double edep) { fEdep += edep; }

    void AddTrackStep(int step, G4double x, G4double y, G4double z, G4double t, G4double energy, G4double velocity, G4double length);
    void setIsOut(bool b);
    void setHasAlreadyHit(bool c);
    bool getHasAlreadyHit();
    bool getIsOut();

  private:
    RunAction* fRunAction;
    G4double     fEdep;

    DataSet* dataSet;
    double *trajectory;
    int maxStep;
    bool isOut;
    bool hasAlreadyHit;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
