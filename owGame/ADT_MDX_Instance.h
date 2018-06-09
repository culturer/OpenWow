﻿#pragma once

struct ADT_MDDF
{
	uint32 nameIndex;
	uint32 uniqueId;
	vec3 position;
	vec3 rotation;
	uint16 scale;
    struct
    {
        uint16 Flag_Biodome : 1;
        uint16 Flag_Shrubbery : 1;
        uint16 : 14;
    } flags;
};

class ADT_MDX_Instance : public SceneNode
{
public:
	ADT_MDX_Instance(SceneNode* _parent, M2* _mdxObject, ADT_MDDF _placementInfo);

    M2* GetMDX() { return m_Object; }

	// IUpdateble
	void Update(double _time, double _dTime) override;

	// IRenderable3D
	void PreRender3D() override;
	void Render3D()override;

public: 
	SmartM2Ptr	m_Object;
	uint32		m_UniqueId;

public:	// Static
	static void reset();
private:
	static std::set<uint32> m_AlreadyDraw;
};