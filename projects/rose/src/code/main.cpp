//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <iostream>
#include <string>

#include <revCore/codeTools/log/log.h>
#include <component/microcontroller/avr/avr8bit.h>

using namespace rev;

using rose::component::Avr8bit;
using namespace std;

int main()
{
	// System initialization
	codeTools::Log::init();
	revLog() << "Rose: Robotic Simulation Environment\n";

	// Actual code
	// revLog() << "Creating AVR Core ... ";
	// Avr8bit * core = new Avr8bit(256, 8);
	// revLog() << "Success\n";
	// core->loadProgram("ciervaPilot.hex");
	// core->reset();

	bool exit = false;
	while(!exit)
	{
		/*cout << "--->";
		string instruction;
		cin >> instruction;
		switch(instruction[0])
		{
			case 's': // Simulate
			{
				unsigned cycles;
				cin >> cycles;
				revLog() << "Simulate " << cycles << " cycles\n";
				core->simulate(cycles);
				core->showExecutionStatus();
				break;
			}
			case 't':
			{
				core->step();
				break;
			}
			case 'a': // Assembly
			{
				unsigned start, end;
				cin >> start >> end;
				core->showAssembly(start, end);
				break;
			}
			case 'b': // Breakpoint
			{
				unsigned instruction;
				cin >> instruction;
				core->setBreakpoint(instruction);
				break;
			}
			case 'r': // run
			{
				core->run();
				break;
			}
			case 'm': // Memory
			{
				unsigned start, end;
				cin >> start >> end;
				core->showMemory(start, end);
				break;
			}
			case 'q': // Quit
			{
				exit = true;
				break;
			}
		}*/
	}

	// House keeping
	codeTools::Log::end();
	return 0;
}