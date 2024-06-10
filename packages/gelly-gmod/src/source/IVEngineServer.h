#ifndef IVENGINESERVER_H
#define IVENGINESERVER_H

#define INTERFACEVERSION_VENGINESERVER "VEngineServer021"

#include <cstdint>

#include "MathTypes.h"

using edict_t = void;
using INetChannelInfo = void;
using soundlevel_t = int;
using ServerClass = void;
using bf_write = void *;
using IRecipientFilter = void;
using byte = unsigned char;
using VPlane = void;
using KeyValues = void;
using CBitVec = void;
using CCheckTransmitInfo = void;
using IChangeInfoAccessor = void;
using CGamestatsData = void;
using CSteamID = void;
using ICollisionQuery = void;
using ISpatialPartition = void;
using IScratchPad3D = void;
using SendTable = void;
using PVSInfo_t = void;
using client_textmessage_t = void;
using IAchievementMgr = void;
using QueryCvarCookie_t = void;
using ICollideable = void;
using player_info_t = void;
using CSharedEdictChangeInfo = void;

struct bbox_t {
	Vector mins;
	Vector maxs;
};

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the game DLL
//-----------------------------------------------------------------------------
class __declspec(novtable) IVEngineServer {
public:
	// Tell engine to change level ( "changelevel s1\n" or "changelevel2 s1
	// s2\n" )
	virtual void ChangeLevel(const char *s1, const char *s2) = 0;

	// Ask engine whether the specified map is a valid map file (exists and has
	// valid version number).
	virtual int IsMapValid(const char *filename) = 0;

	// Is this a dedicated server?
	virtual bool IsDedicatedServer(void) = 0;

	// Is in Hammer editing mode?
	virtual int IsInEditMode(void) = 0;

	// Add to the server/client lookup/precache table, the specified string is
	// given a unique index NOTE: The indices for PrecacheModel are 1 based
	//  a 0 returned from those methods indicates the model or sound was not
	//  correctly precached
	// However, generic and decal are 0 based
	// If preload is specified, the file is loaded into the server/client's
	// cache memory before level startup, otherwise
	//  it'll only load when actually used (which can cause a disk i/o hitch if
	//  it occurs during play of a level).
	virtual int PrecacheModel(const char *s, bool preload = false) = 0;
	virtual int PrecacheSentenceFile(const char *s, bool preload = false) = 0;
	virtual int PrecacheDecal(const char *name, bool preload = false) = 0;
	virtual int PrecacheGeneric(const char *s, bool preload = false) = 0;

	// Check's if the name is precached, but doesn't actually precache the name
	// if not...
	virtual bool IsModelPrecached(char const *s) const = 0;
	virtual bool IsDecalPrecached(char const *s) const = 0;
	virtual bool IsGenericPrecached(char const *s) const = 0;

	// Note that sounds are precached using the IEngineSound interface

	// Special purpose PVS checking
	// Get the cluster # for the specified position
	virtual int GetClusterForOrigin(const Vector &org) = 0;
	// Get the PVS bits for a specified cluster and copy the bits into
	// outputpvs.  Returns the number of bytes needed to pack the PVS
	virtual int GetPVSForCluster(
		int cluster, int outputpvslength, unsigned char *outputpvs
	) = 0;
	// Check whether the specified origin is inside the specified PVS
	virtual bool CheckOriginInPVS(
		const Vector &org, const unsigned char *checkpvs, int checkpvssize
	) = 0;
	// Check whether the specified worldspace bounding box is inside the
	// specified PVS
	virtual bool CheckBoxInPVS(
		const Vector &mins,
		const Vector &maxs,
		const unsigned char *checkpvs,
		int checkpvssize
	) = 0;

	// Returns the server assigned userid for this player.  Useful for logging
	// frags, etc.
	//  returns -1 if the edict couldn't be found in the list of players.
	virtual int GetPlayerUserId(const edict_t *e) = 0;
	virtual const char *GetPlayerNetworkIDString(const edict_t *e) = 0;

