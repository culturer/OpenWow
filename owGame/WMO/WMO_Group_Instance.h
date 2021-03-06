#pragma once

#include "WMO_Group.h"
#include "WMO_Portal_Instance.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
#ifdef USE_M2_MODELS
class CWMO_Doodad_Instance;
#endif
class CWMO_Liquid_Instance;
// FORWARD END

class CWMO_Group_Instance 
	: public CSceneNode
	, public CLoadableObject
	, public IPortalRoom
{
public:
#ifdef USE_M2_MODELS
	typedef std::vector<CWMO_Doodad_Instance*> DoodadInstances;
#endif
	typedef std::vector<CWMO_Liquid_Instance*> LuqidInstances;

public:
	CWMO_Group_Instance(const std::shared_ptr<WMO_Group>& WMOGroupObject);
	virtual ~CWMO_Group_Instance();

	// CLoadableObject
	bool Load() override;

	// IPortalRoom
	void AddPortal(const std::shared_ptr<IPortal>& Portal) override final;
	const std::vector<std::shared_ptr<IPortal>>& GetPortals() const override final;
	void AddRoomObject(const std::weak_ptr<IPortalRoomObject>& RoomObject) override final;
	const std::vector<std::weak_ptr<IPortalRoomObject>>& GetRoomObjects() const override final;
	void Reset() override final;
	BoundingBox GetBoundingBox() const override final;
	void SetVisibilityState(bool Value) override final;
	void SetCalculatedState(bool Value) override final;
	bool IsCalculated() const override final;

	// CWMO_Group_Instance
	void CreatePortals(const std::shared_ptr<CWMO_Base_Instance>& BaseInstance);
    const WMO_Group& getObject() const { return m_WMOGroupObject; }

	// SceneNode3D
	void Initialize() override;
	void Accept(IVisitor* visitor) override;

private:
	std::vector<std::shared_ptr<IPortal>>         m_RoomPortals;
	std::vector<std::weak_ptr<IPortalRoomObject>> m_PortalRoomObjects;
	bool                                          m_PortalsVis;
	bool                                          m_Calculated;

private:
	const WMO_Group& m_WMOGroupObject;
};