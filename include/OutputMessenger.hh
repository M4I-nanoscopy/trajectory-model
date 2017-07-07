/*
 * OutputMessenger.hh
 *
 *  Created on: 7 juil. 2017
 *      Author: lucas
 */

#ifndef INCLUDE_OUTPUTMESSENGER_HH_
#define INCLUDE_OUTPUTMESSENGER_HH_

class G4UIcommand;
class G4UIcmdWithAString;


class RunAction;

class OutputMessenger : public G4UImessenger
{
	public:
    	OutputMessenger();
    	~OutputMessenger();

	public:
	    void SetNewValue(G4UIcommand * command,G4String newValues);
	    G4String GetCurrentValue(G4UIcommand * command);

	private:
	    RunAction * runAction;

	private:
	    G4UIdirectory * dir;
	    G4UIcmdWithAString * nameCmd;
};

#endif /* INCLUDE_OUTPUTMESSENGER_HH_ */
