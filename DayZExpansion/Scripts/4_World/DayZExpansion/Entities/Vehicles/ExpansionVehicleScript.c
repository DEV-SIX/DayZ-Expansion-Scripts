/**
 * ExpansionVehicleScript.c
 *
 * DayZ Expansion Mod
 * www.dayzexpansion.com
 * © 2020 DayZ Expansion Mod Team
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/4.0/.
 *
*/

class ExpansionVehicleSyncState
{
	ExpansionVehicleScript m_Vehicle;

	int m_Time;
	int m_LastRecievedTime;
	int m_TimeDelta;

	vector m_Position;
	vector m_Orientation;
	vector m_LinearVelocity;
	vector m_AngularVelocity;

	ref Transform m_InitialTransform;
	ref Transform m_PredictedTransform;

	private float m_LinVelX;
	private float m_LinVelY;
	private float m_LinVelZ;

	private float m_AngVelX;
	private float m_AngVelY;
	private float m_AngVelZ;

	void ExpansionVehicleSyncState( ExpansionVehicleScript vehicle )
	{
		m_Vehicle = vehicle;

		m_InitialTransform = new Transform;
		m_PredictedTransform = new Transform;
	}

	void OnVariablesSynchronized()
	{
		m_LinearVelocity[0] = m_LinVelX;
		m_LinearVelocity[1] = m_LinVelY;
		m_LinearVelocity[2] = m_LinVelZ;

		m_AngularVelocity[0] = m_AngVelX;
		m_AngularVelocity[1] = m_AngVelY;
		m_AngularVelocity[2] = m_AngVelZ;
	}

	void SynchronizeVariables( ExpansionVehicleNetworkMode mode )
	{
		switch ( mode )
		{
		case ExpansionVehicleNetworkMode.SERVER_ONLY:
			return;
		case ExpansionVehicleNetworkMode.PREDICTION:
			m_LinVelX = m_LinearVelocity[0];
			m_LinVelY = m_LinearVelocity[1];
			m_LinVelZ = m_LinearVelocity[2];

			m_AngVelX = m_AngularVelocity[0];
			m_AngVelY = m_AngularVelocity[1];
			m_AngVelZ = m_AngularVelocity[2];
			return;
		case ExpansionVehicleNetworkMode.CLIENT:
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write( GetGame().GetTime() );
			rpc.Write( m_Position );
			rpc.Write( m_Orientation );
			rpc.Write( m_LinearVelocity );
			rpc.Write( m_AngularVelocity );
			rpc.Send( m_Vehicle, ExpansionVehicleRPC.ClientSync, false, NULL );
			return;
		}
	}
}

// #define EXPANSION_USING_TRANSPORT_BASE

