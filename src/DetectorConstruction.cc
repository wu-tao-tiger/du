#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"


#include "G4Sphere.hh"
#include "G4PVReplica.hh"


#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSDoseDeposit.hh"
#include "G4SDManager.hh"
#include "G4PSDoseDeposit3D.hh"

DetectorConstruction::DetectorConstruction()
{}

DetectorConstruction::~DetectorConstruction()
{}

void DetectorConstruction::DefineMaterial()
{
	G4NistManager* nistmanager = G4NistManager::Instance();
	G4Material* air = nistmanager->FindOrBuildMaterial("G4_AIR");
	G4Material* water = nistmanager->FindOrBuildMaterial("G4_WATER");
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	DefineMaterial();
	G4VSolid* world_s = new G4Box("world_s", 50 * um, 50 * um, 50 * um);
	G4LogicalVolume* world_l = new G4LogicalVolume(world_s, G4Material::GetMaterial("G4_AIR"), "world_l");
	G4VPhysicalVolume* world_p = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), world_l, "world_p", nullptr, false, 0, true);

	G4VSolid* xslice_s = new G4Box("xslice_s", 5 * um, 50 * um, 50 * um);
	G4LogicalVolume* xslice_l = new G4LogicalVolume(xslice_s, G4Material::GetMaterial("G4_AIR"), "xslice_l");
	new G4PVReplica("xslice_p", xslice_l, world_p, kXAxis, 10, 10*um, 0);

	G4VSolid* yslice_s = new G4Box("yslice_s", 5 * um, 5 * um, 50 * um);
	G4LogicalVolume* yslice_l = new G4LogicalVolume(yslice_s, G4Material::GetMaterial("G4_AIR"), "yslice_l");
	new G4PVReplica("yslice_p", yslice_l, xslice_l, kYAxis, 10, 10 * um, 0);

	G4VSolid* zsphere_s = new G4Sphere("zsphere_s", 0, 5 * um, 0, 360 * deg, 0, 180 * deg);
	G4LogicalVolume* zsphere_l = new G4LogicalVolume(zsphere_s, G4Material::GetMaterial("G4_WATER"), "zsphere_l");
	new G4PVReplica("zsphere_p", zsphere_l, yslice_l, kZAxis, 10, 10 * um, 0);

	G4VSolid* nucl_s = new G4Sphere("nucl_s", 0, 3 * um, 0, 360 * deg, 0, 180 * deg);
	G4LogicalVolume* nucl_l = new G4LogicalVolume(nucl_s, G4Material::GetMaterial("G4_WATER"), "nucl_l");
	new G4PVPlacement(nullptr, G4ThreeVector(), nucl_l, "nucl_p", zsphere_l, false, 0, false);

	world_l->SetVisAttributes(G4VisAttributes::GetInvisible());
	xslice_l->SetVisAttributes(G4VisAttributes::GetInvisible());
	yslice_l->SetVisAttributes(G4VisAttributes::GetInvisible());
	zsphere_l->SetVisAttributes(new G4VisAttributes(G4Colour(0, 1, 0, 0.2)));


	return world_p;
}

void DetectorConstruction::ConstructSDandField()
{
	G4MultiFunctionalDetector* dose_detector = new G4MultiFunctionalDetector("dose_detector");
	G4VPrimitiveScorer* dose_scorer = new G4PSDoseDeposit3D("dose_scorer", 10, 10, 10, 2, 1, 0);
	dose_detector->RegisterPrimitive(dose_scorer);
	G4SDManager* sdmanager = G4SDManager::GetSDMpointer();
	sdmanager->AddNewDetector(dose_detector);
	SetSensitiveDetector("zsphere_l", dose_detector);

	G4MultiFunctionalDetector* dose_detector2 = new G4MultiFunctionalDetector("dose_detector2");
	G4VPrimitiveScorer* dose_scorer2 = new G4PSDoseDeposit3D("dose_scorer2", 10, 10, 10, 3, 2, 1);
	dose_detector2->RegisterPrimitive(dose_scorer2);
	sdmanager->AddNewDetector(dose_detector2);
	SetSensitiveDetector("nucl_l", dose_detector2);
}
