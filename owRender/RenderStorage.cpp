#include "stdafx.h"

// General
#include "RenderStorage.h"

RenderStorage::RenderStorage(RenderDevice* _RenderDevice)
	: m_RenderDevice(_RenderDevice)
{
	CreateWoWLayouts();


	CreateLayouts();
	CreateGeometry();
}

RenderStorage::~RenderStorage()
{

}

void RenderStorage::CreateLayouts()
{
	R_VertexLayoutAttrib attribsSky[2] = {
		{"VertexPosition", 0, 3, 0},
		{"color",          1, 3, 0}
	};
	__layoutSky = m_RenderDevice->registerVertexLayout(2, attribsSky);
	//--------------------------------------------------------------------------------------------
	R_VertexLayoutAttrib attribsWMO[3] = {
		{"VertexPosition",      0, 3, 0},
		{"textureCoords",  1, 2, 0},
		{"normal",         2, 3, 0}
	};
	__layoutWMO = m_RenderDevice->registerVertexLayout(3, attribsWMO);

	R_VertexLayoutAttrib attribsWMO_VC[4] = {
		{"VertexPosition",      0, 3, 0},
		{"textureCoords",  1, 2, 0},
		{"normal",         2, 3, 0},
		{"color",          3, 4, 0}
	};
	__layoutWMO_VC = m_RenderDevice->registerVertexLayout(4, attribsWMO_VC);
	//--------------------------------------------------------------------------------------------
	R_VertexLayoutAttrib attribsWater[3] = {
		{"VertexPosition", 0, 3, 0},
		{"textureCoords",  1, 3, 0},
		{"normal",         2, 3, 0}
	};
	__layoutWater = m_RenderDevice->registerVertexLayout(3, attribsWater);
}

