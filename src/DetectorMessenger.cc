//
// Created by lucas on 14/07/17.
//

#include "DetectorConstruction.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithADouble.hh"
#include "DetectorMessenger.hh"
//#include "G4UserRunAction.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4SystemOfUnits.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction * det)
:detectorConstruction(det)
{
    dir = new G4UIdirectory("/tpx/");
    dir->SetGuidance("Change the solid height.");

    heightCmd = new G4UIcmdWithADouble("/tpx/height",this);
    heightCmd->SetGuidance("Change the solid height.");
    heightCmd->SetGuidance(" (300um is default)");
    heightCmd->SetParameterName("height in um",true,true);
    heightCmd->SetDefaultValue(300*um);
    //fmat1Cmd->SetParameterName("wMaterial",false);
    heightCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    //nameCmd->
}

DetectorMessenger::~DetectorMessenger()
{
    delete heightCmd;
    delete dir;
}

void DetectorMessenger::SetNewValue(
        G4UIcommand * command,G4String newValues)
{
    if( command==heightCmd )
    {
        if(newValues.strip() != "")
        {
            detectorConstruction->SetZ((heightCmd->ConvertToDouble(newValues))*um);
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
    return cv;
}
