#include "pch.h"
#include "MainApp.h"

namespace
{
    MainApp* g_pMainApp = nullptr;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// (����) Ư�� �Ҵ� ��ȣ���� �ߴ��� �ɱ�
	_CrtSetBreakAlloc(5504);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
		return -1;

    g_pMainApp = new MainApp();

	if (!g_pMainApp->Initialize())
	{
		std::cerr << "Failed to initialize sample code." << std::endl;
		return -1;
	}

	g_pMainApp->Run();

	g_pMainApp->Finalize();

    delete g_pMainApp;

	CoUninitialize();

	return 0;
}
