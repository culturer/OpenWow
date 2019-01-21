#include <stdafx.h>

// General
#include "UI_Font_Material.h"

// Additional
#include "Application.h"

UI_Font_Material::UI_Font_Material() :
	Material(_RenderDevice)
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();
	m_pConstantBuffer = m_RenderDevice->CreateConstantBuffer(*m_pProperties);

	// CreateShaders
	std::shared_ptr<Shader> g_pVertexShader = _RenderDevice->CreateShader(
		Shader::VertexShader, "shaders_D3D/UI/UI_Font.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
	);
	std::shared_ptr<Shader> g_pPixelShader = _RenderDevice->CreateShader(
		Shader::PixelShader, "shaders_D3D/UI/UI_Font.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
	);

	// Create samplers
	std::shared_ptr<SamplerState> g_LinearClampSampler = _RenderDevice->CreateSamplerState();
	g_LinearClampSampler->SetFilter(SamplerState::MinFilter::MinLinear, SamplerState::MagFilter::MagLinear, SamplerState::MipFilter::MipLinear);
	g_LinearClampSampler->SetWrapMode(SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp);

	std::shared_ptr<SamplerState> g_LinearRepeatSampler = _RenderDevice->CreateSamplerState();
	g_LinearRepeatSampler->SetFilter(SamplerState::MinFilter::MinLinear, SamplerState::MagFilter::MagLinear, SamplerState::MipFilter::MipLinear);
	g_LinearRepeatSampler->SetWrapMode(SamplerState::WrapMode::Repeat, SamplerState::WrapMode::Repeat, SamplerState::WrapMode::Repeat);

	g_pPixelShader->GetShaderParameterByName("DiffuseTextureSampler").Set(g_LinearRepeatSampler);

	// Material
	SetShader(Shader::VertexShader, g_pVertexShader);
	SetShader(Shader::PixelShader, g_pPixelShader);
}

UI_Font_Material::~UI_Font_Material()
{
	if (m_pProperties)
	{
		_aligned_free(m_pProperties);
		m_pProperties = nullptr;
	}
}

void UI_Font_Material::SetColor(vec4 color)
{
	m_pProperties->Color = color;
	m_Dirty = true;
}

void UI_Font_Material::SetOffset(vec2 offset)
{
	m_pProperties->Offset = offset;
	m_Dirty = true;
}

void UI_Font_Material::UpdateConstantBuffer() const
{
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Set(*m_pProperties);
	}
}