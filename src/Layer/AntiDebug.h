#pragma once

#include <pch.h>
#include <Icetrix.h>

class AntiDebug
{
public:
	AntiDebug()
	{
		g_sApp.dispatcher.sink<Icetrix::LayerEvent::Attach>().connect<&AntiDebug::Attach>(*this);
	}

	void Attach()
	{
		LOG_INFO("[AntiDebug] Searching for anti debug thread...");

		HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnapshot != INVALID_HANDLE_VALUE)
		{
			THREADENTRY32 tEntry = { 0 };
			tEntry.dwSize = sizeof(THREADENTRY32);

			// Iterate threads
			for (BOOL success = Thread32First(hThreadSnapshot, &tEntry);
				success == TRUE;
				success = Thread32Next(hThreadSnapshot, &tEntry))
			{
				if (tEntry.th32OwnerProcessID != g_sProcess->pid())
					continue;

				if (tEntry.tpBasePri != 6)
					continue;

				g_sProcess->threads().get(tEntry.th32ThreadID)->Terminate();
				LOG_INFO("[AntiDebug] Killed Thread: " << std::hex << tEntry.th32ThreadID);
			}

			CloseHandle(hThreadSnapshot);
		}
	}
};