	// Return the current number of used edict slots
	virtual int GetEntityCount(void) = 0;
	// Given an edict, returns the entity index
	virtual int IndexOfEdict(const edict_t *pEdict) = 0;
	// Given and entity index, returns the corresponding edict pointer
	virtual edict_t *PEntityOfEntIndex(int iEntIndex) = 0;

	// Get stats info interface for a client netchannel
	virtual INetChannelInfo *GetPlayerNetInfo(int playerIndex) = 0;

	// Allocate space for string and return index/offset of string in global
	// string list If iForceEdictIndex is not -1, then it will return the edict
	// with that index. If that edict index is already used, it'll return null.
	virtual edict_t *CreateEdict(int iForceEdictIndex = -1) = 0;
	// Remove the specified edict and place back into the free edict list
	virtual void RemoveEdict(edict_t *e) = 0;

	// Memory allocation for entity class data
	virtual void *PvAllocEntPrivateData(long cb) = 0;
	virtual void FreeEntPrivateData(void *pEntity) = 0;

	// Save/restore uses a special memory allocator (which zeroes newly
	// allocated memory, etc.)
	virtual void *SaveAllocMemory(size_t num, size_t size) = 0;
	virtual void SaveFreeMemory(void *pSaveMem) = 0;

	// Emit an ambient sound associated with the specified entity
	virtual void EmitAmbientSound(
		int entindex,
		const Vector &pos,
		const char *samp,
		float vol,
		soundlevel_t soundlevel,
		int fFlags,
		int pitch,
		float delay = 0.0f
	) = 0;

	// Fade out the client's volume level toward silence (or fadePercent)
	virtual void FadeClientVolume(
		const edict_t *pEdict,
		float fadePercent,
		float fadeOutSeconds,
		float holdTime,
		float fadeInSeconds
	) = 0;

	// Sentences / sentence groups
	virtual int SentenceGroupPick(
		int groupIndex, char *name, int nameBufLen
	) = 0;
	virtual int SentenceGroupPickSequential(
		int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset
	) = 0;
	virtual int SentenceIndexFromName(const char *pSentenceName) = 0;
	virtual const char *SentenceNameFromIndex(int sentenceIndex) = 0;
	virtual int SentenceGroupIndexFromName(const char *pGroupName) = 0;
	virtual const char *SentenceGroupNameFromIndex(int groupIndex) = 0;
	virtual float SentenceLength(int sentenceIndex) = 0;

	// Issue a command to the command parser as if it was typed at the server
	// console.
	virtual void ServerCommand(const char *str) = 0;
	// Execute any commands currently in the command parser immediately (instead
	// of once per frame)
	virtual void ServerExecute(void) = 0;
	// Issue the specified command to the specified client (mimics that client
	// typing the command at the console).
	virtual void ClientCommand(edict_t *pEdict, const char *szFmt, ...) = 0;

	// Set the lightstyle to the specified value and network the change to any
	// connected clients.  Note that val must not
	//  change place in memory (use MAKE_STRING) for anything that's not
	//  compiled into your mod.
	virtual void LightStyle(int style, const char *val) = 0;

	// Project a static decal onto the specified entity / model (for level
	// placed decals in the .bsp)
	virtual void StaticDecal(
		const Vector &originInEntitySpace,
		int decalIndex,
		int entityIndex,
		int modelIndex,
		bool lowpriority
	) = 0;

	// Given the current PVS(or PAS) and origin, determine which players should
	// hear/receive the message
	virtual void Message_DetermineMulticastRecipients(
		bool usepas, const Vector &origin, void *playerbits
	) = 0;

	// Begin a message from a server side entity to its client side counterpart
	// (func_breakable glass, e.g.)
	virtual bf_write *EntityMessageBegin(
		int ent_index, ServerClass *ent_class, bool reliable
	) = 0;
	// Begin a usermessage from the server to the client .dll
	virtual bf_write *UserMessageBegin(
		IRecipientFilter *filter, int msg_type
	) = 0;
	// Finish the Entity or UserMessage and dispatch to network layer
	virtual void MessageEnd(void) = 0;