#ifdef EXPANSION_USING_TRANSPORT_BASE
class ExpansionVehicleScript extends Transport
#else
class ExpansionVehicleScript extends ItemBase
#endif
{
	protected ref array< ref ExpansionVehicleAxle > m_Axles;
	protected ref array< ref ExpansionCrewData > m_Crew;

	protected ref array< ExpansionVehicleWheel > m_Wheels;

	protected ref ExpansionController m_Controller;
	protected bool m_HasDriver;
	
	// ------------------------------------------------------------
	//! Constant Values - Set in Constructor, Errors occur if not.
	// ------------------------------------------------------------
	protected float m_MaxSpeed; // (km/h)
	protected float m_MaxSpeedMS; // (m/s)

	protected float m_AltitudeFullForce; // (m)
	protected float m_AltitudeNoForce; // (m)

	float m_BodyMass;
	float m_InvMass;
	vector m_BodyCenterOfMass;

	protected bool m_IsPhysicsHost;
	bool m_PhysicsCreated;
	bool m_PhysicsDestroyed;
	
	protected float m_BoundingRadius;
	protected vector m_BoundingBox[2];

	protected EffectSound m_HornSound;

	protected string m_HornSoundSetINT = "Expansion_Horn_Int_SoundSet";
	protected string m_HornSoundSetEXT = "Expansion_Horn_Ext_SoundSet";

	protected bool m_Exploded;
	protected bool m_ExplodedSynchRemote;
	protected int m_ExplosionSize;

	protected bool m_HornPlaying;
	protected bool m_HornSynchRemote;
	
	protected bool m_SafeZone;
	protected bool m_SafeZoneSynchRemote;

	protected bool m_EngineIsOn;

	protected ExpansionVehicleLockState m_VehicleLockedState;

	protected int m_PersistentIDA = 0;
	protected int m_PersistentIDB = 0;
	protected int m_PersistentIDC = 0;
	protected int m_PersistentIDD = 0;

	protected float m_dmgContactCoef;

	protected ExpansionSoundProxyBase m_SoundProxy;

	vector m_LinearVelocity;
	vector m_AngularVelocity;

	vector m_LinearVelocityMS;
	vector m_AngularVelocityMS;

	vector m_AdjustCenterOfMass;

	ref Transform m_Transform;

	vector m_InertiaTensor;
	vector m_InvInertiaTensor;
	ref Matrix3 m_InvInertiaTensorWS;

	private ref ExpansionVehicleSyncState m_SyncState;
	private ExpansionVehicleNetworkMode m_NetworkMode;

	ref array< ref ExpansionPointLight > m_Lights;
	ref array< ref Particle > m_Particles;

	protected bool m_PlayCrashSoundLight;
	protected bool m_PlayCrashSoundHeavy;
	
	protected bool m_HeadlightsOn;
	protected bool m_HeadlightsState;
	protected bool m_BrakesArePressed;
	
	CarLightBase m_Headlight;
	CarRearLightBase m_RearLight;

	// ------------------------------------------------------------
	void ExpansionVehicleScript()
	{
		//! Print( "[" + this + "] ExpansionVehicleScript" );

		if ( IsMissionHost() && GetExpansionSettings().GetGeneral() )
		{
			m_NetworkMode = GetExpansionSettings().GetGeneral().VehicleSync;
		} else
		{
			m_NetworkMode = ExpansionVehicleNetworkMode.SERVER_ONLY;
		}

		SetFlags( EntityFlags.ACTIVE | EntityFlags.SOLID | EntityFlags.VISIBLE, false );
		SetEventMask( EntityEvent.SIMULATE | EntityEvent.POSTSIMULATE | EntityEvent.INIT | EntityEvent.FRAME | EntityEvent.PHYSICSMOVE );

		m_Axles = new array< ref ExpansionVehicleAxle >;
		m_Crew = new array< ref ExpansionCrewData >;

		m_Wheels = new array< ExpansionVehicleWheel >;

		string path = "";
		int i = 0;
		int count = 0;

		path = "CfgVehicles " + GetType() + " VehicleSimulation Axles";
		count = GetGame().ConfigGetChildrenCount( path );
		for ( i = 0; i < count; i++ )
		{
			string axleName;
			GetGame().ConfigGetChildName( path, i, axleName );

			string wheelPath = "CfgVehicles " + GetType() + " VehicleSimulation Axles " + axleName + " Wheels";
			int wheelCount = GetGame().ConfigGetChildrenCount( wheelPath );

			ref ExpansionVehicleAxle axle = NULL;
			
			if ( wheelCount == 1 )
			{
				axle = new ExpansionVehicleOneWheelAxle( this, axleName );

				axle.AddWheel( "Center", m_Wheels );
			} else if ( wheelCount == 2 )
			{
				axle = new ExpansionVehicleTwoWheelAxle( this, axleName );

				axle.AddWheel( "Left", m_Wheels );
				axle.AddWheel( "Right", m_Wheels );
			} else
			{
				axle = new ExpansionVehicleAxle( this, axleName );

				for ( int j = 0; j < wheelCount; j++ )
				{
					string wheelName;
					GetGame().ConfigGetChildName( wheelPath, j, wheelName );

					axle.AddWheel( wheelName, m_Wheels );
				}
			}

			m_Axles.Insert( axle );
		}

		path = "CfgVehicles " + GetType() + " Crew";
		count = GetGame().ConfigGetChildrenCount( path );
		for ( i = 0; i < count; i++ )
		{
			string crewName;
			GetGame().ConfigGetChildName( path, i, crewName );

			m_Crew.Insert( new ExpansionCrewData( this, crewName ) );
		}

		m_VehicleLockedState = ExpansionVehicleLockState.NOLOCK;

		m_SafeZone = false;

		string cHSSE = "hornSoundSetEXT";
		if ( ConfigIsExisting( cHSSE ) )
		{
			m_HornSoundSetEXT = ConfigGetString( cHSSE );
		} else
		{
			m_HornSoundSetEXT = "Expansion_Horn_Ext_SoundSet";
		}
		
		string cHSSI = "hornSoundSetINT";
		if ( ConfigIsExisting( cHSSI ) )
		{
			m_HornSoundSetINT = ConfigGetString( cHSSI );
		} else
		{		
			m_HornSoundSetINT = "Expansion_Horn_Int_SoundSet";
		}

		GetPersistentID( m_PersistentIDA, m_PersistentIDB, m_PersistentIDC, m_PersistentIDD );

		RegisterNetSyncVariableInt( "m_PersistentIDA" );
		RegisterNetSyncVariableInt( "m_PersistentIDB" );
		RegisterNetSyncVariableInt( "m_PersistentIDC" );
		RegisterNetSyncVariableInt( "m_PersistentIDD" );

		RegisterNetSyncVariableBool( "m_EngineIsOn" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
		RegisterNetSyncVariableBool( "m_ExplodedSynchRemote" ); 
		RegisterNetSyncVariableBool( "m_SafeZoneSynchRemote" ); 

		// RegisterNetSyncVariableBool( "m_IsWinched" );

		RegisterNetSyncVariableInt( "m_NetworkMode", ExpansionVehicleNetworkMode.SERVER_ONLY, ExpansionVehicleNetworkMode.COUNT );

		m_SyncState = new ExpansionVehicleSyncState( this );
		float min = 0;
		float max = 0;
		int prec = 8;

		// Unfortunately it has to be registered even if not used for client side vehicles
		//if ( m_NetworkMode == ExpansionVehicleNetworkMode.PREDICTION )
		//{
			RegisterNetSyncVariableFloat( "m_SyncState.m_LinVelX", min, max, prec );
			RegisterNetSyncVariableFloat( "m_SyncState.m_LinVelY", min, max, prec );
			RegisterNetSyncVariableFloat( "m_SyncState.m_LinVelZ", min, max, prec );

			RegisterNetSyncVariableFloat( "m_SyncState.m_AngVelX", min, max, prec );
			RegisterNetSyncVariableFloat( "m_SyncState.m_AngVelY", min, max, prec );
			RegisterNetSyncVariableFloat( "m_SyncState.m_AngVelZ", min, max, prec );
		//}

		if ( IsMissionClient() )
		{		
			string sound_controller_path = "CfgVehicles " + GetType() + " soundController";
			string sound_controller = GetGame().ConfigGetTextOut( sound_controller_path );
		
			if ( Class.CastTo( m_SoundProxy, GetGame().CreateObject( sound_controller, GetPosition(), true ) ) )
			{
				m_SoundProxy.SetPosition( "0 0 0" );
				AddChild( m_SoundProxy, -1 );
		
				m_SoundProxy.SetExpansionVehicle( this );
			}
		}

		m_Transform = new Transform;

		m_InvInertiaTensorWS = new Matrix3;

		// #ifdef EXPANSION_USING_TRANSPORT_BASE
		// OnCreatePhysics();
		// #endif

		LoadConstantVariables();

		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Call( DeferredInit );
	}

	// ------------------------------------------------------------
	void ~ExpansionVehicleScript()
	{
		if ( dBodyIsSet( this ) )
		{
			dBodyDestroy( this );
		}
	}

	// ------------------------------------------------------------
	void LoadConstantVariables()
	{
	}

	// ------------------------------------------------------------
	void OnAfterLoadConstantVariables()
	{
	}

	// ------------------------------------------------------------
	void DeferredInit()
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::DeferredInit - Start");
		#endif

		m_BoundingRadius = ClippingInfo( m_BoundingBox );

		m_MaxSpeedMS = m_MaxSpeed * ( 1.0 / 3.6 );

		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( OnAfterLoadConstantVariables, 100, false );

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::DeferredInit - End");
		#endif
	}

	// ------------------------------------------------------------
	override void OnCreatePhysics()
	{
		//! Print( "[" + this + "] OnCreatePhysics" );

		if ( m_NetworkMode == ExpansionVehicleNetworkMode.CLIENT )
		{
			if ( !IsMissionClient() )
			{
				// this is for when we are only doing client side physics and we are the server

				// TODO: add check to see if a driver is in the vehicle, driver might have left 
				// the vehicle seat and in that case we need to run vehicle sim on the server until it's in rest
				return;
			}
		}

		CreateDynamicPhysics( 288 );

		//autoptr PhysicsGeomDef geoms[] = {
		//	ExpansionPhysicsGeometry.CreateBox( "1.5 1 5.0" ).SetPosition( "0 0.8 0" )
		//}
		//dBodyDestroy( this );
		//dBodyCreateDynamicEx( this, "0 0.75 0", 10, geoms );
		
		dBodyDynamic( this, true );
		dBodyActive( this, ActiveState.ALWAYS_ACTIVE );

		dBodySetInteractionLayer( this, 288 );

		EnableDynamicCCD( true );
		SetDynamicPhysicsLifeTime( -1 );

		dBodyEnableGravity( this, true );

		m_BodyMass = dBodyGetMass( this );
		m_InvMass = 1.0 / m_BodyMass;
		m_BodyCenterOfMass = dBodyGetCenterOfMass( this );

		SetVelocity( this, m_LinearVelocity );
		dBodySetAngularVelocity( this, m_AngularVelocity );

		EnableCollisionsWithCharacter( false );

		SetAllowDamage( false );

		PlaceOnSurface();
	}

	// ------------------------------------------------------------
	override void EOnContact(IEntity other, Contact extra) //!EntityEvent.CONTACT
	{
		//! Print( "[" + this + "] EOnContact " + other );
	}

	// ------------------------------------------------------------
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::EOnPostSimulate - Start");
		#endif

		if ( HasKey() && GetGame().IsServer() ) 
		{
			if ( m_VehicleLockedState == ExpansionVehicleLockState.READY_TO_LOCK )
			{
				array< string > doors = new array< string >;
				ConfigGetTextArray( "doors", doors );

				bool doorsAreClosed = true;

				for ( int z = 0; z < doors.Count(); z++ )
				{
					if ( GetCarDoorsState( doors[z] ) != CarDoorState.DOORS_CLOSED )
					{
						//KeyMessage( doors[z] + " != DOORS_CLOSED" );
						doorsAreClosed = false;
						break;
					}
				}

				if ( doorsAreClosed )
				{
					m_VehicleLockedState = ExpansionVehicleLockState.LOCKED;

					//KeyMessage( "OnCarDoorClosed::LOCKED" );

					OnCarLocked();
					SetSynchDirty();
				}
			}
		}

		if ( !GetGame().IsClient() && m_NetworkMode == ExpansionVehicleNetworkMode.CLIENT )
		{
			return;
		}
	}

	// ------------------------------------------------------------
	void ExpansionDebugUI( string message = "" )
	{
		ExpansionDebugger.Display( EXPANSION_DEBUG_VEHICLE_CAR, message );
	}

	// ------------------------------------------------------------
	protected bool CanSimulate()
	{
		return true;
	}

	// ------------------------------------------------------------
	private void UpdateSpawning()
	{
		//m_IsSpawning = false;
	}

	// ------------------------------------------------------------
	protected void OnHumanPilot( PlayerBase driver, float pDt )
	{
	}

	// ------------------------------------------------------------
	protected void OnAIPilot( ExpansionAIBase driver, float pDt )
	{
	}

	// ------------------------------------------------------------
	protected void OnPreSimulation( float pDt )
	{
		if ( IsMissionClient() )
		{
			OnParticleUpdate( pDt );
		}

		OnAnimationUpdate( pDt );

		SetSynchDirty();
	}

	// ------------------------------------------------------------
	protected void OnSimulation( float pDt, out vector force, out vector torque )
	{
	}

	// ------------------------------------------------------------
	protected void OnPostSimulation( float pDt )
	{
	}

	// ------------------------------------------------------------
	protected void OnParticleUpdate( float pDt )
	{
	}

	// ------------------------------------------------------------
	protected void OnAnimationUpdate( float pDt )
	{
	}

	// ------------------------------------------------------------
	override void EOnSimulate( IEntity owner, float dt ) 
	{
		//! Print( "[" + this + "] EOnSimulate" );

		if ( !CanSimulate() )
			return;

		ExpansionDebugUI( "[[ " + this + " ]]" );

		DayZPlayerImplement driver = DayZPlayerImplement.Cast( CrewMember( DayZPlayerConstants.VEHICLESEAT_DRIVER ) );

		if ( GetGame().IsClient() )
		{
			m_IsPhysicsHost = driver == GetGame().GetPlayer();

			if ( m_IsPhysicsHost )
			{
				m_IsPhysicsHost = m_NetworkMode != ExpansionVehicleNetworkMode.SERVER_ONLY;

				if ( !m_IsPhysicsHost )
				{
					m_Controller.Update();
				}
			}
		} else if ( GetGame().IsServer() )
		{
			m_IsPhysicsHost = m_NetworkMode != ExpansionVehicleNetworkMode.CLIENT;
		} else if ( !GetGame().IsMultiplayer() )
		{
			m_IsPhysicsHost = true;
		} else
		{
			m_IsPhysicsHost = false;
		}

		m_HasDriver = false;
		if ( driver && m_IsPhysicsHost )
		{
			ExpansionAIBase aiDriver;
			PlayerBase humanDriver;

			if ( Class.CastTo( aiDriver, driver ) )
			{
				m_HasDriver = true;
				
				OnAIPilot( aiDriver, dt );
			} else if ( Class.CastTo( humanDriver, driver ) )
			{
				m_HasDriver = true;

				m_Controller.Update();

				OnHumanPilot( humanDriver, dt );
			}
		}

		OnPreSimulation( dt );

		if ( !GetGame().IsClient() && m_NetworkMode == ExpansionVehicleNetworkMode.CLIENT )
		{
			int clientTimePredict = GetGame().GetTime() - m_SyncState.m_TimeDelta;
			int clientTimePredictDelta = clientTimePredict - m_SyncState.m_Time;
			float nDelta = clientTimePredictDelta / 1000.0;
			
			ExpansionPhysics.IntegrateTransform( m_SyncState.m_InitialTransform, m_SyncState.m_LinearVelocity, m_SyncState.m_AngularVelocity, nDelta, m_SyncState.m_PredictedTransform );

			// smooth movement for non-driven clients
			MoveInTime( m_SyncState.m_PredictedTransform.data, dt );

			SetSynchDirty();

			ExpansionDebugger.Push( EXPANSION_DEBUG_VEHICLE_CAR );
			return;
		}

		if ( m_IsPhysicsHost )
		{
			dBodyEnableGravity( this, true );
			dBodyDynamic( this, true );
			dBodyActive( this, ActiveState.ALWAYS_ACTIVE );

			m_Transform.Get( this );

			m_LinearVelocity = GetVelocity( this );
			m_AngularVelocity = dBodyGetAngularVelocity( this );

			m_LinearVelocityMS = m_LinearVelocity.InvMultiply3( m_Transform.GetBasis().data );
			m_AngularVelocityMS = m_AngularVelocity.InvMultiply3( m_Transform.GetBasis().data );

			ExpansionDebugUI( "Mass: " + m_BodyMass + " (Center of Mass: " + m_BodyCenterOfMass + ")");

			ExpansionDebugUI( "Linear Velocity (WS): " + m_LinearVelocity );
			ExpansionDebugUI( "Linear Velocity (MS): " + m_LinearVelocityMS );

			m_InvInertiaTensor = dBodyGetLocalInertia( this );
			dBodyGetInvInertiaTensorWorld( this, m_InvInertiaTensorWS.data );
			
			int i = 0;
			int numWheelsGrounded = 0;
			for ( i = 0; i < m_Axles.Count(); i++ )
			{
				m_Axles[i].SetupSimulation( dt, numWheelsGrounded );
			}
		
			vector impulse = vector.Zero;
			vector impulseTorque = vector.Zero;
			vector force = vector.Zero;
			vector torque = vector.Zero;

			//vector gravity = "0 -9.8 0" * m_BodyMass * dt;
			//impulse += gravity;
			//impulseTorque += m_BodyCenterOfMass * gravity;

			OnSimulation( dt, force, torque );

			impulse += force * dt;
			impulseTorque += torque * dt;

			for ( i = 0; i < m_Axles.Count(); i++ )
			{
				m_Axles[i].Simulate( dt, numWheelsGrounded, impulse, impulseTorque );
			}

			dBodyApplyImpulse( this, impulse );
			dBodyApplyTorqueImpulse( this, impulseTorque );

			//ApplyPhysics( dt, impulse, impulseTorque );

			if ( GetGame().IsMultiplayer() )
			{
				SetSynchDirty();
			}

			if ( IsMissionClient() )
			{
				NetworkSend();
			}
		}

		OnPostSimulation( dt );
		
		ExpansionDebugger.Push( EXPANSION_DEBUG_VEHICLE_WHEELS );
		ExpansionDebugger.Push( EXPANSION_DEBUG_VEHICLE_CAR );
	}

	// ------------------------------------------------------------
	private void HandleSync_Server()
	{
		switch ( m_NetworkMode )
		{
		case ExpansionVehicleNetworkMode.SERVER_ONLY:
			m_SyncState.m_LinearVelocity = m_LinearVelocity;
			m_SyncState.m_AngularVelocity = m_AngularVelocity;

			m_SyncState.SynchronizeVariables( m_NetworkMode );
			return;
		case ExpansionVehicleNetworkMode.PREDICTION:
			return;
		case ExpansionVehicleNetworkMode.CLIENT:
			return;
		}
	}

	// ------------------------------------------------------------
	private void HandleSync_Client()
	{
		switch ( m_NetworkMode )
		{
		case ExpansionVehicleNetworkMode.SERVER_ONLY:
			return;
		case ExpansionVehicleNetworkMode.PREDICTION:
			return;
		case ExpansionVehicleNetworkMode.CLIENT:
			m_SyncState.m_LinearVelocity = m_LinearVelocity;
			m_SyncState.m_AngularVelocity = m_AngularVelocity;
			m_SyncState.m_Position = GetPosition();
			m_SyncState.m_Orientation = GetOrientation();

			m_SyncState.SynchronizeVariables( m_NetworkMode );
			return;
		}
	}

	// ------------------------------------------------------------
	void ApplyPhysics( float pDt, vector impulse, vector impulseTorque )
	{
		dBodyApplyImpulse( this, impulse );
		dBodyApplyTorqueImpulse( this, impulseTorque );

		m_LinearVelocity = GetVelocity( this );
		m_AngularVelocity = dBodyGetAngularVelocity( this );

		if ( !GetGame().IsClient() )
		{
			HandleSync_Server();
		} else if ( !GetGame().IsServer() )
		{
			HandleSync_Client();
		}

		//m_LinearVelocity = vector.Zero;
		//m_AngularVelocity = vector.Zero;
		//dBodySetInteractionLayer( this, 0 );

		SetVelocity( this, m_LinearVelocity );
		dBodySetAngularVelocity( this, m_AngularVelocity );

		dBodyActive( this, ActiveState.ALWAYS_ACTIVE );

		if ( GetGame().IsClient() )
		{
			/*
			ExpansionDebugUI();

			ExpansionDebugUI( "Server Position: " + m_SyncState.m_Position );
			ExpansionDebugUI( "Server Orientation: " + m_SyncState.m_Orientation );

			ExpansionDebugUI( "Server Linear Velocity: " + m_SyncState.m_LinearVelocity );
			ExpansionDebugUI( "Server Angular Velocity: " + m_SyncState.m_AngularVelocity );
			*/
		}

		if ( IsMissionClient() )
		{
			/*
			ExpansionDebugUI();

			ExpansionDebugUI( "Client Position: " + GetPosition() );
			ExpansionDebugUI( "Client Orientation: " + GetOrientation() );

			ExpansionDebugUI( "Client Linear Velocity: " + m_LinearVelocity );
			ExpansionDebugUI( "Client Angular Velocity: " + m_AngularVelocity );

			ExpansionDebugUI();
			*/
		}

		for ( int i = 0; i < m_Wheels.Count(); i++ )
		{
			m_Wheels[i].DebugUpdate( pDt );
		}
	}

	// ------------------------------------------------------------
	bool OnNetworkTransformUpdate( out vector pos, out vector ypr )
	{
		if ( m_NetworkMode == ExpansionVehicleNetworkMode.CLIENT )
			return true;

		m_SyncState.m_Position = pos;
		m_SyncState.m_Orientation[0] = ypr[0] * Math.RAD2DEG;
		m_SyncState.m_Orientation[1] = ypr[1] * Math.RAD2DEG;
		m_SyncState.m_Orientation[2] = ypr[2] * Math.RAD2DEG;

		if ( m_IsPhysicsHost )
		{
			pos = GetPosition();
			ypr = GetOrientation();
			ypr[0] = ypr[0] * Math.DEG2RAD;
			ypr[1] = ypr[1] * Math.DEG2RAD;
			ypr[2] = ypr[2] * Math.DEG2RAD;

			return true;
		}
		
		return false;
	}

	// ------------------------------------------------------------
	override void OnRPC( PlayerIdentity sender, int rpc_type, ParamsReadContext ctx )
	{
		super.OnRPC( sender, rpc_type, ctx );

		switch( rpc_type )
		{
			case ExpansionVehicleRPC.ControllerSync:
			{
				if ( m_Controller )
				{
					if ( CrewMember( DayZPlayerConstants.VEHICLESEAT_DRIVER ) == PlayerBase.GetPlayerByUID( sender.GetId() ) )
					{
						if ( m_Controller.NetworkRecieve( ctx ) )
						{
							OnNetworkRecieve( ctx );
						}
					}
				}

				return;
			}
			case ERPCs.RPC_EXPLODE_EVENT:
			{
				if ( GetGame().IsClient() )
				{
					Param2< int, string > params;

					if ( ctx.Read( params ) )
					{
						ExpansionOnExplodeClient( params.param1, params.param2 );
					}
				}
				
				return;
			}
			case ExpansionVehicleRPC.ClientSync:
			{
				if ( GetGame().IsClient() || m_NetworkMode != ExpansionVehicleNetworkMode.CLIENT )
					return;

				int time;
				ctx.Read( time );

				// check if this is an old state and if so, remove it
				if ( m_SyncState.m_Time > time )
					return;

				m_SyncState.m_LastRecievedTime = m_SyncState.m_Time;
				m_SyncState.m_Time = time;

				// time delta should always be positive, this is not the latency
				m_SyncState.m_TimeDelta = GetGame().GetTime() - time;

				ctx.Read( m_SyncState.m_Position );
				ctx.Read( m_SyncState.m_Orientation );
				ctx.Read( m_SyncState.m_LinearVelocity );
				ctx.Read( m_SyncState.m_AngularVelocity );

				return;
			}
		}
	}

	// ------------------------------------------------------------
	void NetworkSend()
	{
		if ( IsMissionOffline() )
			return;

		ScriptRPC rpc = new ScriptRPC();

		m_Controller.NetworkSend( rpc );
		OnNetworkSend( rpc );

		if ( IsMissionHost() )
		{
			Human human = CrewMember( DayZPlayerConstants.VEHICLESEAT_DRIVER );
			if ( human != NULL )
				rpc.Send( NULL, ExpansionVehicleRPC.ControllerSync, true, human.GetIdentity() );
		} else
		{
			rpc.Send( NULL, ExpansionVehicleRPC.ControllerSync, true, NULL );
		}
	}

	// ------------------------------------------------------------
	protected void OnNetworkSend( ref ParamsWriteContext ctx )
	{
	}

	// ------------------------------------------------------------
	protected void OnNetworkRecieve( ref ParamsReadContext ctx )
	{
	}

	// ------------------------------------------------------------
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		m_SyncState.OnVariablesSynchronized();
	}

	// ------------------------------------------------------------
	void SetWheelEngineForce( int wheel, float force )
	{
		m_Wheels[wheel].m_EngineForce = force;
	}
	
	// ------------------------------------------------------------
	float GetWheelEngineForce( int wheel )
	{
		return m_Wheels[wheel].m_EngineForce;
	}

	// ------------------------------------------------------------
	void SetWheelBrakeForce( int wheel, float force )
	{
		m_Wheels[wheel].m_BrakeForce = force;
	}
	
	// ------------------------------------------------------------
	float GetWheelBrakeForce( int wheel )
	{
		return m_Wheels[wheel].m_BrakeForce;
	}

	// ------------------------------------------------------------
	void SetWheelSteering( int wheel, float steering )
	{
		m_Wheels[wheel].m_Steering = steering;
	}

	// ------------------------------------------------------------
	float GetNumWheels()
	{
		return m_Wheels.Count();
	}

	// ------------------------------------------------------------
	vector GetModelVelocityAt( vector relPos )
	{
		return m_LinearVelocityMS + ( m_AngularVelocityMS * relPos );
	}

	// ------------------------------------------------------------
	vector GetWorldVelocityAt( vector relPos )
	{
		return GetModelVelocityAt( relPos ).Multiply3( m_Transform.GetBasis().data );
	}

	/**
	 * Returns an extrapolated orientation based on the current velocity
	 * 
	 * @param pDt delta to extrapolate by
	 * @return	current orientation extrapolated based on the time from the angular velocity
	 * 			if pDt = 0, no extrapolation, if pDt = 1/40 then orientation next frame
	 */
	Matrix3 GetEstimatedOrientation( float pDt )
	{
		Matrix3 estimatedOrientation;
		Matrix3.Tilda( m_AngularVelocity, estimatedOrientation );

		Math3D.MatrixInvMultiply3( estimatedOrientation.data, m_Transform.GetBasis().data, estimatedOrientation.data );
		
		estimatedOrientation.data[0] = m_Transform.data[0] + ( estimatedOrientation.data[0] * pDt );
		estimatedOrientation.data[1] = m_Transform.data[1] + ( estimatedOrientation.data[1] * pDt );
		estimatedOrientation.data[2] = m_Transform.data[2] + ( estimatedOrientation.data[2] * pDt );

		estimatedOrientation.data[0] = estimatedOrientation.data[0].Normalized();
		estimatedOrientation.data[1] = estimatedOrientation.data[1].Normalized();
		estimatedOrientation.data[2] = estimatedOrientation.data[2].Normalized();

		return estimatedOrientation;
	}

	// ------------------------------------------------------------
	ref ExpansionVehicleAxle GetAxle( int axle )
	{
		return m_Axles[axle];
	}

	// ------------------------------------------------------------
	override void EEItemAttached( EntityAI item, string slot_name ) 
	{
		ExpansionWheel wheel;
		if ( !Class.CastTo( wheel, item ) )
			return;

		string slot = slot_name;
		slot.ToLower();

		for ( int i = 0; i < m_Wheels.Count(); i++ )
		{
			string inv_slot = m_Wheels[i].GetInventorySlot();
			inv_slot.ToLower();

			if ( inv_slot == slot )
			{
				m_Wheels[i].OnWheelAttach( wheel );
			}
		}
	}

	// ------------------------------------------------------------
	override void EEItemDetached( EntityAI item, string slot_name )
	{
		ExpansionWheel wheel;
		if ( !Class.CastTo( wheel, item ) )
			return;

		string slot = slot_name;
		slot.ToLower();

		for ( int i = 0; i < m_Wheels.Count(); i++ )
		{
			string inv_slot = m_Wheels[i].GetInventorySlot();
			inv_slot.ToLower();

			if ( inv_slot == slot )
			{
				m_Wheels[i].OnWheelDetach();
			}
		}
	}

	// ------------------------------------------------------------
	ref ExpansionController GetExpansionController()
	{
		return m_Controller;
	}

	// ------------------------------------------------------------
	void SetAttachmentHealth( string itemName, float health )
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::SetAttachmentHealth - Start");
		#endif
		EntityAI item = FindAttachmentBySlotName( itemName );

		if ( item )
			item.SetHealth( "", "", health );

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::SetAttachmentHealth - End");
		#endif
	}

	void CreateLights( Object lod, string point, typename type, vector color, vector ambient, float radius, float brigthness, bool flare, bool shadows, float default = 0 )
	{
		ref array<Selection> lodSelections = new array<Selection>();

		LOD lodLod = lod.GetLODByName( "memory" );
		if ( lodLod )
		{
			if ( lodLod.GetSelections( lodSelections ) )
			{
				for ( int i = 0; i < lodSelections.Count(); i++ )
				{	 
					if ( lodSelections[i].GetName() == point )
					{
						for( int j = 0; j < lodSelections[i].GetVertexCount(); j++ )
						{
							ExpansionPointLight light = ExpansionPointLight.Cast( ExpansionPointLight.CreateLight( type, "0 0 0" ) );
							light.m_Val = default;
 
							light.SetRadiusTo( radius );
							light.SetBrightnessTo( brigthness );

							light.SetDiffuseColor( color[0], color[1], color[2] );
							light.SetAmbientColor( ambient[0], ambient[1], ambient[2] );
											
							light.SetFlareVisible( flare );
							light.SetCastShadow( shadows );

							light.AttachOnObject( lod, lodSelections[i].GetVertexPosition(lodLod, j), "0 0 0" );

							light.ExpansionSetEnabled( true );

							m_Lights.Insert( light );
						}
					}
				}
			}
		}
	}

	void CreateParticle( Object lod, string point, int type )
	{
		ref array<Selection> lodSelections = new array<Selection>();

		LOD lodLod = lod.GetLODByName( "memory" );
		if ( lodLod )
		{
			if ( lodLod.GetSelections( lodSelections ) )
			{
				for ( int i = 0; i < lodSelections.Count(); i++ )
				{	 
					if ( lodSelections[i].GetName() == point )
					{
						for( int j = 0; j < lodSelections[i].GetVertexCount(); j++ )
						{
							Particle particle = Particle.PlayOnObject( type, lod, lodSelections[i].GetVertexPosition(lodLod, j) );
							//! AddChild( particle.GetDirectParticleEffect(), -1, true );

							m_Particles.Insert( particle );
						}
					}
				}
			}
		}
	}