void RenderStorage::CreateGeometry()
{
	// Indexes
	uint16 indexes[6] = { 0, 1, 2, 2, 1, 3 };
	__ibQuadDefault = m_RenderDevice->createIndexBuffer(sizeof(indexes), indexes);

	vector<vec3> verticesQuad;
	verticesQuad.push_back(vec3(-1.0f, -1.0f, 1.0f));
	verticesQuad.push_back(vec3(1.0f, -1.0f, 1.0f));
	verticesQuad.push_back(vec3(-1.0f, 1.0f, 1.0f));
	verticesQuad.push_back(vec3(1.0f, 1.0f, 1.0f));

	vector<vec2> texCoordsQuad;
	texCoordsQuad.push_back(vec2(0.0f, 0.0f));
	texCoordsQuad.push_back(vec2(1.0f, 0.0f));
	texCoordsQuad.push_back(vec2(0.0f, 1.0f));
	texCoordsQuad.push_back(vec2(1.0f, 1.0f));

	//-----------------------------------------
	// Unit quad
	//-----------------------------------------

	R_Buffer* __vbQuad = m_RenderDevice->createVertexBuffer(4 * sizeof(vec3), verticesQuad.data());
	__Quad = m_RenderDevice->beginCreatingGeometry(__layout_GxVBF_P);
	__Quad->setGeomVertexParams(__vbQuad, R_DataType::T_FLOAT, 0, sizeof(vec3));
	__Quad->setGeomIndexParams(__ibQuadDefault, R_IndexFormat::IDXFMT_16);
	__Quad->finishCreatingGeometry();

	//-----------------------------------------
	// Unit quad with texture coords
	//-----------------------------------------

	R_Buffer* __vbQuadVT = m_RenderDevice->createVertexBuffer(4 * sizeof(Texture_Vertex), nullptr);
	__vbQuadVT->updateBufferData(0, 4 * sizeof(vec3), verticesQuad.data());
	__vbQuadVT->updateBufferData(4 * sizeof(vec3), 4 * sizeof(vec2), texCoordsQuad.data());

	__QuadVT = m_RenderDevice->beginCreatingGeometry(__layout_GxVBF_PT);
	__QuadVT->setGeomVertexParams(__vbQuadVT, R_DataType::T_FLOAT, 0, 0);
	__QuadVT->setGeomVertexParams(__vbQuadVT, R_DataType::T_FLOAT, 4 * sizeof(vec3), 0);
	__QuadVT->setGeomIndexParams(__ibQuadDefault, R_IndexFormat::IDXFMT_16);
	__QuadVT->finishCreatingGeometry();

	//-----------------------------------------
	// Unit quad with updatable texture coords
	//-----------------------------------------

	__vbQuadVTDynamic = m_RenderDevice->createVertexBuffer(4 * sizeof(Texture_Vertex), nullptr);
	__vbQuadVTDynamic->updateBufferData(0, 4 * sizeof(vec3), verticesQuad.data());
	__vbQuadVTDynamic->updateBufferData(4 * sizeof(vec3), 4 * sizeof(vec2), texCoordsQuad.data());

	__QuadVTDynamic = m_RenderDevice->beginCreatingGeometry(__layout_GxVBF_PT);
	__QuadVTDynamic->setGeomVertexParams(__vbQuadVTDynamic, R_DataType::T_FLOAT, 0, 0);
	__QuadVTDynamic->setGeomVertexParams(__vbQuadVTDynamic, R_DataType::T_FLOAT, 4 * sizeof(vec3), 0);
	__QuadVTDynamic->setGeomIndexParams(__ibQuadDefault, R_IndexFormat::IDXFMT_16);
	__QuadVTDynamic->finishCreatingGeometry();

	//-----------------------------------------
	// Unit cube
	//-----------------------------------------

	float cubeVerts[8 * 3] = {  // x, y, z
		0.f, 0.f, 1.f,   1.f, 0.f, 1.f,   1.f, 1.f, 1.f,   0.f, 1.f, 1.f,
		0.f, 0.f, 0.f,   1.f, 0.f, 0.f,   1.f, 1.f, 0.f,   0.f, 1.f, 0.f
	};
	uint16 cubeInds[36] = {
		0, 1, 2, 2, 3, 0,   1, 5, 6, 6, 2, 1,   5, 4, 7, 7, 6, 5,
		4, 0, 3, 3, 7, 4,   3, 2, 6, 6, 7, 3,   4, 5, 1, 1, 0, 4
	};

	R_Buffer* _vbCube = m_RenderDevice->createVertexBuffer(8 * sizeof(vec3), cubeVerts);
	R_Buffer* _ibCube = m_RenderDevice->createIndexBuffer(36 * sizeof(uint16), cubeInds);

	_cubeGeo = m_RenderDevice->beginCreatingGeometry(__layout_GxVBF_P);
	_cubeGeo->setGeomVertexParams(_vbCube, R_DataType::T_FLOAT, 0, sizeof(vec3));
	_cubeGeo->setGeomIndexParams(_ibCube, IDXFMT_16);
	_cubeGeo->finishCreatingGeometry();

	//-----------------------------------------
	// Unit (geodesic) sphere (created by recursively subdividing a base octahedron)
	//-----------------------------------------

	vec3 spVerts[126] =
	{  // x, y, z
		vec3(0.0f, 1.0f, 0.0f),       vec3(0.0f, -1.0f, 0.0f),
		vec3(-0.707f, 0.0f, 0.707f),   vec3(0.707f, 0.0f, 0.707f),
		vec3(0.707f, 0.0f, -0.707f),   vec3(-0.707f, 0.0f, -0.707f)
	};
	uint16 spInds[128 * 3] = {  // Number of faces: (4 ^ iterations) * 8
		2, 3, 0,   3, 4, 0,   4, 5, 0,   5, 2, 0,   2, 1, 3,   3, 1, 4,   4, 1, 5,   5, 1, 2
	};
	for (uint32 i = 0, nv = 6, ni = 24; i < 2; ++i)  // Two iterations
	{
		// Subdivide each face into 4 tris by bisecting each edge and push vertices onto unit sphere
		for (uint32 j = 0, prevNumInds = ni; j < prevNumInds; j += 3)
		{
			spVerts[nv++] = ((spVerts[spInds[j + 0]] + spVerts[spInds[j + 1]]) * 0.5f).normalized();
			spVerts[nv++] = ((spVerts[spInds[j + 1]] + spVerts[spInds[j + 2]]) * 0.5f).normalized();
			spVerts[nv++] = ((spVerts[spInds[j + 2]] + spVerts[spInds[j + 0]]) * 0.5f).normalized();

			spInds[ni++] = spInds[j + 0];
			spInds[ni++] = nv - 3;
			spInds[ni++] = nv - 1;

			spInds[ni++] = nv - 3;
			spInds[ni++] = spInds[j + 1];
			spInds[ni++] = nv - 2;

			spInds[ni++] = nv - 2;
			spInds[ni++] = spInds[j + 2];
			spInds[ni++] = nv - 1;

			spInds[j + 0] = nv - 3;
			spInds[j + 1] = nv - 2;
			spInds[j + 2] = nv - 1;
		}
	}

	R_Buffer* _vbSphere = m_RenderDevice->createVertexBuffer(126 * sizeof(vec3), spVerts);
	R_Buffer* _ibSphere = m_RenderDevice->createIndexBuffer(128 * 3 * sizeof(uint16), spInds);

	_sphereGeo = m_RenderDevice->beginCreatingGeometry(__layout_GxVBF_P);
	_sphereGeo->setGeomVertexParams(_vbSphere, R_DataType::T_FLOAT, 0, sizeof(vec3));
	_sphereGeo->setGeomIndexParams(_ibSphere, IDXFMT_16);
	_sphereGeo->finishCreatingGeometry();

	//-----------------------------------------
	// Unit cone
	//-----------------------------------------

	float coneVerts[13 * 3] = {  // x, y, z
		0.f, 0.f, 0.f,
		0.f, 1.f, -1.f,   -0.5f, 0.866f, -1.f,   -0.866f, 0.5f, -1.f,
		-1.f, 0.f, -1.f,   -0.866f, -0.5f, -1.f,   -0.5f, -0.866f, -1.f,
		0.f, -1.f, -1.f,   0.5f, -0.866f, -1.f,   0.866f, -0.5f, -1.f,
		1.f, 0.f, -1.f,   0.866f, 0.5f, -1.f,   0.5f, 0.866f, -1.f,
	};
	uint16 coneInds[22 * 3] = {
		0, 1, 2,   0, 2, 3,   0, 3, 4,   0, 4, 5,   0, 5, 6,   0, 6, 7,
		0, 7, 8,   0, 8, 9,   0, 9, 10,   0, 10, 11,   0, 11, 12,   0, 12, 1,
		10, 6, 2,   10, 8, 6,   10, 9, 8,   8, 7, 6,   6, 4, 2,   6, 5, 4,   4, 3, 2,
		2, 12, 10,   2, 1, 12,   12, 11, 10
	};

	R_Buffer* _vbCone = m_RenderDevice->createVertexBuffer(13 * sizeof(vec3), coneVerts);
	R_Buffer* _ibCone = m_RenderDevice->createIndexBuffer(22 * 3 * sizeof(uint16), coneInds);

	_coneGeo = m_RenderDevice->beginCreatingGeometry(__layout_GxVBF_P);
	_coneGeo->setGeomVertexParams(_vbCone, R_DataType::T_FLOAT, 0, sizeof(vec3));
	_coneGeo->setGeomIndexParams(_ibCone, IDXFMT_16);
	_coneGeo->finishCreatingGeometry();
}

