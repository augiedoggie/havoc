// SPDX-License-Identifier: MIT
// Copyright (c) 2021 Chris Roberts


#include "popt/system.h"
#include "VolumeControl.h"

#include <Application.h>
#include <Bitmap.h>
#include <IconUtils.h>
#include <Notification.h>
#include <Resources.h>

#include <iostream>


static const float kInitialArgVal = -99999.0;

float gAdjustArg = 0.0;
float gVolumeArg = kInitialArgVal;
int gToggleArg = 0;
int gMuteArg = 0;
int gUnMuteArg = 0;
float gNotifyArg = kInitialArgVal;

const struct poptOption optionsTable[] = {
	{ "adjust",	'a', POPT_ARG_FLOAT,	&gAdjustArg,	0, "Increase/decrease volume by X",	"[1,-2.5,-4,9.5,...]" },
	{ "volume",	'v', POPT_ARG_FLOAT,	&gVolumeArg,	0, "Set absolute volume level",		"[-60,-20.5,0,18,...]" },
	{ "toggle",	't', POPT_ARG_NONE,		&gToggleArg,	0, "Toggle mute on/off",			NULL },
	{ "mute",	'm', POPT_ARG_NONE,		&gMuteArg,		0, "Set mute on",					NULL },
	{ "unmute",	'u', POPT_ARG_NONE,		&gUnMuteArg,	0, "Set mute off",					NULL },
	{ "notify",	'n', POPT_ARG_FLOAT | POPT_ARGFLAG_OPTIONAL,	&gNotifyArg,	0, "Show system notification and specify optional timeout (default: 1.5 seconds)",	"0,1,1.5,3,..."},
	POPT_AUTOHELP
	POPT_TABLEEND
};


class MixerApp : public BApplication {
public:
	MixerApp()
		:	BApplication("application/x-vnd.cpr.VolumeControl"),
			fVolume(new VolumeControl()),
			fArgReceived(false),
			fNotificationTimeout(kInitialArgVal)
		{}

	virtual ~MixerApp() {
		delete fVolume;
	}

	virtual void ArgvReceived(int32 argc, char** argv) {

		fArgReceived = true;

		poptContext optionContext = poptGetContext("VolumeControl", argc, (const char**)argv, optionsTable, 0);

		int rc = poptGetNextOpt(optionContext);
		if (rc < -1) {
			std::cout << poptBadOption(optionContext, 0) << " : " << poptStrerror(rc) << std::endl;
			poptPrintHelp(optionContext, stderr, 0);
			return;
		}

		poptFreeContext(optionContext);

		if (gNotifyArg != kInitialArgVal) {
			if (gNotifyArg <= 0)
				fNotificationTimeout = 1.5; // use our default timeout
			else
				fNotificationTimeout = gNotifyArg;
		}

		if (gAdjustArg != 0) {
			std::cout << "Adjust volume: " << gAdjustArg << std::endl;
			fVolume->AdjustVolume(gAdjustArg);
			return;
		}

		// compare against kInitialArgVal in case we get a request like --volume=0
		if (gVolumeArg != kInitialArgVal) {
			std::cout << "Set volume: " << gVolumeArg << std::endl;
			fVolume->SetVolume(gVolumeArg);
			return;
		}

		if (gToggleArg != 0) {
			std::cout << (fVolume->IsMuted() ? "Un-Muting" : "Muting") << std::endl;
			fVolume->ToggleMute();
			return;
		}

		if (gMuteArg != 0) {
			std::cout << "Muting" << std::endl;
			fVolume->SetMute(true);
			return;
		}

		if (gUnMuteArg != 0) {
			std::cout << "Un-Muting" << std::endl;
			fVolume->SetMute(false);
			return;
		}

		if (fNotificationTimeout != kInitialArgVal)
			return;

		poptPrintHelp(optionContext, stderr, 0);

	}

	virtual void ReadyToRun() {
		float vol = fVolume->GetVolume();
		if (vol == 0)
			vol = 0.0; // avoid a floating point -0

		if (!fArgReceived)
			// no arguments were given, just print our current state
			printf("Volume: %g (min=%g, max=%g, step=%g)\nState: %s\n",
				vol, fVolume->GetMinVolume(), fVolume->GetMaxVolume(), fVolume->GetStepSize(),
				fVolume->IsMuted() ? "Muted" : "Not Muted");

		if (fNotificationTimeout > 0) {
			BNotification* notification;
			BString content;
			if (fVolume->IsMuted()) {
				notification = new BNotification(B_INFORMATION_NOTIFICATION);
				BBitmap* bitmap = _LoadResourceBitmap("SpeakerMute", 32);
				if (bitmap != NULL) {
					notification->SetIcon(bitmap);
					delete bitmap;
				}
				content = "Audio Muted";
			} else {
				notification = new BNotification(B_PROGRESS_NOTIFICATION);
				notification->SetProgress((vol - fVolume->GetMinVolume()) / (fVolume->GetMaxVolume() - fVolume->GetMinVolume()));
				content.SetToFormat("Volume: %g dB", vol);
			}
			notification->SetContent(content);
			notification->SetGroup("VolumeControl");
			notification->SetTitle("System Volume");
			notification->SetMessageID("volume_control_status");
			notification->Send(fNotificationTimeout * 1000 * 1000);
			delete notification;
		}

		be_app->PostMessage(B_QUIT_REQUESTED);
	}


private:
	VolumeControl* fVolume;
	bool fArgReceived;
	float fNotificationTimeout;


	BBitmap* _LoadResourceBitmap(const char* name, int32 size) {
		BResources* resources = be_app->AppResources();
		if (resources == NULL)
			return NULL;

		size_t dataSize;
		const void* data = resources->LoadResource(B_VECTOR_ICON_TYPE, name, &dataSize);
		if (data == NULL)
			return NULL;

		BBitmap* bitmap = new BBitmap(BRect(0, 0, size - 1, size - 1), B_RGBA32);
		if (bitmap == NULL)
			return NULL;

		if (bitmap->InitCheck() != B_OK) {
			delete bitmap;
			return NULL;
		}

		if (BIconUtils::GetVectorIcon(static_cast<const uint8*>(data), dataSize, bitmap) != B_OK) {
			delete bitmap;
			return NULL;
		}

		return bitmap;
	}
};


int main(int argc, char** argv)
{
	try {
		MixerApp app;
		app.Run();
	} catch(std::exception &e) {
		std::cout << std::endl << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
