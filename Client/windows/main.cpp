#include <stdio.h>
#include <memory>
#include "WindowsBluetoothConnector.h"
#include "CommandSerializer.h"
#include "BluetoothWrapper.h"

#include "WindowsGUI.h"

int main()
{
	SetProcessDPIAware();
	SetConsoleOutputCP(65001);
	std::cout << "Initializing... If can't see the GUI, something has gone wrong." << std::endl;
	try
	{
		std::unique_ptr<IBluetoothConnector> connector = std::make_unique<WindowsBluetoothConnector>();
		BluetoothWrapper wrap(std::move(connector));
		EnterGUIMainLoop(std::move(wrap));
	}
	catch (const std::exception& e)
	{
		DisplayErrorMessagebox(e.what());
	}
}