#ifndef EXPANSION_USING_TRANSPORT_BASE

////////////////////////////////////////////////////////////////////
// TRANSPORT CODE
////////////////////////////////////////////////////////////////////

	//! Returns crew capacity of this vehicle.
	int CrewSize()
	{
		return m_Crew.Count();
	}

	//! Returns crew member index based on action component index.
	//! -1 is returned when no crew position corresponds to given component index.
	int CrewPositionIndex( int componentIdx )
	{
		for ( int i = 0; i < m_Crew.Count(); i++ )
		{
			if ( m_Crew[i].GetComponentIndex() == componentIdx )
			{
				return i;
			}
		}

		return -1;
	}

	int CrewPositionIndexSelection( int componentIdx )
	{
		array<string> selections = new array<string>();
		GetActionComponentNameList( componentIdx, selections );
		
		for ( int i = 0; i < selections.Count(); i++ )
		{
			for ( int j = 0; j < m_Crew.Count(); j++ )
			{
				if ( m_Crew[j].GetComponentSelection() == selections[i] )
				{
					return j;
				}
			}
		}
		return -1;
	}

	//! Returns crew member index based on player's instance.
	//! -1 is returned when the player is not isnide.
	int CrewMemberIndex( Human player )
	{
		if ( player == NULL )
			return -1;

		for ( int i = 0; i < m_Crew.Count(); i++ )
		{
			if ( m_Crew[i].GetPlayer() == player )
			{
				return i;
			}
		}

		return -1;
	}

	//! Returns crew member based on position index.
	//! Null can be returned if no Human is present on the given position.
	Human CrewMember( int posIdx )
	{
		if ( posIdx >= m_Crew.Count() )
			return NULL;

		if ( posIdx < 0 )
			return NULL;
		
		return m_Crew[posIdx].GetPlayer();
	}

	//! Performs transfer of player from world into vehicle on given position.
	void CrewGetIn( Human player, int posIdx )
	{
		if ( posIdx >= m_Crew.Count() )
			return;

		if ( posIdx < 0 )
			return;

		m_Crew[posIdx].SetPlayer( DayZPlayerImplement.Cast( player ) );
	}

	//! Performs transfer of player from vehicle into world from given position.
	Human CrewGetOut( int posIdx )
	{
		if ( posIdx >= m_Crew.Count() )
			return NULL;

		if ( posIdx < 0 )
			return NULL;

		Human human = m_Crew[posIdx].GetPlayer();

		m_Crew[posIdx].SetPlayer( NULL );

		if ( human == NULL )
			return NULL;

		return human;
	}
	
	void CrewEntry( int posIdx, out vector pos, out vector dir )
	{
		if ( posIdx >= m_Crew.Count() )
			return;

		if ( posIdx < 0 )
			return;

		m_Crew[posIdx].GetIn( pos, dir );
	}

	void CrewEntryWS( int posIdx, out vector pos, out vector dir )
	{
		if ( posIdx >= m_Crew.Count() )
			return;

		if ( posIdx < 0 )
			return;

		m_Crew[posIdx].GetIn( pos, dir );

		pos = ModelToWorld( pos );
		dir = vector.Direction( GetPosition(), ModelToWorld( dir ) );
	}
	
	void CrewTransform( int posIdx, out vector mat[4] )
	{
		if ( posIdx >= m_Crew.Count() )
			return;

		if ( posIdx < 0 )
			return;

		m_Crew[posIdx].GetTransform( mat );
	}

	void CrewTransformWS( int posIdx, out vector mat[4] )
	{
		if ( posIdx >= m_Crew.Count() )
			return;

		if ( posIdx < 0 )
			return;

		m_Crew[posIdx].GetTransform( mat );

		mat[3] = ModelToWorld( mat[3] );
	}

	int GetAnimInstance()
	{
		return ExpansionVehicleAnimInstances.EX_HATCHBACK;
	}
	
	int GetSeatAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
		}

		return 0;
	}

	int Get3rdPersonCameraType()
	{
		return DayZPlayerCameras.DAYZCAMERA_3RD_VEHICLE;
	}

	// ------------------------------------------------------------
	bool CrewCanGetThrough( int posIdx )
	{
		return true;
	}

	// ------------------------------------------------------------
	bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
		return true;	
	}

	// ------------------------------------------------------------
	bool CanReachSeatFromDoors( string pSeatSelection, vector pFromPos, float pDistance = 1.0 )
	{
		return true;		
	}

	// ------------------------------------------------------------
	bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
		return true;		
	}
