#pragma once

#include "M2.h"

class CM2_Manager : public IM2Manager, public CRefManager1Dim<M2>
{
public:
	CM2_Manager(IBaseManager& BaseManager);
	virtual ~CM2_Manager();

	// IM2Manager
	std::shared_ptr<M2> Add(IRenderDevice& RenderDevice, const std::string& name) override { return CRefManager1Dim::Add(RenderDevice, name); }
	bool Exists(const std::string& name) const override { return CRefManager1Dim::Exists(name); }
	void Delete(const std::string& name) override { CRefManager1Dim::Delete(name); }
	void Delete(std::shared_ptr<M2> item) override { CRefManager1Dim::Delete(item); }

	// CRefManager1Dim
	std::shared_ptr<M2> CreateAction(IRenderDevice& RenderDevice, const std::string& name) override;
	bool DeleteAction(const std::string& name) override;

private:
	std::shared_ptr<M2> m_DefaultModel;
	IBaseManager& m_BaseManager;
};
