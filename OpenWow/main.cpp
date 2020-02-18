#include "stdafx.h"

static IBaseManager* BaseManager = nullptr;

#include "SceneWoW.h"
#include "SceneWoW2.h"

void main_internal(int argumentCount, char* arguments[])
{
	// 1. Initialize engine and some improtant managers
	BaseManager = InitializeEngine(ArgumentsToVector(argumentCount, arguments), "");

	// 3. Create application
	Application app(*BaseManager, ::GetModuleHandle(NULL));

	CNativeWindowFactory nativeWindowFactory(&app);

	std::unique_ptr<INativeWindow> nativeWindow = nativeWindowFactory.CreateWindowInstance(
		L"Zenon Engine",
		BaseManager->GetManager<ISettings>()->GetGroup("Video")->GetSettingT<glm::vec2>("WindowSize")->Get().x,
		BaseManager->GetManager<ISettings>()->GetGroup("Video")->GetSettingT<glm::vec2>("WindowSize")->Get().y
	);

	//std::unique_ptr<INativeWindow> nativeWindow2 = nativeWindowFactory.CreateWindowInstance(
	//	L"Zenon Engine test window",
	//	BaseManager->GetManager<ISettings>()->GetGroup("Video")->GetSettingT<glm::vec2>("WindowSize")->Get().x,
	//	BaseManager->GetManager<ISettings>()->GetGroup("Video")->GetSettingT<glm::vec2>("WindowSize")->Get().y
	//);

	IRenderDevice& renderDevice = app.CreateRenderDevice(RenderDeviceType::RenderDeviceType_DirectX);

	std::shared_ptr<IFontsManager> fontsManager = std::make_shared<FontsManager>(renderDevice, *BaseManager);
	BaseManager->AddManager<IFontsManager>(std::move(fontsManager));

	const auto& firstRenderWindow = renderDevice.GetObjectsFactory().CreateRenderWindow(*nativeWindow, false);
	app.AddRenderWindow(firstRenderWindow);

	//const auto& secondRenderWindow = renderDevice.GetObjectsFactory().CreateRenderWindow(*nativeWindow2, false);
	//app.AddRenderWindow(secondRenderWindow);

	auto environmentManager = BaseManager->AddManager<EnvironmentManager>(std::make_shared<EnvironmentManager>(*BaseManager));

	BaseManager->GetManager<ILoader>()->SetCamera(nullptr);
	BaseManager->GetManager<ILoader>()->Start();

	std::shared_ptr<IScene> scene = std::make_shared<CSceneWoW>(*BaseManager);//BaseManager->GetManager<IScenesFactory>()->CreateScene("SceneDefault");
	scene->ConnectEvents(std::dynamic_pointer_cast<IRenderWindowEvents>(firstRenderWindow));
	scene->Initialize();

	//std::shared_ptr<IScene> scene2 = std::make_shared<CSceneWoW2>(*BaseManager, scene->GetRootNode3D(), scene->GetCameraController()->GetCamera());//BaseManager->GetManager<IScenesFactory>()->CreateScene("SceneDefault");
	//scene2->ConnectEvents(std::dynamic_pointer_cast<IRenderWindowEvents>(secondRenderWindow));
	//scene2->Initialize();

	

	app.Run();
}


int main(int argumentCount, char* arguments[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	//_CrtSetBreakAlloc(158);

	main_internal(argumentCount, arguments);		

	delete BaseManager;

	_CrtMemDumpAllObjectsSince(NULL);

	return 0;
}