	// Print szMsg to the client console.
	virtual void ClientPrintf(edict_t *pEdict, const char *szMsg) = 0;

	// SINGLE PLAYER/LISTEN SERVER ONLY (just matching the client .dll api for
	// this) Prints the formatted string to the notification area of the screen
	// ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void Con_NPrintf(int pos, const char *fmt, ...) = 0;
	// SINGLE PLAYER/LISTEN SERVER ONLY(just matching the client .dll api for
	// this) Similar to Con_NPrintf, but allows specifying custom text color and
	// duration information
	virtual void Con_NXPrintf(
		const struct con_nprint_s *info, const char *fmt, ...
	) = 0;

	// Change a specified player's "view entity" (i.e., use the view entity
	// position/orientation for rendering the client view)
	virtual void SetView(const edict_t *pClient, const edict_t *pViewent) = 0;

	// Get a high precision timer for doing profiling work
	virtual float Time(void) = 0;

	// Set the player's crosshair angle
	virtual void CrosshairAngle(
		const edict_t *pClient, float pitch, float yaw
	) = 0;

	// Get the current game directory (hl2, tf2, hl1, cstrike, etc.)
	virtual void GetGameDir(char *szGetGameDir, int maxlength) = 0;

	// Used by AI node graph code to determine if .bsp and .ain files are out of
	// date
	virtual int CompareFileTime(
		const char *filename1, const char *filename2, int *iCompare
	) = 0;

	// Locks/unlocks the network string tables (.e.g, when adding bots to
	// server, this needs to happen). Be sure to reset the lock after executing
	// your code!!!
	virtual bool LockNetworkStringTables(bool lock) = 0;

	// Create a bot with the given name.  Returns NULL if fake client can't be
	// created
	virtual edict_t *CreateFakeClient(const char *netname) = 0;

	// Get a convar keyvalue for s specified client
	virtual const char *GetClientConVarValue(
		int clientIndex, const char *name
	) = 0;

	// Parse a token from a file
	virtual const char *ParseFile(
		const char *data, char *token, int maxlen
	) = 0;
	// Copies a file
	virtual bool CopyFile(const char *source, const char *destination) = 0;

	// Reset the pvs, pvssize is the size in bytes of the buffer pointed to by
	// pvs. This should be called right before any calls to AddOriginToPVS
	virtual void ResetPVS(byte *pvs, int pvssize) = 0;
	// Merge the pvs bits into the current accumulated pvs based on the
	// specified origin ( not that each pvs origin has an 8 world unit fudge
	// factor )
	virtual void AddOriginToPVS(const Vector &origin) = 0;

	// Mark a specified area portal as open/closed.
	// Use SetAreaPortalStates if you want to set a bunch of them at a time.
	virtual void SetAreaPortalState(int portalNumber, int isOpen) = 0;

	// Queue a temp entity for transmission
	virtual void PlaybackTempEntity(
		IRecipientFilter *filter,
		float delay,
		const void *pSender,
		const SendTable *pST,
		int classID
	) = 0;
	// Given a node number and the specified PVS, return with the node is in the
	// PVS
	virtual int CheckHeadnodeVisible(
		int nodenum, const byte *pvs, int vissize
	) = 0;
	// Using area bits, cheeck whether area1 flows into area2 and vice versa
	// (depends on area portal state)
	virtual int CheckAreasConnected(int area1, int area2) = 0;
	// Given an origin, determine which area index the origin is within
	virtual int GetArea(const Vector &origin) = 0;
	// Get area portal bit set
	virtual void GetAreaBits(int area, unsigned char *bits, int buflen) = 0;
	// Given a view origin (which tells us the area to start looking in) and a
	// portal key, fill in the plane that leads out of this area (it points into
	// whatever area it leads to).
	virtual bool GetAreaPortalPlane(
		Vector const &vViewOrigin, int portalKey, VPlane *pPlane
	) = 0;

