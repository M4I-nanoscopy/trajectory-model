//
// Created by lucas on 14/07/17.
//

#ifndef TRAJECTORY_MODEL_DETECTORMESSENGER_HH
#define TRAJECTORY_MODEL_DETECTORMESSENGER_HH

#include <G4UImessenger.hh>

class G4UIcommand;
class G4UIcmdWithADouble;
class G4UIcmdWithAString;


class DetectorConstruction;

class DetectorMessenger : public G4UImessenger
{
public:
    DetectorMessenger(DetectorConstruction * det);
    ~DetectorMessenger();

public:
    void SetNewValue(G4UIcommand * command,G4String newValues);
    G4String GetCurrentValue(G4UIcommand * command);

private:
    DetectorConstruction * detectorConstruction;

private:
    G4UIdirectory * dir;
    G4UIcmdWithADouble * heightCmd;
    G4UIcmdWithAString * matCmd;
};

#endif //TRAJECTORY_MODEL_DETECTORMESSENGER_HH
