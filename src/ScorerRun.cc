#include "ScorerRun.hh"
#include "G4Event.hh"
#include "G4StatDouble.hh"
#include "G4HCofThisEvent.hh"

ScorerRun::ScorerRun()
{}

ScorerRun::~ScorerRun()
{
	RunMap[0]->clear();
	RunMap[1]->clear();
}

void ScorerRun::RecordEvent(const G4Event* event)
{
	G4HCofThisEvent* HCE = event->GetHCofThisEvent();
	if (!HCE) return;
	for (size_t i = 0; i < 2; i++)
	{
		G4THitsMap<G4double>* eventmap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(i));
		 if(eventmap) *RunMap[i] += *eventmap;
	}
	G4Run::RecordEvent(event);
}

void ScorerRun::Merge(const G4Run* run)
{
	ScorerRun* localrun = (ScorerRun*)run;
	for (size_t i = 0; i < 2; i++)
	{
		*RunMap[i] += *(localrun->RunMap[i]);
	}
	G4Run::Merge(run);
}
