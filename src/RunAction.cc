#include "RunAction.hh"
#include "ScorerRun.hh"
#include "G4AnalysisManager.hh"
#include "G4StatDouble.hh"

RunAction::RunAction()
{}

RunAction::~RunAction()
{}

G4Run* RunAction::GenerateRun()
{
	return new ScorerRun;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
	if (!IsMaster()) return;
	G4AnalysisManager* analysismanager = G4AnalysisManager::Instance();

	analysismanager->OpenFile("out.root");
	analysismanager->SetNtupleMerging(false, 0);
	analysismanager->CreateNtuple("zsphere", "dose");
	analysismanager->CreateNtupleDColumn("dose");
	analysismanager->CreateNtupleDColumn("error");
	analysismanager->CreateNtupleIColumn("entries");
	analysismanager->FinishNtuple();
	analysismanager->CreateNtuple("nucl", "dose");
	analysismanager->CreateNtupleDColumn("dose");
	analysismanager->CreateNtupleDColumn("error");
	analysismanager->CreateNtupleIColumn("entries");
	analysismanager->FinishNtuple();
}

void RunAction::EndOfRunAction(const G4Run* arun)
{
	if (!IsMaster()) return;
	G4int NOfEvent = arun->GetNumberOfEvent();
	ScorerRun* run = (ScorerRun*)arun;
	G4AnalysisManager* analysismanager = G4AnalysisManager::Instance();
	for (size_t i = 0; i < 2; i++)
	{
		G4THitsMap<G4StatDouble>* RunMap = run->getRunMap()[i];
		for (std::map<G4int,G4StatDouble*>::iterator iter = RunMap->GetMap()->begin(); iter != RunMap->GetMap()->end(); iter++)
		{
			G4double dose = iter->second->sum_wx();
			G4double d2 = iter->second->sum_wx2();
			G4double temp = d2 - std::pow(dose, 2) / NOfEvent;
			temp = std::max(0.0, temp);
			G4double error = std::sqrt(temp);
			G4int entries = iter->second->sum_w();
			
			analysismanager->FillNtupleDColumn(i, 0, dose);
			analysismanager->FillNtupleDColumn(i, 1, error);
			analysismanager->FillNtupleIColumn(i, 2, error);
			analysismanager->AddNtupleRow(i);
		}
	}
	analysismanager->Write();
	analysismanager->CloseFile();

}