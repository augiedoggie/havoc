// SPDX-License-Identifier: MIT
// Copyright (c) 2021 Chris Roberts

#ifndef _VOLUMECONTROL_H_
#define _VOLUMECONTROL_H_


#include <ParameterWeb.h>
#include <SupportDefs.h>


enum {
	kRosterError = B_ERRORS_END + 1,
	kMixerError,
	kParameterWebError,
	kGainError,
	kMuteError
};


class VolumeControl {

public:
	VolumeControl();

	status_t InitCheck();

	status_t AdjustVolume(float adjustment);
	status_t SetVolume(float volume);
	status_t ToggleMute();
	status_t SetMute(bool muted);

	status_t GetVolume(float* volume);
	float GetMinVolume();
	float GetMaxVolume();
	float GetStepSize();
	bool IsMuted();

private:
	BContinuousParameter* fGainParameter;
	BDiscreteParameter* fMuteParameter;
	status_t fInitStatus;
};

#endif	// _VOLUMECONTROL_H_
