// CrossingLimits.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "ErrorLogger.h"

#include "MaxMemoryCounter.h"
#include "MemoryFragmenter.h"
#include "MaxHandleCounter.h"
#include "MaxPensCounter.h"
#include "FunctionCallMemoryCounter.h"
#include "PathLengthMeasurer.h"

int main()
{
	//PrintMaxMemoryAndLastError();
	FragmentMemory();
	//PrintMaxHandlesCount();
	//PrintMaxPensCount();
	//DoNothing();
	//DoNotingWithIntVariable();
	//PrintMaxPathLength();

	Log();

	// This piece of crap is used to keep process running.
	int enterSomethingToCloseProcess = 0;
	std::cin >> enterSomethingToCloseProcess;

	return 0;
}
