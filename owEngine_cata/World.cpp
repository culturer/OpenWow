#include "stdafx.h"

// General
#include "world.h"


World::World()
{
	// SHADERS
	mainCamera = new Camera;
	testCamera = new Camera;
	_PipelineGlobal->SetCamera(mainCamera);
	_PipelineGlobal->SetProjection(45.0f, _Settings->aspectRatio, 1.0f, 10000.0f);

	_EnvironmentManager->Init();

	//----------------------------------------------------------------//

	glGenTextures(1, &finalTexture1);
	glBindTexture(GL_TEXTURE_2D, finalTexture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Settings->windowSizeX, _Settings->windowSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &finalTexture2);
	glBindTexture(GL_TEXTURE_2D, finalTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Settings->windowSizeX, _Settings->windowSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	m_gbuffer = new GBuffer();
	m_gbuffer->Init();

	//----------------------------------------------------------------//

	_Settings->CalculateSquareDistances();

	// Fog params
	l_const = 0.0f;
	l_linear = 0.7f;
	l_quadratic = 0.03f;

	_TechniquesMgr->Init();
}

World::~World()
{
	Debug::Info("Unloaded world [%s]", _Map->GetPath().c_str());

	// temp code until I figure out water properly
	//if (water)
	//	video.textures.del(water);


	_EnvironmentManager->Destroy();

	Debug::Info("World [%s] unloaded", _Map->GetPath().c_str());
}

void World::initDisplay()
{
	_EnvironmentManager->InitSkies(_Map->GetTemplate()->Get_ID());
}

void World::drawShader(GLint _color)
{
#ifdef WMO_INCL
	WMOInstance::reset();
#endif

#ifdef MDX_INCL
	_ModelsMgr->resetAnim();
#endif

	_PipelineGlobal->SetCamera(mainCamera);
	_Perfomance->FrameBegin();
	_EnvironmentManager->BeforeDraw();
	_Render->frustum.retrieve();

	// Main frame
	m_gbuffer->StartFrame(finalTexture1);
	m_gbuffer->ClearFinalBuffer();
	
	// Geometry pass
	m_gbuffer->BindForGeomPass();
	m_gbuffer->Clear();
	RenderGeom();
	_PipelineGlobal->RenderCamera(mainCamera);

	// Postprocess pass
	m_gbuffer->BindForLightPass();
	RenderPostprocess();

	// Final pass
	m_gbuffer->BindForFinalPass(_color);


#ifdef WMO_INCL
	WMOInstance::reset();
#endif

	//
	// SECONDS PASS
	//
	
	// Conf test camera
	testCamera->Position = mainCamera->Position + vec3(0, 1, 0) * 1000.0f;
	testCamera->Yaw = mainCamera->Yaw;
	testCamera->Pitch = -89.0f;
	testCamera->Update();

	// Test frame
	_PipelineGlobal->SetCamera(testCamera);
	m_gbuffer->StartFrame(finalTexture2);
	m_gbuffer->ClearFinalBuffer();

	// Geometry pass
	m_gbuffer->BindForGeomPass();
	m_gbuffer->Clear();
	RenderGeom();
	

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

//***************************************

void World::RenderGeom()
{
	DSGeometryPassBegin();//////
	////////////////////////////
	m_gbuffer->Clear();


	//------------------------------------------------------------------------------
	// Draw sky
	//------------------------------------------------------------------------------
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	_EnvironmentManager->skies->drawSky(_Camera->Position);


	//------------------------------------------------------------------------------
	// Draw sky from WMO
	//------------------------------------------------------------------------------
	//if (_Settings->draw_map_mdx)
	//{
	_Map->RenderSky();
	//}

	//------------------------------------------------------------------------------
	// Draw sky from GLOBAL WMO
	//------------------------------------------------------------------------------
	if (_Map->MapHasGlobalWMO() && !_EnvironmentManager->m_HasSky)
	{
#ifdef WMO_INCL
#ifdef MDX_INCL
		_Map->SetOutOfBounds(false);
		_Map->GetGlobalWMOInstance()->GetWMO()->drawSkybox();
#endif
#endif
	}

	/*glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	DSGeometryPassEnd();
	DSSimpleRenderPass222();
	m_gbuffer->BindForFinalPass(_color);
	m_gbuffer->Clear();
	//m_gbuffer->StartFrame();
	DSGeometryPassBegin();*/

	//

	PERF_START(PERF_MAP);

	//------------------------------------------------------------------------------
	// Map low-resolution tiles
	//------------------------------------------------------------------------------
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	PERF_START(PERF_MAP_LOWRESOLUTION);
	if (/*_Settings->drawfog &&*/ _Settings->draw_map_chunk)
	{
		_TechniquesMgr->m_MapTileLowRes_GeometryPass->Bind();
		_Pipeline->Clear();
		_TechniquesMgr->m_MapTileLowRes_GeometryPass->SetPVW();
		_TechniquesMgr->m_MapTileLowRes_GeometryPass->SetShadowColor(_EnvironmentManager->GetSkyColor(FOG_COLOR));

		_Map->RenderLowResTiles();

		_TechniquesMgr->m_MapTileLowRes_GeometryPass->Unbind();
	}
	PERF_STOP(PERF_MAP_LOWRESOLUTION);

	//

	//------------------------------------------------------------------------------
	// Map chunks
	//------------------------------------------------------------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	PERF_START(PERF_MAP_CHUNK_GEOMETRY);
	if (_Settings->draw_map_chunk)
	{
		_Settings->uselowlod = _Settings->drawfog;

		_TechniquesMgr->m_MapChunk_GeometryPass->Bind();
		_Pipeline->Clear();
		_TechniquesMgr->m_MapChunk_GeometryPass->SetPVW();
		_TechniquesMgr->m_MapChunk_GeometryPass->SetWorldMatrix(_Pipeline->GetWorld());

		_Map->RenderTiles();

		_TechniquesMgr->m_MapChunk_GeometryPass->Unbind();
	}
	PERF_STOP(PERF_MAP_CHUNK_GEOMETRY);

	//

	//------------------------------------------------------------------------------
	// Map water
	//------------------------------------------------------------------------------
	glDisable(GL_CULL_FACE);
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//if (_Settings->draw_map_chunk)
	{
		_TechniquesMgr->m_WMO_MH2O_GeometryPass->Bind();
		_Pipeline->Clear();
		_TechniquesMgr->m_WMO_MH2O_GeometryPass->SetPVW();
		_TechniquesMgr->m_WMO_MH2O_GeometryPass->SetWorldMatrix(_Pipeline->GetWorld());

		_Map->RenderWater();

		_TechniquesMgr->m_WMO_MH2O_GeometryPass->Unbind();
	}

	//

	PERF_STOP(PERF_MAP);

	//

	//------------------------------------------------------------------------------
	// Global WMO
	//------------------------------------------------------------------------------
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	PERF_START(PERF_MAP_MODELS_WMO_GLOBAL);
	if (_Map->MapHasGlobalWMO())
	{
#ifdef WMO_INCL
		_Map->SetOutOfBounds(false);
		_Map->GetGlobalWMOInstance()->draw();

#endif
	}
	PERF_STOP(PERF_MAP_MODELS_WMO_GLOBAL);

	//

	//------------------------------------------------------------------------------
	// WMOs
	//------------------------------------------------------------------------------
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	PERF_START(PERF_MAP_MODELS_WMOs);
	if (_Settings->draw_map_wmo)
	{
		_Map->RenderObjects();
	}
	PERF_STOP(PERF_MAP_MODELS_WMOs);

	//

	//------------------------------------------------------------------------------
	// Map MDXs
	//------------------------------------------------------------------------------
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	PERF_START(PERF_MAP_MODELS_MDXs);
	if (_Settings->draw_map_mdx)
	{
		_Map->RenderModels();
	}
	PERF_STOP(PERF_MAP_MODELS_MDXs);

	//

	////////////////////////////
	DSGeometryPassEnd();////////
}

void World::RenderPostprocess()
{
	//DSSimpleRenderPass();

	// LIGHTS, FOG ...
	DirectionalLight dirLight;
	dirLight.ambient = _EnvironmentManager->skies->colorSet[LIGHT_GLOBAL_AMBIENT];
	dirLight.diffuse = _EnvironmentManager->skies->colorSet[LIGHT_GLOBAL_DIFFUSE];
	dirLight.Direction.x = 0;
	dirLight.Direction.y = 0;
	dirLight.Direction.z = 0;
	DSDirectionalLightPass(dirLight);
}

void World::tick(float dt)
{
	_Settings->CalculateSquareDistances();

	_Map->Tick();

#ifdef MDX_INCL
	while (dt > 0.1f)
	{
		_ModelsMgr->updateEmitters(0.1f);
		dt -= 0.1f;
	}

	_ModelsMgr->updateEmitters(dt);
#endif
}




void World::DSGeometryPassBegin()
{
	//m_gbuffer->BindForGeomPass();

	//glDepthMask(GL_TRUE);
	//glEnable(GL_DEPTH_TEST);
}

void World::DSGeometryPassEnd()
{
	//glDepthMask(GL_FALSE);
}

void World::DSDirectionalLightPass(DirectionalLight& _light)
{
	

	_TechniquesMgr->m_DSDirLightPassTech->Bind();
	_TechniquesMgr->m_DSDirLightPassTech->SetEyeWorldPos(_Camera->Position);
	_TechniquesMgr->m_DSDirLightPassTech->SetDirectionalLight(_light);

	_TechniquesMgr->m_DSDirLightPassTech->BindToPostprocess();

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBegin(GL_QUADS);
	{
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	}
	glEnd();

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	_TechniquesMgr->m_DSDirLightPassTech->Unbind();
}

void World::DSSimpleRenderPass()
{
	m_gbuffer->BindForLightPass();

	_TechniquesMgr->m_SimpleRender->Bind();

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBegin(GL_QUADS);
	{
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	}
	glEnd();

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	_TechniquesMgr->m_SimpleRender->Unbind();
}