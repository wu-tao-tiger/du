#pragma once

#include "G4Run.hh"
#include "G4THitsMap.hh"

class G4Event;
class G4StatDouble;

class ScorerRun :public G4Run
{
public:
	ScorerRun();
	~ScorerRun();

	void RecordEvent(const G4Event*);
	void Merge(const G4Run*);

	G4THitsMap<G4StatDouble>** getRunMap() { return RunMap; }

private:
	G4THitsMap<G4StatDouble>* RunMap[2] = { new G4THitsMap<G4StatDouble>("dose_detector", "dose_scorer") ,new G4THitsMap<G4StatDouble>("dose_detector2", "dose_scorer2") };

};