// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts


#include "popt/system.h"
#include "VolumeControl.h"

#include <Application.h>
#include <Bitmap.h>
#include <IconUtils.h>
#include <Notification.h>
#include <Resources.h>

#include <iostream>


#ifdef HAIKU_ENABLE_I18N
#include <Catalog.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "VolumeControl"
#else
#define B_TRANSLATE(x) x
#endif


static const float kInitialArgVal = -99999.0;

float gAdjustArg = 0.0;
float gVolumeArg = kInitialArgVal;
int gToggleArg = 0;
int gMuteArg = 0;
int gUnMuteArg = 0;
float gNotifyArg = kInitialArgVal;

const struct poptOption optionsTable[] = {
	{ "adjust",	'a', POPT_ARG_FLOAT,	&gAdjustArg,	0, "Increase/decrease volume by X dB",	"[1,-2.5,-4,9.5,...]" },
	{ "volume",	'v', POPT_ARG_FLOAT,	&gVolumeArg,	0, "Set absolute volume dB level",		"[-60,-20.5,0,18,...]" },
	{ "toggle",	't', POPT_ARG_NONE,		&gToggleArg,	0, "Toggle mute on/off",				NULL },
	{ "mute",	'm', POPT_ARG_NONE,		&gMuteArg,		0, "Set mute on",						NULL },
	{ "unmute",	'u', POPT_ARG_NONE,		&gUnMuteArg,	0, "Set mute off",						NULL },
	{ "notify",	'n', POPT_ARG_FLOAT | POPT_ARGFLAG_OPTIONAL,	&gNotifyArg,	0, "Show system notification and specify optional timeout (default: 1.5 seconds)",	"1,1.5,3,..."},
	POPT_AUTOHELP
	POPT_TABLEEND
};


class MixerApp : public BApplication {
public:
	MixerApp()
		:
		BApplication("application/x-vnd.cpr.VolumeControl"),
		fArgReceived(false),
		fNotificationTimeout(kInitialArgVal),
		fVolume(new VolumeControl())
		{}


	virtual ~MixerApp() {
		delete fVolume;
	}


	virtual void ArgvReceived(int32 argc, char** argv) {

		poptContext optionContext = poptGetContext("VolumeControl", argc, (const char**)argv, optionsTable, 0);

		int rc = poptGetNextOpt(optionContext);
		if (rc < -1) {
			std::cout << poptBadOption(optionContext, 0) << " : " << poptStrerror(rc) << std::endl;
			poptPrintHelp(optionContext, stderr, 0);
			poptFreeContext(optionContext);
			Quit();
			return;
		}

		const char* extraArg = poptPeekArg(optionContext);
		if (extraArg != NULL) {
			std::cout << extraArg << " : unknown extra argument" << std::endl;
			poptPrintHelp(optionContext, stderr, 0);
			poptFreeContext(optionContext);
			Quit();
			return;
		}

		// compare against kInitialArgVal because '-n' will actually be '-n=0'
		if (gNotifyArg != kInitialArgVal) {
			if (gNotifyArg <= 0)
				fNotificationTimeout = 1.5; // use our default timeout
			else
				fNotificationTimeout = gNotifyArg;
		}

		if (fVolume->InitCheck() != B_OK) {
			poptFreeContext(optionContext);
			return;
		}

		// don't allow multiple mute operations at the same time
		if (gToggleArg != 0) {
			std::cout << (fVolume->IsMuted() ? "Unmuting" : "Muting") << std::endl;
			fVolume->ToggleMute();
			fArgReceived = true;
		} else if (gMuteArg != 0) {
			std::cout << "Muting" << std::endl;
			fVolume->SetMute(true);
			fArgReceived = true;
		} else if (gUnMuteArg != 0) {
			std::cout << "Unmuting" << std::endl;
			fVolume->SetMute(false);
			fArgReceived = true;
		}

		// don't allow multiple volume operations at the same time
		if (gAdjustArg != 0) {
			std::cout << "Adjust volume: " << gAdjustArg << std::endl;
			fVolume->AdjustVolume(gAdjustArg);
			fArgReceived = true;
		} else if (gVolumeArg != kInitialArgVal) { // compare against kInitialArgVal in case we get a request like --volume=0
			std::cout << "Set volume: " << gVolumeArg << std::endl;
			fVolume->SetVolume(gVolumeArg);
			fArgReceived = true;
		}

		poptFreeContext(optionContext);
	}


	virtual void ReadyToRun() {
		float volume = kInitialArgVal;
		BString outputString;

		if (fVolume->InitCheck() == B_OK) {
			if (fVolume->GetVolume(&volume) != B_OK)
				outputString = B_TRANSLATE("Error getting volume from mixer!");
		} else
			outputString = B_TRANSLATE("Error initializing mixer control!");

		if (volume == 0)
			volume = 0.0; // avoid a floating point -0

		if (volume != kInitialArgVal && !fArgReceived)
			// no arguments were given, just list our current state
			outputString.SetToFormat("Gain: %g dB (min=%g, max=%g, step=%g)\nMute: %s",
				volume, fVolume->GetMinVolume(), fVolume->GetMaxVolume(), fVolume->GetStepSize(),
				fVolume->IsMuted() ? "On" : "Off");

		if (outputString.Length() > 0)
			std::cout << outputString << std::endl;

		if (fNotificationTimeout > 0) {
			BNotification* notification;
			if (volume == kInitialArgVal)
				notification = new BNotification(B_ERROR_NOTIFICATION);
			else if (fVolume->IsMuted()) {
				notification = new BNotification(B_INFORMATION_NOTIFICATION);
				BBitmap* bitmap = _LoadResourceBitmap("SpeakerMute", 32);
				if (bitmap != NULL) {
					notification->SetIcon(bitmap);
					delete bitmap;
				}
				outputString = B_TRANSLATE("Audio Muted");
			} else {
				notification = new BNotification(B_PROGRESS_NOTIFICATION);
				notification->SetProgress((volume - fVolume->GetMinVolume()) / (fVolume->GetMaxVolume() - fVolume->GetMinVolume()));
				outputString.SetToFormat(B_TRANSLATE("Gain: %g dB"), volume);
			}

			notification->SetTitle(outputString);
			notification->SetGroup(B_TRANSLATE("System Volume"));
			notification->SetMessageID("volume_control_status");
			notification->Send(fNotificationTimeout * 1000 * 1000);
			delete notification;
		}

		Quit();
	}


private:
	bool			fArgReceived;
	float			fNotificationTimeout;
	VolumeControl*	fVolume;


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


int
main(int /*argc*/, char** /*argv*/)
{
	MixerApp app;
	app.Run();

	return 0;
}
