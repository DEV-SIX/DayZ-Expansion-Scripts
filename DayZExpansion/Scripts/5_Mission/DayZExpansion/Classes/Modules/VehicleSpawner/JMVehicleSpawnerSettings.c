/**
 * JMVehicleSpawnerSettings.c
 *
 * DayZ Expansion Mod
 * www.dayzexpansion.com
 * © 2020 DayZ Expansion Mod Team
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/4.0/.
 *
*/

modded class JMVehicleSpawnerSettings
{
	override void Defaults()
	{
		super.Defaults();

		DefaultExpansionMerlin();
		DefaultExpansionMh6();
		DefaultExpansionUh1h();
		DefaultExpansionGyro();

		//! DefaultExpansionForklift();
		DefaultExpansionTractor();
		DefaultExpansionBus();

		DefaultExpansionUAZ();
		DefaultExpansionUAZRoofless();
		DefaultExpansionUAZCargoRoofless();
		DefaultExpansionUAZCargo();

		DefaultExpansionVodnik();
		//! DefaultExpansionAmphibiaBoat();

		DefaultExpansionZodiacBoat();
		DefaultExpansionUtilityBoat();
		DefaultExpansionLHD();

		DefaultExpansionAN2();
		//! DefaultExpansionSU34();
		DefaultExpansionC130J();

		//! DefaultExpansionQuadATV();
		//! DefaultExpansionTT650();
		//! DefaultExpansion250N();

		DefaultExpansionCivilianSedanTest();
		DefaultExpansionUAZCargoRooflessTest();
	}

	void DefaultExpansionMerlin()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionHelicopterBattery");
		attArr.Insert("ExpansionHydraulicHoses");
		attArr.Insert("ExpansionIgniterPlug");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionMerlinFrontWheel");
		attArr.Insert("ExpansionMerlinFrontWheel");
		attArr.Insert("ExpansionMerlinBackWheel");
		attArr.Insert("ExpansionMerlinBackWheel");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionMerlin";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionMh6()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionHelicopterBattery");
		attArr.Insert("ExpansionIgniterPlug");
		attArr.Insert("ExpansionHydraulicHoses");
		attArr.Insert("HeadlightH7");

		attArr.Insert("Expansion_Mh6_Door_1_1");
		attArr.Insert("Expansion_Mh6_Door_1_2");
		attArr.Insert("Expansion_Mh6_Door_2_1");
		attArr.Insert("Expansion_Mh6_Door_2_2");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionMh6";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionUh1h()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionHelicopterBattery");
		attArr.Insert("ExpansionHydraulicHoses");
		attArr.Insert("ExpansionIgniterPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionUh1h";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}
	
	void DefaultExpansionGyro()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionHelicopterBattery");
		attArr.Insert("SparkPlug");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionGyrocopter";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionQuad()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionHelicopterBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionQuad";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	/*
	void DefaultExpansionForklift()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionForkliftWheel");
		attArr.Insert("ExpansionForkliftWheel");
		attArr.Insert("ExpansionForkliftWheel");
		attArr.Insert("ExpansionForkliftWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionForklift";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}
	*/

	void DefaultExpansionTractor()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionTractorFrontWheel");
		attArr.Insert("ExpansionTractorFrontWheel");
		attArr.Insert("ExpansionTractorBackWheel");
		attArr.Insert("ExpansionTractorBackWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionTractorDoorsDriver");
		attArr.Insert("ExpansionTractorDoorsCodriver");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionTractor";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionUAZ()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionUAZDoorDriver");
		attArr.Insert("ExpansionUAZDoorCoDriver");
		attArr.Insert("ExpansionUAZDoorCargo1");
		attArr.Insert("ExpansionUAZDoorCargo2");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionUAZ";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionUAZRoofless()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionUAZDoorDriver");
		attArr.Insert("ExpansionUAZDoorCoDriver");
		attArr.Insert("ExpansionUAZDoorCargo1");
		attArr.Insert("ExpansionUAZDoorCargo2");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionUAZRoofless";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionUAZCargo()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionUAZDoorDriver");
		attArr.Insert("ExpansionUAZDoorCoDriver");
		attArr.Insert("ExpansionUAZDoorCargo1");
		attArr.Insert("ExpansionUAZDoorCargo2");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionUAZCargo";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionUAZCargoRoofless()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("ExpansionUAZWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionUAZDoorDriver");
		attArr.Insert("ExpansionUAZDoorCoDriver");
		attArr.Insert("ExpansionUAZDoorCargo1");
		attArr.Insert("ExpansionUAZDoorCargo2");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionUAZCargoRoofless";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionQuadATV()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionQuadATVWheelFront");
		attArr.Insert("ExpansionQuadATVWheelFront");
		attArr.Insert("ExpansionQuadATVWheelBack");
		attArr.Insert("ExpansionQuadATVWheelBack");
		attArr.Insert("HeadlightH7");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionQuadAtv";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionVodnik()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionVodnikWheel");
		attArr.Insert("ExpansionVodnikWheel");
		attArr.Insert("ExpansionVodnikWheel");
		attArr.Insert("ExpansionVodnikWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionVodnikDoorDriver");
		attArr.Insert("ExpansionVodnikDoorCoDriver");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionVodnik";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	/*
	void DefaultExpansionAmphibiaBoat()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionUtilityBoatWheel");
		attArr.Insert("ExpansionUtilityBoatWheel");
		attArr.Insert("ExpansionUtilityBoatWheel");
		attArr.Insert("ExpansionUtilityBoatWheel");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionAmphibiaBoat";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}
	*/

	void DefaultExpansionBus()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionBusWheel");
		attArr.Insert("ExpansionBusWheel");
		attArr.Insert("ExpansionBusWheelDouble");
		attArr.Insert("ExpansionBusWheelDouble");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("TruckBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionBus";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionLHD()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");
		attArr.Insert("TruckBattery");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionLHD";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionZodiacBoat()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");
		attArr.Insert("TruckBattery");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionZodiacBoat";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionUtilityBoat()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");
		attArr.Insert("TruckBattery");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionUtilityBoat";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionAN2()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionAnWheel");
		attArr.Insert("ExpansionAnWheel");
		attArr.Insert("ExpansionAnWheelSmall");
		attArr.Insert("ExpansionAnWheelSmall");
		
		attArr.Insert("ExpansionAircraftBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionAn2";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	/*
	void DefaultExpansionSU34()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionAnWheel");
		attArr.Insert("ExpansionAnWheel");
		attArr.Insert("ExpansionAnWheelSmall");
		attArr.Insert("ExpansionAnWheelSmall");
		
		attArr.Insert("ExpansionAircraftBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionSu34";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}
	*/

	void DefaultExpansionC130J()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionC130JWheel");
		attArr.Insert("ExpansionC130JWheel");
		attArr.Insert("ExpansionC130JWheel");
		attArr.Insert("ExpansionC130JWheel");
		
		attArr.Insert("ExpansionAircraftBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionC130J";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	/*
	void DefaultExpansionTT650()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionTT650Wheel");
		attArr.Insert("ExpansionTT650Wheel");
		attArr.Insert("ExpansionAircraftBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("GlowPlug");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionTT650";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}
	*/

	/*
	void DefaultExpansion250N()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("Expansion_250N_Wheel_Front");
		attArr.Insert("Expansion_250N_Wheel_Back");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "Expansion250N";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}
	*/

	void DefaultExpansionCivilianSedanTest()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("CivSedanHood");
		attArr.Insert("CivSedanTrunk");
		attArr.Insert("CivSedanDoors_Driver");
		attArr.Insert("CivSedanDoors_BackRight");
		attArr.Insert("CivSedanDoors_BackLeft");
		attArr.Insert("CivSedanDoors_CoDriver");
		attArr.Insert("ExpansionCivSedanWheel");
		attArr.Insert("ExpansionCivSedanWheel");
		attArr.Insert("ExpansionCivSedanWheel");
		attArr.Insert("ExpansionCivSedanWheel");
		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionCivilianSedanTest";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	void DefaultExpansionUAZCargoRooflessTest()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("ExpansionUAZWheelTest");
		attArr.Insert("ExpansionUAZWheelTest");
		attArr.Insert("ExpansionUAZWheelTest");
		attArr.Insert("ExpansionUAZWheelTest");
		attArr.Insert("HeadlightH7");
		attArr.Insert("HeadlightH7");

		attArr.Insert("ExpansionUAZDoorDriver");
		attArr.Insert("ExpansionUAZDoorCoDriver");
		attArr.Insert("ExpansionUAZDoorCargo1");
		attArr.Insert("ExpansionUAZDoorCargo2");

		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "ExpansionUAZCargoRooflessTest";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	override void Default_CivilianSedan()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("Expansion_CivSedanHood");
		attArr.Insert("Expansion_CivSedanTrunk");
		attArr.Insert("Expansion_CivSedanDoors_Driver");
		attArr.Insert("Expansion_CivSedanDoors_BackRight");
		attArr.Insert("Expansion_CivSedanDoors_BackLeft");
		attArr.Insert("Expansion_CivSedanDoors_CoDriver");
		attArr.Insert("CivSedanWheel");
		attArr.Insert("CivSedanWheel");
		attArr.Insert("CivSedanWheel");
		attArr.Insert("CivSedanWheel");
		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "CivilianSedan";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	override void Default_OffroadHatchback()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("Expansion_HatchbackHood");
		attArr.Insert("Expansion_HatchbackTrunk");
		attArr.Insert("Expansion_HatchbackDoors_Driver");
		attArr.Insert("Expansion_HatchbackDoors_CoDriver");
		attArr.Insert("HatchbackWheel");
		attArr.Insert("HatchbackWheel");
		attArr.Insert("HatchbackWheel");
		attArr.Insert("HatchbackWheel");
		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "OffroadHatchback";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	override void Default_Hatchback_02()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("Expansion_Hatchback_02_Hood");
		attArr.Insert("Expansion_Hatchback_02_Trunk");
		attArr.Insert("Expansion_Hatchback_02_Door_1_1");
		attArr.Insert("Expansion_Hatchback_02_Door_1_2");
		attArr.Insert("Expansion_Hatchback_02_Door_2_1");
		attArr.Insert("Expansion_Hatchback_02_Door_2_2");
		attArr.Insert("Hatchback_02_Wheel");
		attArr.Insert("Hatchback_02_Wheel");
		attArr.Insert("Hatchback_02_Wheel");
		attArr.Insert("Hatchback_02_Wheel");
		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "Hatchback_02";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}

	override void Default_Sedan_02()
	{
		ref array< string> attArr = new array< string>;

		attArr.Insert("Expansion_Sedan_02_Hood");
		attArr.Insert("Expansion_Sedan_02_Trunk");
		attArr.Insert("Expansion_Sedan_02_Door_1_1");
		attArr.Insert("Expansion_Sedan_02_Door_1_2");
		attArr.Insert("Expansion_Sedan_02_Door_2_1");
		attArr.Insert("Expansion_Sedan_02_Door_2_2");
		attArr.Insert("Sedan_02_Wheel");
		attArr.Insert("Sedan_02_Wheel");
		attArr.Insert("Sedan_02_Wheel");
		attArr.Insert("Sedan_02_Wheel");
		attArr.Insert("CarBattery");
		attArr.Insert("CarRadiator");
		attArr.Insert("SparkPlug");

		ref JMVehicleSpawnerSerialize file = new JMVehicleSpawnerSerialize;

		file.VehicleName = "Sedan_02";
		file.m_FileName = file.VehicleName;
		file.Parts = attArr;

		Vehicles.Insert( file.VehicleName, file );
	}
}