#else
	static ref map<typename, ref TInputActionMap> m_AdvComTypeActionsMap = new map<typename, ref TInputActionMap>;
	TInputActionMap m_InputActionMap;
	bool m_ActionsInitialize;

	void InitializeActions()
	{
		m_InputActionMap = m_AdvComTypeActionsMap.Get( this.Type() );
		if(!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap;
			m_InputActionMap = iam;
			SetActions();
			m_AdvComTypeActionsMap.Insert(this.Type(), m_InputActionMap);
		}
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if(!m_ActionsInitialize)
		{
			m_ActionsInitialize = true;
			InitializeActions();
		}
		
		actions = m_InputActionMap.Get(action_input_type);
	}

	void AddAction(typename actionName)
	{
		ActionBase action = ActionManagerBase.GetAction(actionName);

		if(!action)
		{
			Debug.LogError("Action " + actionName + " dosn't exist!");
			return;
		}		
		
		typename ai = action.GetInputType();
		if(!ai)
		{
			m_ActionsInitialize = false;
			return;
		}
		ref array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
		if(!action_array)
		{
			action_array = new array<ActionBase_Basic>;
			m_InputActionMap.Insert(ai, action_array);
		}
		
		#ifdef EXPANSIONEXLOGPRINT
		EXLogPrint("+ " + this + " add action: " + action + " input " + ai);
		#endif

		action_array.Insert(action);
	}
	
	void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		ActionBase action = player.GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		ref array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
		if(action_array)
		{
			action_array.RemoveItem(action);
		}
	}
