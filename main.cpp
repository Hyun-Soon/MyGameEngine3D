#include "Application.h"

/*
 * TODO
 * 1. Implement SkinnedMeshModel
 */

// 디버깅 위해서 main 함수 사용. 위 함수 사용하려면 sln -> properties -> Linker -> System -> Subsystem -> Windows로 설정
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
