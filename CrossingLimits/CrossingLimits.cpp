// CrossingLimits.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "MaxMemoryCounter.h"
#include "MemoryFragmenter.h"


int main()
{
	//PrintMaxMemoryAndLastError();
	FragmentMemory();
	
	// This piece of crap is used to keep process running.
	int enterSomethingToCloseProcess = 0;
	std::cin >> enterSomethingToCloseProcess;

	return 0;
}