#endif

	void SetActions()
	{
		AddAction( ExpansionActionGetInExpansionVehicle );
		AddAction( ExpansionActionOpenVehicleDoor );
		AddAction( ExpansionActionCloseVehicleDoor );
	}

////////////////////////////////////////////////////////////////////
// CAR/CARSCRIPT CODE
////////////////////////////////////////////////////////////////////

	ref CarController m_CarScController;

	//!	Returns the instance of vehicle's controller.
	CarController GetController()
	{
		if (m_CarScController == NULL)
			m_CarScController = new CarController;
		return m_CarScController;
	}

	//!	Returns the current speed of the vehicle in km/h.
	float GetSpeedometer()
	{
		return GetVelocity( this ).Length();
	}

	/*!
		Returns tank capacity for the specified vehicle's fluid.

		\param fluid the specified fluid type
	*/
	float GetFluidCapacity( CarFluid fluid )
	{
		return 0;
	}

	/*!
		Returns fraction value (in range <0, 1>)
		of the current state of the specified vehicle's fluid.

		\param[in] fluid the specified fluid type
	*/
	float GetFluidFraction( CarFluid fluid )
	{
		return 0;
	}

	//! Removes from the specified fluid the specified amount.
	void Leak( CarFluid fluid, float amount )
	{
	}

	//! Removes all the specified fluid from vehicle.
	void LeakAll( CarFluid fluid )
	{
	}

	//! Adds to the specified fluid the specified amount.
	void Fill( CarFluid fluid, float amount )
	{
	}

	/*!
		Is called every time when the specified vehicle's fluid level
		changes eg. when car is consuming fuel.

		\param[in] fluid fluid identifier, \see CarFluid
		\param[in] newValue new fluid level
		\param[in] oldValue previous fluid level before change
	*/
	void OnFluidChanged( CarFluid fluid, float newValue, float oldValue )
	{
	}

	//! Returns engine's max rpm before engine blows up.
	float EngineGetRPMMax()
	{
		return 0;
	}

	//! Returns engine's maximal working rpm without damaging the engine.
	float EngineGetRPMRedline()
	{
		return 0;
	}

	//! Returns engine's rpm value.
	float EngineGetRPM()
	{
		return 0;
	}

	//! Returns true when engine is running, false otherwise.
	bool EngineIsOn()
	{
		return m_EngineIsOn;
	}

	//! Starts the engine.
	void EngineStart()
	{
		if ( OnBeforeEngineStart() )
		{
			m_EngineIsOn = true;

			OnEngineStart();
		}
	}

	/*!
		Is called every time the game wants to start the engine.

		\return true if the engine can start, false otherwise.
	*/
	protected bool OnBeforeEngineStart()
	{
		// engine can start by default
		return true;
	}

	//! Is called every time the engine starts.
	protected void OnEngineStart()
	{
	}

	//! Stops the engine.
	void EngineStop()
	{
		m_EngineIsOn = false;

		OnEngineStop();
	}

	//! Is called every time the engine stops.
	protected void OnEngineStop()
	{
	}

	//! Returns total number of gears.
	int GetGearsCount()
	{
		return 0;
	}

	/*!
		Is called every time when the simulation changed gear.

		\param[in] newGear new gear level
		\param[in] oldGear previous gear level before gear shift
	*/
	void OnGearChanged( int newGear, int oldGear )
	{
	}

	/*!
		Is called every time when vehicle collides with other object.

		\param[in] zoneName configured vehicle's zone that was hit
		\param[in] localPos position where the vehicle was hit in vehicle's space
		\param[in] other object with which the vehicle is colliding
		\param[in] data contact properties
	*/
	void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
	{
	}

	/*!
		Is called every sound simulation step.
		In this callback, user can modify behaviour of sound controllers.

		\param[in] ctrl sound controller identifier, \see CarSoundCtrl
		\param[in] oldValue already computed value by the game code
		\return new value of the specified sound controller.
	*/
	float OnSound( CarSoundCtrl ctrl, float oldValue )
	{
		// just use the computed value by the game code
		return oldValue;
	}

