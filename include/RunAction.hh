#pragma once

#include "G4UserRunAction.hh"

class RunAction : public G4UserRunAction
{
public:
	RunAction();
	~RunAction();
	G4Run* GenerateRun();
	void BeginOfRunAction(const G4Run*);
	void EndOfRunAction(const G4Run*);
};