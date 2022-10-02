// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2021 Chris Roberts

#include "VolumeControl.h"

#include <MediaRoster.h>
#include <ParameterWeb.h>


VolumeControl::VolumeControl()
	:
	fInitStatus(B_NOT_INITIALIZED),
	fGainParameter(NULL),
	fMuteParameter(NULL)
{
	//Get global media roster
	BMediaRoster* roster = BMediaRoster::Roster();
	if (roster == NULL) {
		fInitStatus = kRosterError;
		return;
	}

	//Get system mixer
	media_node mixer;
	status_t status = roster->GetAudioMixer(&mixer);
	if (status != B_OK) {
		fInitStatus = kMixerError;
		return;
	}

	BParameterWeb* web;
	status = roster->GetParameterWebFor(mixer, &web);

	roster->ReleaseNode(mixer);

	if (status != B_OK) {
		fInitStatus = kParameterWebError;
		return;
	}

	BParameter* parameter;
	for (int32 index = 0; (parameter = web->ParameterAt(index)) != NULL; index++) {
		if (strcmp(parameter->Kind(), B_MASTER_GAIN) == 0) {
			fGainParameter = dynamic_cast<BContinuousParameter*>(parameter);
			break;
		}
	}

	if (fGainParameter == NULL) {
		fInitStatus = kGainError;
		return;
	}


	for (int32 index = 0; (parameter = web->ParameterAt(index)) != NULL; index++) {
		if (strcmp(parameter->Kind(), B_MUTE) == 0) {
			fMuteParameter = dynamic_cast<BDiscreteParameter*>(parameter);
			break;
		}
	}

	if (fMuteParameter == NULL) {
		fInitStatus = kMuteError;
		return;
	}

	fInitStatus = B_OK;
}


status_t
VolumeControl::InitCheck()
{
	return fInitStatus;
}


status_t
VolumeControl::GetVolume(float* volume)
{
	if (volume == NULL)
		return B_ERROR;

	size_t size = sizeof(*volume);
	if (fGainParameter->GetValue(volume, &size, NULL) != B_OK || isnan(*volume)) {
		*volume = -99.0;
		return B_ERROR;
	}

	return B_OK;
}


float
VolumeControl::GetMinVolume()
{
	return fGainParameter->MinValue();
}


float
VolumeControl::GetMaxVolume()
{
	return fGainParameter->MaxValue();
}


float
VolumeControl::GetStepSize()
{
	return fGainParameter->ValueStep();
}


bool
VolumeControl::IsMuted()
{
	int32 isMuted = 0;
	size_t size = sizeof(isMuted);

	fMuteParameter->GetValue(&isMuted, &size, NULL);

	return isMuted;
}


status_t
VolumeControl::AdjustVolume(float adjustment)
{
	float volume;
	if (GetVolume(&volume) != B_OK)
		return B_ERROR;

	return SetVolume(volume + adjustment);
}


status_t
VolumeControl::SetVolume(float volume)
{
	float minVolume = GetMinVolume();
	float maxVolume = GetMaxVolume();

	if (volume < minVolume)
		volume = minVolume;

	if (volume > maxVolume)
		volume = maxVolume;

	return fGainParameter->SetValue(&volume, sizeof(volume), 0);
}


status_t
VolumeControl::ToggleMute()
{
	int32 isMuted = 0;
	size_t size = sizeof(isMuted);

	fMuteParameter->GetValue(&isMuted, &size, NULL);

	isMuted = isMuted == 1 ? 0 : 1;

	return fMuteParameter->SetValue(&isMuted, size, 0);
}


status_t
VolumeControl::SetMute(bool muted)
{
	int32 isMuted = muted;
	return fMuteParameter->SetValue(&isMuted, sizeof(isMuted), 0);
}