////////////////////////////////////////////////////////////////////
// EXPANSION CODE
////////////////////////////////////////////////////////////////////

	// ------------------------------------------------------------
	ExpansionVehicleLockState GetLockedState()
	{
		return m_VehicleLockedState;
	}

	// ------------------------------------------------------------
	void KeyMessage( string message )
	{
		Message( GetPlayer(), message );
	}

	// ------------------------------------------------------------
	bool IsLocked()
	{
		if ( HasKey() )
		{
			return m_VehicleLockedState == ExpansionVehicleLockState.LOCKED;
		}

		return false;
	}

	// ------------------------------------------------------------
	bool HasKey()
	{
		return m_VehicleLockedState != ExpansionVehicleLockState.NOLOCK;
	}

	// ------------------------------------------------------------
	void PairKeyTo( ExpansionCarKey key )
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::PairKeyTo - Start");
		#endif

		if ( key )
		{
			key.PairToVehicle( this );

			m_VehicleLockedState = ExpansionVehicleLockState.UNLOCKED;

			SetSynchDirty();
		}

		//KeyMessage( "PairKeyTo (" + this + ", " + key + ")" );

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::PairKeyTo - End");
		#endif
	}

	// ------------------------------------------------------------
	bool CanBeLocked()
	{
		return true;
	}

	// ------------------------------------------------------------
	void OnCarDoorOpened( string source )
	{
		if ( HasKey() ) 
		{
			if ( m_VehicleLockedState == ExpansionVehicleLockState.READY_TO_LOCK )
			{
				m_VehicleLockedState = ExpansionVehicleLockState.UNLOCKED;
				//KeyMessage( "OnCarDoorOpened::UNLOCKED" );
				SetSynchDirty();
			}
		}
	}

	// ------------------------------------------------------------
	void OnCarDoorClosed( string source )
	{

	}

	// ------------------------------------------------------------
	bool IsCarKeys( ExpansionCarKey key )
	{
		if ( !HasKey() )
		{
			//KeyMessage( "IsCarKeys::HasKey" );
			return false;
		}

		if ( !key.IsPairedTo( this ) )
		{
			//KeyMessage( "IsCarKeys not paired!" );
			return false;
		}

		return true;
	}

	// ------------------------------------------------------------
	void LockCar( ExpansionCarKey key = NULL )
	{
		if ( key && !IsCarKeys( key ) )
			return;

		m_VehicleLockedState = ExpansionVehicleLockState.READY_TO_LOCK;
		//KeyMessage( "LockCar::READY_TO_LOCK" );

		SetSynchDirty();
	}

	// ------------------------------------------------------------
	void UnlockCar( ExpansionCarKey key = NULL )
	{
		if ( key && !IsCarKeys( key ) )
			return;

		m_VehicleLockedState = ExpansionVehicleLockState.UNLOCKED;
		//KeyMessage( "UnlockCar::UNLOCKED" );

		OnCarUnlocked();
		SetSynchDirty();
	}

	// ------------------------------------------------------------
	void OnCarLocked()
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnCarLocked - Start");
		#endif

		//KeyMessage( "OnCarLocked" );

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnCarLocked - Stop");
		#endif
	}

	// ------------------------------------------------------------
	void OnCarUnlocked()
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnCarUnlocked - Start");
		#endif

		//KeyMessage( "OnCarUnlocked" );

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnCarUnlocked - End");
		#endif
	}
	
	// ------------------------------------------------------------
	void OnHonkSoundPlay()
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnHonkSoundPlay - Start");
		#endif

		m_HornPlaying = true;
		
		Object battery;
		if (IsVitalCarBattery()) battery = this.FindAttachmentBySlotName("CarBattery");
		if (IsVitalTruckBattery()) battery = this.FindAttachmentBySlotName("TruckBattery");
		
		if (battery)
		{
			if (GetGame().GetPlayer().IsCameraInsideVehicle())
				m_HornSound = SEffectManager.PlaySoundOnObject( m_HornSoundSetINT, this );
			else
				m_HornSound = SEffectManager.PlaySoundOnObject( m_HornSoundSetEXT, this );
		}
		
		m_HornSound.SetSoundAutodestroy( true );

		m_HornSound.SetSoundLoop( true );

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnHonkSoundPlay - End");
		#endif
	}

	// ------------------------------------------------------------
	void OnHonkSoundStop()
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnHonkSoundStop - Start");
		#endif

		m_HornPlaying = false;

		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::OnHonkSoundStop - End");
		#endif
	}
	
	// ------------------------------------------------------------
	void PlayHonk()
	{	
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::PlayHonk - Start");
		#endif

		m_HornSynchRemote = true;
		m_HornPlaying = false;

		SetSynchDirty();

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::PlayHonk - End");
		#endif
	}
	
	// ------------------------------------------------------------
	void StopHonk()
	{	
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::StopHonk - Start");
		#endif

		m_HornSynchRemote = false;
		m_HornPlaying = true;

		SetSynchDirty();

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::StopHonk - End");
		#endif
	}
	
	// ------------------------------------------------------------
	bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}

	// ------------------------------------------------------------
	override void Explode( int damageType, string ammoType = "" )
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::Explode - Start");
		#endif

		if ( GetGame().IsServer() && !m_Exploded )
		{
			if ( ammoType == "" )
				ammoType = this.ConfigGetString( "ammoType" );
			
			if ( ammoType == "" )
				ammoType = "Dummy_Heavy";

			ExpansionOnExplodeServer( damageType, ammoType );

			if ( GetGame().IsServer() && !GetGame().IsMultiplayer() )
			{
				ExpansionOnExplodeClient( damageType, ammoType );
			} else 
			{				
				GetGame().RPCSingleParam( this, ERPCs.RPC_EXPLODE_EVENT, new Param2< int, string >( damageType, ammoType ), true );
			}
		}

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::Explode - End");
		#endif
	}

	// ------------------------------------------------------------
	// Explodes the vehicle on the server, telling the client that they should explode by setting the sync boolean.
	// ------------------------------------------------------------	
	void ExpansionOnExplodeServer( int damageType, string ammoType )
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::ExpansionOnExplodeServer - Start");
		#endif

		m_Exploded = true;
		m_ExplodedSynchRemote = true;

		LeakAll( CarFluid.COOLANT );
		LeakAll( CarFluid.FUEL );
		LeakAll( CarFluid.OIL );
		LeakAll( CarFluid.BRAKE );

		SetHealth( "", "", 0.0 );

		SetAttachmentHealth( "ExpansionHelicopterBattery", 0.0 );
		SetAttachmentHealth( "ExpansionAircraftBattery", 0.0 );
		SetAttachmentHealth( "CarBattery", 0.0 );
		SetAttachmentHealth( "TruckBattery", 0.0 );

		SetAttachmentHealth( "GlowPlug", 0.0 );
		SetAttachmentHealth( "SparkPlug", 0.0 );
		
		SetAttachmentHealth( "Reflector_1_1", 0.0 );
		SetAttachmentHealth( "Reflector_2_1", 0.0 );

		SetAttachmentHealth( "EngineBelt", 0.0 );
		SetAttachmentHealth( "CarRadiator", 0.0 );

		for ( int i = 0; i < CrewSize(); i++ )
		{
			Human crew = CrewMember( i );
			if ( !crew )
				continue;

			crew.SetHealth( 0.0 );
		}

		vector force = GetVelocity( this ) * m_BodyMass;
		force[1] = m_BodyMass * 10 + force[1];
		dBodyApplyForce( this, force * 0.1 );

		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::ExpansionOnExplodeServer - End");
		#endif
	}

	// ------------------------------------------------------------
	// Explodes the vehicle on the client, setting that the client has already exploded
	// ------------------------------------------------------------	
	void ExpansionOnExplodeClient( int damageType, string ammoType )
	{
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::ExpansionOnExplodeClient - Start");
		#endif

		StopSounds();
		EffectSound sound =	SEffectManager.PlaySound( "Expansion_Explosive_Vehicle_SoundSet", GetPosition() );
		sound.SetSoundAutodestroy( true );
/*
		ExpansionPointLight explosionLight = ExpansionPointLight.Cast( ScriptedLightBase.CreateLight( ExpansionPointLight, Vector( 0, 0, 0 ) )  ); 

		explosionLight.SetDiffuseColor( 1, 0.25, 0 );
		explosionLight.SetPosition( GetPosition() );
		explosionLight.SetRadiusTo( 20 * m_ExplosionSize );
		explosionLight.SetBrightnessTo( 4.5 ); 
		explosionLight.SetLifetime( 0.1 );
		explosionLight.SetFadeOutTime( 0.1 );
		explosionLight.SetFlareVisible( false );

		vector localPos = GetMemoryPointPos( "velka osa" );
		Particle p = Particle.PlayInWorld( ParticleList.EXPANSION_EXPLOSION_HELICOPTER, ModelToWorld( localPos ) );
		p.SetOrientation( n );

		Particle fire = Particle.PlayOnObject( ParticleList.EXPANSION_FIRE_HELICOPTER, this, localPos );
		//fire.SetOrientation( n );
		//AddChild(fire, 0);
*/
		#ifdef EXPANSIONEXPRINT
		EXPrint("CarScript::ExpansionOnExplodeClient - End");
		#endif
	}

	// ------------------------------------------------------------
	// The entity was already exploded but the client never knew about it (i.e. outside of network bubble)
	// ------------------------------------------------------------	
	void ExpansionOnSpawnExploded()
	{
		m_Exploded = true;

//		m_SmokeParticle = Particle.PlayOnObject( ParticleList.SMOKING_HELI_WRECK, this, GetMemoryPointPos( "velka osa" ) );
//		m_SmokeParticle.SetOrientation( n );
	}

	// ------------------------------------------------------------
	// Does nothing, vanilla code shall not run
	// ------------------------------------------------------------	
	override void OnExplodeClient()
	{
	}

	// ------------------------------------------------------------
	// Does nothing, vanilla code shall not run
	// ------------------------------------------------------------	
	override void SynchExplosion()
	{
	}

	// ------------------------------------------------------------
	override bool CanPutAsAttachment( EntityAI parent )
	{
		return false;
	}

	// ------------------------------------------------------------
	override bool CanPutInCargo( EntityAI parent )
	{
		return false;
	}

	// ------------------------------------------------------------
	override bool CanRemoveFromCargo( EntityAI parent )
	{
		return false;
	}

	// ------------------------------------------------------------
	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}

	// ------------------------------------------------------------	
	void StopSounds()
	{
	}

	// ------------------------------------------------------------
	bool IsExploded()
	{
		return m_Exploded;
	}

	// ------------------------------------------------------------
	string GetAnimSourceFromSelection( string selection )
	{
		return "";
	}

	// ------------------------------------------------------------
	string GetDoorConditionPointFromSelection( string selection )
	{
		return "";
	}
	
	// ------------------------------------------------------------
	string GetDoorSelectionNameFromSeatPos(int posIdx)
	{
		return "";
	}

	// ------------------------------------------------------------
	string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		return "";
	}

	// ------------------------------------------------------------
	int GetCarDoorsState( string slotType )
	{
		return CarDoorState.DOORS_MISSING;
	}

	// ------------------------------------------------------------
	bool IsVitalAircraftBattery()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool IsVitalHelicopterBattery()
	{
		return true;
	}
	
	// ------------------------------------------------------------
	bool IsVitalCarBattery()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool IsVitalSparkPlug()
	{
		return true;
	}
	
	// ------------------------------------------------------------
	bool IsVitalRadiator()
	{
		return false;
	}
	
	// ------------------------------------------------------------
	bool IsVitalGlowPlug()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool IsVitalEngineBelt()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool IsVitalTruckBattery()
	{
		return false;
	}

	// ------------------------------------------------------------	
	void UpdateLights( int new_gear = -1 ) 
	{
	}
	
	// ------------------------------------------------------------
	CarLightBase CreateRearLight()
	{
		return NULL;
	}
	
	// ------------------------------------------------------------
	CarLightBase CreateFrontLight()
	{
		return NULL;
	}

	// ------------------------------------------------------------
	bool IsPlane()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool IsBoat()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool IsHelicopter()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool IsCar()
	{
		return false;
	}

	// ------------------------------------------------------------
	bool CanObjectAttach( Object obj )
	{
		return false;
	}
	
	// ------------------------------------------------------------
	float GetActionDistanceFuel()
	{
		return 2.5;
	}

	// ------------------------------------------------------------
	float GetCameraHeight()
	{
		return 1;
	}

	// ------------------------------------------------------------
	float GetCameraDistance()
	{
		return 20;
	}

	bool IsFreeLook()
	{
		return false;
	}

	bool IsTransport()
	{
		return true;
	}
	
	// ------------------------------------------------------------
	float GetWreckAltitude()
	{
		return 1;
	}

	// ------------------------------------------------------------
	vector GetWreckOffset()
	{
		return "0 0 1";
	}

	// ------------------------------------------------------------
	string GetActionCompNameFuel()
	{
		return "refill";
	}
}