void RenderStorage::CreateWoWLayouts()
{
	R_VertexLayoutAttrib attribs_GxVBF_P[1] = { // 12
		{"position",    0, 3, 0}
	};
	__layout_GxVBF_P = m_RenderDevice->registerVertexLayout(1, attribs_GxVBF_P); // USED IN LOW-RESOLUTION TILES

	//--

	R_VertexLayoutAttrib attribs_GxVBF_PN[2] = { // 24
		{"position",    0, 3, 0},
		{"normal",      1, 3, 0}
	};
	__layout_GxVBF_PN = m_RenderDevice->registerVertexLayout(2, attribs_GxVBF_PN); // USED IN LOW-RESOLUTION TILES

	R_VertexLayoutAttrib attribs_GxVBF_PNC[3] = { // 28
		{"position",    0, 3, 0},
		{"normal",      1, 3, 0},
		{"color",       2, 1, 0}
	};

	//--

	R_VertexLayoutAttrib attribs_GxVBF_PNT[3] = { // 32
		{"position",    0, 3, 0},
		{"normal",      1, 3, 0},
		{"tc",          2, 2, 0}
	};

	R_VertexLayoutAttrib attribs_GxVBF_PNCT[4] = { // 36
		{"position",    0, 3, 0},
		{"normal",      1, 3, 0},
		{"color",       2, 1, 0},
		{"tc",          3, 2, 0}
	};

	//--

	R_VertexLayoutAttrib attribs_GxVBF_PNT2[4] = { // 40
		{"position",    0, 3, 0},
		{"normal",      1, 3, 0},
		{"tc0",         2, 2, 0},
		{"tc1",         3, 2, 0}
	};
	__layout_GxVBF_PNT2 = m_RenderDevice->registerVertexLayout(4, attribs_GxVBF_PNT2);

	R_VertexLayoutAttrib attribs_GxVBF_PNCT2[5] = { // 44
		{"position",    0, 3, 0},
		{"normal",      1, 3, 0},
		{"color",       2, 1, 0},
		{"tc0",         3, 2, 0},
		{"tc1",         4, 2, 0}
	};

	//--

	R_VertexLayoutAttrib attribs_GxVBF_PC[2] = { // 16
		{"position",    0, 3, 0},
		{"color",       1, 1, 0}
	};
	__layout_GxVBF_P� = m_RenderDevice->registerVertexLayout(2, attribs_GxVBF_PC); // USED IN M2


	R_VertexLayoutAttrib attribs_GxVBF_PCT[3] = { // 24
		{"position",    0, 3, 0},
		{"color",       1, 1, 0},
		{"tc",          2, 2, 0}
	};

	R_VertexLayoutAttrib attribs_GxVBF_PCT2[4] = { // 32
		{"position",    0, 3, 0},
		{"color",       1, 1, 0},
		{"tc0",         2, 2, 0},
		{"tc1",         3, 2, 0}
	};

	//--

	R_VertexLayoutAttrib attribs_GxVBF_PT[2] = { // 20
		{"position",    0, 3, 0},
		{"tc",          1, 2, 0}
	};
	__layout_GxVBF_PT = m_RenderDevice->registerVertexLayout(2, attribs_GxVBF_PT); // USED IN M2

	R_VertexLayoutAttrib attribs_GxVBF_PT2[3] = { // 28
		{"position",    0, 3, 0},
		{"tc0",         1, 2, 0},
		{"tc0",         2, 2, 0}
	};

	//--

	R_VertexLayoutAttrib attribs_GxVBF_PBNT2[6] = { // 48
		{"position",    0, 3, 0},
		{"blendWeight", 1, 1, 0},
		{"blendIndices",2, 1, 0},
		{"normal",      3, 3, 0},
		{"tc0",         4, 2, 0},
		{"tc1",         5, 2, 0}
	};
	__layout_GxVBF_PBNT2 = m_RenderDevice->registerVertexLayout(6, attribs_GxVBF_PBNT2); // USED IN M2

	R_VertexLayoutAttrib attribs_GxVBF_PNC2T2[6] = { // Original 48
		{"position",    0, 3, 0},
		{"normal",      1, 3, 0},
		{"color0",      2, 4, 0}, // Original 1 but type is uint8
		{"color1",      3, 4, 0}, // Original 1
		{"tc0",         4, 2, 0},
		{"tc1",         5, 2, 0}
	};
	__layout_GxVBF_PNC2T2 = m_RenderDevice->registerVertexLayout(6, attribs_GxVBF_PNC2T2); // USED IN MapChunk
}