	// Save/restore wrapper - FIXME:  At some point we should move this to it's
	// own interface
	virtual bool LoadGameState(char const *pMapName, bool createPlayers) = 0;
	virtual void LoadAdjacentEnts(
		const char *pOldLevel, const char *pLandmarkName
	) = 0;
	virtual void ClearSaveDir() = 0;

	// Get the pristine map entity lump string.  (e.g., used by CS to reload the
	// map entities when restarting a round.)
	virtual const char *GetMapEntitiesString() = 0;

	// Text message system -- lookup the text message of the specified name
	virtual client_textmessage_t *TextMessageGet(const char *pName) = 0;

	// Print a message to the server log file
	virtual void LogPrint(const char *msg) = 0;

	// Builds PVS information for an entity
	virtual void BuildEntityClusterList(
		edict_t *pEdict, PVSInfo_t *pPVSInfo
	) = 0;

	// A solid entity moved, update spatial partition
	virtual void SolidMoved(
		edict_t *pSolidEnt,
		ICollideable *pSolidCollide,
		const Vector *pPrevAbsOrigin,
		bool testSurroundingBoundsOnly
	) = 0;
	// A trigger entity moved, update spatial partition
	virtual void TriggerMoved(
		edict_t *pTriggerEnt, bool testSurroundingBoundsOnly
	) = 0;

	// Create/destroy a custom spatial partition
	virtual ISpatialPartition *CreateSpatialPartition(
		const Vector &worldmin, const Vector &worldmax
	) = 0;
	virtual void DestroySpatialPartition(ISpatialPartition *) = 0;

	// Draw the brush geometry in the map into the scratch pad.
	// Flags is currently unused.
	virtual void DrawMapToScratchPad(
		IScratchPad3D *pPad, unsigned long iFlags
	) = 0;

	// This returns which entities, to the best of the server's knowledge, the
	// client currently knows about. This is really which entities were in the
	// snapshot that this client last acked. This returns a bit vector with one
	// bit for each entity.
	//
	// USE WITH CARE. Whatever tick the client is really currently on is subject
	// to timing and ordering differences, so you should account for about a
	// quarter-second discrepancy in here. Also, this will return NULL if the
	// client doesn't exist or if this client hasn't acked any frames yet.
	//
	// iClientIndex is the CLIENT index, so if you use pPlayer->entindex(),
	// subtract 1.
	virtual const void *GetEntityTransmitBitsForClient(int iClientIndex) = 0;

	// Is the game paused?
	virtual bool IsPaused() = 0;

	// Marks the filename for consistency checking.  This should be called after
	// precaching the file.
	virtual void ForceExactFile(const char *s) = 0;
	virtual void ForceModelBounds(
		const char *s, const Vector &mins, const Vector &maxs
	) = 0;
	virtual void ClearSaveDirAfterClientLoad() = 0;

	// Sets a USERINFO client ConVar for a fakeclient
	virtual void SetFakeClientConVarValue(
		edict_t *pEntity, const char *cvar, const char *value
	) = 0;

	// Marks the material (vmt file) for consistency checking.  If the client
	// and server have different contents for the file, the client's vmt can
	// only use the VertexLitGeneric shader, and can only contain $baseTexture
	// and $bumpmap vars.
	virtual void ForceSimpleMaterial(const char *s) = 0;

	// Is the engine in Commentary mode?
	virtual int IsInCommentaryMode(void) = 0;

	// Mark some area portals as open/closed. It's more efficient to use this
	// than a bunch of individual SetAreaPortalState calls.
	virtual void SetAreaPortalStates(
		const int *portalNumbers, const int *isOpen, int nPortals
	) = 0;

	// Called when relevant edict state flags change.
	virtual void NotifyEdictFlagsChange(int iEdict) = 0;

	// Only valid during CheckTransmit. Also, only the PVS, networked areas, and
	// m_pTransmitInfo are valid in the returned strucutre.
	virtual const CCheckTransmitInfo *GetPrevCheckTransmitInfo(
		edict_t *pPlayerEdict
	) = 0;

