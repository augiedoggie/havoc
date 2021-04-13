// SPDX-License-Identifier: MIT
// Copyright (c) 2021 Chris Roberts

#ifndef _VOLUMECONTROL_H_
#define _VOLUMECONTROL_H_


#include <ParameterWeb.h>
#include <SupportDefs.h>


class VolumeControl {

public:
	VolumeControl();

	status_t AdjustVolume(float adjustment);
	status_t SetVolume(float volume);
	status_t ToggleMute();
	status_t SetMute(bool muted);

	float GetVolume();
	float GetMinVolume();
	float GetMaxVolume();
	float GetStepSize();
	bool IsMuted();

private:
	BContinuousParameter* fGainParameter;
	BDiscreteParameter* fMuteParameter;
};

#endif	// _VOLUMECONTROL_H_
