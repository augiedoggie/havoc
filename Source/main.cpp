// SPDX-License-Identifier: MIT
// Copyright (c) 2021 Chris Roberts


#include "system.h"
#include "VolumeControl.h"

#include <iostream>


static const float kDefaultVolumeArg = -99999.0;

float gAdjustArg = 0.0;
float gVolumeArg = kDefaultVolumeArg;
int gToggleArg = 0;
int gMuteArg = 0;
int gUnMuteArg = 0;

const struct poptOption optionsTable[] = {
	{ "adjust",	'a', POPT_ARG_FLOAT,	&gAdjustArg,	0, "Increase/decrease volume by X",	"[1,-2.5,-4,9.5,...]" },
	{ "volume",	'v', POPT_ARG_FLOAT,	&gVolumeArg,	0, "Set absolute volume level",		"[-60,-20.5,0,18,...]" },
	{ "toggle",	't', POPT_ARG_NONE,		&gToggleArg,	0, "Toggle mute on/off",			NULL },
	{ "mute",	'm', POPT_ARG_NONE,		&gMuteArg,		0, "Set mute on",					NULL },
	{ "unmute",	'u', POPT_ARG_NONE,		&gUnMuteArg,	0, "Set mute off",					NULL },
	POPT_AUTOHELP
	POPT_TABLEEND
};


int main(int argc, char** argv)
{
	poptContext optionContext = poptGetContext("MediaKeyHelper", argc, (const char**)argv, optionsTable, 0);

	int rc = poptGetNextOpt(optionContext);
	if (rc < -1) {
		std::cout << poptBadOption(optionContext, 0) << " : " << poptStrerror(rc) << std::endl;
		poptPrintHelp(optionContext, stderr, 0);
		return 1;
	}

	poptFreeContext(optionContext);

	try {
		VolumeControl volume;

		if (argc == 1) {
			// no arguments were given, just print our current state

			// TODO investigate why using std::cout here sometimes causes crashes
			//std::cout << "Volume: " << volume.GetVolume() << " (min = " << volume.GetMinVolume() << ", max = " << volume.GetMaxVolume() << ", step = " << volume.GetStepSize() << ")" << std::endl;
			printf("Volume: %.1f (min=%.1f, max=%.1f, step=%.1f)\n", volume.GetVolume(), volume.GetMinVolume(), volume.GetMaxVolume(), volume.GetStepSize());

			std::cout << "State: " << (volume.IsMuted() ? "Muted" : "Not Muted") << std::endl;

			return 0;
		}

		if (gAdjustArg != 0) {
			std::cout << "Adjust volume: " << gAdjustArg << std::endl;
			volume.AdjustVolume(gAdjustArg);
			return 0;
		}

		// compare against kDefaultVolumeArg in case we get a request like --volume=0
		if (gVolumeArg != kDefaultVolumeArg) {
			std::cout << "Set volume: " << gVolumeArg << std::endl;
			volume.SetVolume(gVolumeArg);
			return 0;
		}

		if (gToggleArg != 0) {
			std::cout << (volume.IsMuted() ? "Un-Muting" : "Muting") << std::endl;
			volume.ToggleMute();
			return 0;
		}

		if (gMuteArg != 0) {
			std::cout << "Muting" << std::endl;
			volume.SetMute(true);
			return 0;
		}

		if (gUnMuteArg != 0) {
			std::cout << "Un-Muting" << std::endl;
			volume.SetMute(false);
			return 0;
		}

		poptPrintHelp(optionContext, stderr, 0);

	} catch(std::exception &e) {
		std::cout << std::endl << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 1;
}
