#ifndef ISERVERGAMEENTS_H
#define ISERVERGAMEENTS_H

#include "CBaseEntity.h"
using edict_t = CBaseEntity;
using CCheckTransmitInfo = void;

#define INTERFACEVERSION_SERVERGAMEENTS			"ServerGameEnts001"
//-----------------------------------------------------------------------------
// Purpose: Interface to get at server entities
//-----------------------------------------------------------------------------
class __declspec(novtable) IServerGameEnts
{
	public:
	virtual					~IServerGameEnts()	{}

	// Only for debugging. Set the edict base so you can get an edict's index in the debugger while debugging the game .dll
	virtual void			SetDebugEdictBase(edict_t *base) = 0;

	// The engine wants to mark two entities as touching
	virtual void			MarkEntitiesAsTouching( edict_t *e1, edict_t *e2 ) = 0;

	// Frees the entity attached to this edict
	virtual void			FreeContainingEntity( edict_t * ) = 0;

	// This allows the engine to get at edicts in a CGameTrace.
	virtual edict_t*		BaseEntityToEdict( CBaseEntity *pEnt ) = 0;
	virtual CBaseEntity*	EdictToBaseEntity( edict_t *pEdict ) = 0;

	// This sets a bit in pInfo for each edict in the list that wants to be transmitted to the
	// client specified in pInfo.
	//
	// This is also where an entity can force other entities to be transmitted if it refers to them
	// with ehandles.
	virtual void			CheckTransmit( CCheckTransmitInfo *pInfo, const unsigned short *pEdictIndices, int nEdicts ) = 0;
};

IServerGameEnts* GetServerGameEnts();

#endif //ISERVERGAMEENTS_H