//


void RenderStorage::SetEGxBlend(uint8 _index)
{
	switch (_index)
	{
	case 0: // Opaque
		m_RenderDevice->setBlendMode(false, BS_BLEND_ONE, BS_BLEND_ZERO);
		break;

	case 1: // AlphaKey
		m_RenderDevice->setBlendMode(false, BS_BLEND_ONE, BS_BLEND_ZERO);
		break;

	case 2: // Alpha
		m_RenderDevice->setBlendModeEx(false, BS_BLEND_SRC_ALPHA, BS_BLEND_INV_SRC_ALPHA, BS_BLEND_ONE, BS_BLEND_INV_SRC_ALPHA);
		break;

	case 3: // Add
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_SRC_ALPHA, BS_BLEND_ONE, BS_BLEND_ZERO, BS_BLEND_ONE);
		break;

	case 4: // Mod
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_DEST_COLOR, BS_BLEND_ZERO, BS_BLEND_DEST_ALPHA, BS_BLEND_ZERO);
		break;

	case 5: // Mod2x
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_DEST_COLOR, BS_BLEND_SRC_COLOR, BS_BLEND_DEST_ALPHA, BS_BLEND_SRC_ALPHA);
		break;

	case 6: // ModAdd
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_DEST_COLOR, BS_BLEND_ONE, BS_BLEND_DEST_ALPHA, BS_BLEND_ONE);
		break;

	case 7: // InvSrcAlphaAdd
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_INV_SRC_ALPHA, BS_BLEND_ONE, BS_BLEND_INV_SRC_ALPHA, BS_BLEND_ONE);
		break;

	case 8: // InvSrcAlphaOpaque
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_INV_SRC_ALPHA, BS_BLEND_ZERO, BS_BLEND_INV_SRC_ALPHA, BS_BLEND_ZERO);
		break;

	case 9: // SrcAlphaOpaque
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_SRC_ALPHA, BS_BLEND_ZERO, BS_BLEND_SRC_ALPHA, BS_BLEND_ZERO);
		break;

	case 10: // NoAlphaAdd
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_ONE, BS_BLEND_ONE, BS_BLEND_ZERO, BS_BLEND_ONE);
		break;

	case 11: // ConstantAlpha
	//GLSetBlend(true, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
		fail1("Not implemented!!!");
		break;

	case 12: // Screen
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_INV_DEST_COLOR, BS_BLEND_ONE, BS_BLEND_ONE, BS_BLEND_ZERO);
		break;

	case 13: // BlendAdd
		m_RenderDevice->setBlendModeEx(true, BS_BLEND_ONE, BS_BLEND_INV_SRC_ALPHA, BS_BLEND_ONE, BS_BLEND_INV_SRC_ALPHA);
		break;

	default:
		fail2(std::to_string(_index).c_str());
	}
}
