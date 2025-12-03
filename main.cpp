
#include "Internals/VM.h"

int main() {

	VM myVM;
	bool isLoaded = myVM.loadProgram("C:\\sample.zpp");
	
	if (isLoaded)
		myVM.run();

	return 0;
}