	virtual CSharedEdictChangeInfo *GetSharedEdictChangeInfo() = 0;

	// Tells the engine we can immdiately re-use all edict indices
	// even though we may not have waited enough time
	virtual void AllowImmediateEdictReuse() = 0;

	// Returns true if the engine is an internal build. i.e. is using the
	// internal bugreporter.
	virtual bool IsInternalBuild(void) = 0;

	virtual IChangeInfoAccessor *GetChangeAccessor(const edict_t *pEdict) = 0;

	// Name of most recently load .sav file
	virtual char const *GetMostRecentlyLoadedFileName() = 0;
	virtual char const *GetSaveFileName() = 0;

	// Matchmaking
	virtual void MultiplayerEndGame() = 0;
	virtual void ChangeTeam(const char *pTeamName) = 0;

	// Cleans up the cluster list
	virtual void CleanUpEntityClusterList(PVSInfo_t *pPVSInfo) = 0;

	virtual void SetAchievementMgr(IAchievementMgr *pAchievementMgr) = 0;
	virtual IAchievementMgr *GetAchievementMgr() = 0;

	virtual int GetAppID() = 0;

	virtual bool IsLowViolence() = 0;

	// Call this to find out the value of a cvar on the client.
	//
	// It is an asynchronous query, and it will call
	// IServerGameDLL::OnQueryCvarValueFinished when the value comes in from the
	// client.
	//
	// Store the return value if you want to match this specific query to the
	// OnQueryCvarValueFinished call. Returns InvalidQueryCvarCookie if the
	// entity is invalid.
	virtual QueryCvarCookie_t StartQueryCvarValue(
		edict_t *pPlayerEntity, const char *pName
	) = 0;

	virtual void InsertServerCommand(const char *str) = 0;

	// Fill in the player info structure for the specified player index (name,
	// model, etc.)
	virtual bool GetPlayerInfo(int ent_num, player_info_t *pinfo) = 0;

	// Returns true if this client has been fully authenticated by Steam
	virtual bool IsClientFullyAuthenticated(edict_t *pEdict) = 0;

	// This makes the host run 1 tick per frame instead of checking the system
	// timer to see how many ticks to run in a certain frame. i.e. it does the
	// same thing timedemo does.
	virtual void SetDedicatedServerBenchmarkMode(bool bBenchmarkMode) = 0;

	// Methods to set/get a gamestats data container so client & server running
	// in same process can send combined data
	virtual void SetGamestatsData(CGamestatsData *pGamestatsData) = 0;
	virtual CGamestatsData *GetGamestatsData() = 0;

	// Returns the SteamID of the specified player. It'll be NULL if the player
	// hasn't authenticated yet.
	virtual const CSteamID *GetClientSteamID(edict_t *pPlayerEdict) = 0;

	// Returns the SteamID of the game server
	virtual const CSteamID *GetGameServerSteamID() = 0;

	// Send a client command keyvalues
	// keyvalues are deleted inside the function
	virtual void ClientCommandKeyValues(
		edict_t *pEdict, KeyValues *pCommand
	) = 0;

	// Returns the SteamID of the specified player. It'll be NULL if the player
	// hasn't authenticated yet.
	virtual const CSteamID *GetClientSteamIDByPlayerIndex(int entnum) = 0;
	// Gets a list of all clusters' bounds.  Returns total number of clusters.
	virtual int GetClusterCount() = 0;
	virtual int GetAllClusterBounds(bbox_t *pBBoxList, int maxBBox) = 0;

	// Create a bot with the given name.  Returns NULL if fake client can't be
	// created
	virtual edict_t *CreateFakeClientEx(
		const char *netname, bool bReportFakeClient = true
	) = 0;

	// Server version from the steam.inf, this will be compared to the GC
	// version
	virtual int GetServerVersion() const = 0;

	// Get sv.GetTime()
	virtual float GetServerTime() const = 0;
};

IVEngineServer *GetEngineServer();

#endif	// IVENGINESERVER_H
