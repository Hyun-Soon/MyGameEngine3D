#include "Application.h"

/*
 * TODO
 * 1. Implement SkinnedMeshModel
 */

// ����� ���ؼ� main �Լ� ���. �� �Լ� ����Ϸ��� sln -> properties -> Linker -> System -> Subsystem -> Windows�� ����
// int main()
// int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE, char*, int)

// int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
int main()
{
	const int	width = 1280;
	const int	height = 960;
	Application app(width, height);

	if (!app.Initialize())
		return 1;

	app.Run();

	return 0;
}
