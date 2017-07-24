//
// Created by lucas on 14/07/17.
//

#include <RunAction.hh>
#include "DetectorConstruction.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithADouble.hh"
#include "DetectorMessenger.hh"
#include "G4UIcmdWithAString.hh"
//#include "G4UserRunAction.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4SystemOfUnits.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction * det)
:detectorConstruction(det)
{
    dir = new G4UIdirectory("/tpx/");
    dir->SetGuidance("Change the solid properties.");

    heightCmd = new G4UIcmdWithADouble("/tpx/height",this);
    heightCmd->SetGuidance("Change the solid height.");
    heightCmd->SetGuidance(" (300um is default)");
    heightCmd->SetParameterName("height in um",true,true);
    heightCmd->SetDefaultValue(300*um);
    heightCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    matCmd = new G4UIcmdWithAString("/tpx/mat",this);
    matCmd->SetGuidance("Change the solid material.");
    matCmd->SetGuidance(" (G4_Si is default)");
    matCmd->SetParameterName("new material",true,true);
    matCmd->SetDefaultValue("G4_si");
    matCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
    delete heightCmd;
    delete matCmd;
    delete dir;
}

void DetectorMessenger::SetNewValue(
        G4UIcommand * command,G4String newValues)
{
    RunAction * run = (RunAction *) G4RunManager::GetRunManager()->GetUserRunAction();
    if( command==heightCmd )
    {
        if(newValues.strip() != "")
        {
            detectorConstruction->SetZ((heightCmd->ConvertToDouble(newValues))*um);
            run->setHeight((heightCmd->ConvertToDouble(newValues))*um);
        }
    }
    if( command==matCmd )
    {
        if(newValues.strip() != "")
        {
            detectorConstruction->SetMat(newValues);
            run->setMaterial(newValues);
        }
    }
}

G4String DetectorMessenger::GetCurrentValue(G4UIcommand * command)
{
    G4String cv;
    if( command==heightCmd )
    {
        cv = heightCmd->ConvertToString(detectorConstruction->GetZ());
    }
    if( command==matCmd )
    {
        cv = detectorConstruction->GetMat();
    }
    return cv;
}
