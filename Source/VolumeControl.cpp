// SPDX-License-Identifier: MIT
// Copyright (c) 2021 Chris Roberts

#include "VolumeControl.h"

#include <MediaRoster.h>
#include <ParameterWeb.h>

#include <stdexcept>


VolumeControl::VolumeControl()
	:
	fGainParameter(NULL),
	fMuteParameter(NULL)
{
	//Get global media roster
	BMediaRoster* roster = BMediaRoster::Roster();
	if (roster == NULL)
		throw std::runtime_error("Unable to get media roster!");

	//Get system mixer
	media_node mixer;
	status_t status = roster->GetAudioMixer(&mixer);
	if (status != B_OK)
		throw std::runtime_error("Can't get audio mixer!");

	BParameterWeb* web;
	status = roster->GetParameterWebFor(mixer, &web);

	roster->ReleaseNode(mixer);

	if (status != B_OK)
		throw std::runtime_error("Can't get mixer parameter web!");


	BParameter* parameter;
	for (int32 index = 0; (parameter = web->ParameterAt(index)) != NULL; index++) {
		if (strcmp(parameter->Kind(), B_MASTER_GAIN) == 0) {
			fGainParameter = dynamic_cast<BContinuousParameter*>(parameter);
			break;
		}
	}

	if (fGainParameter == NULL)
		throw std::runtime_error("Couldn't find master gain control!");


	for (int32 index = 0; (parameter = web->ParameterAt(index)) != NULL; index++) {
		if (strcmp(parameter->Kind(), B_MUTE) == 0) {
			fMuteParameter = dynamic_cast<BDiscreteParameter*>(parameter);
			break;
		}
	}

	if (fMuteParameter == NULL)
		throw std::runtime_error("Couldn't find mute control!");

}


float VolumeControl::GetVolume()
{
	float currentVolume = 0.0;
	size_t size = sizeof(&currentVolume);
	if (fGainParameter->GetValue(&currentVolume, &size, NULL) != B_OK || isnan(currentVolume))
		throw std::runtime_error("Couldn't get current volume");

	return currentVolume;
}


float VolumeControl::GetMinVolume()
{
	return fGainParameter->MinValue();
}


float VolumeControl::GetMaxVolume()
{
	return fGainParameter->MaxValue();
}


float VolumeControl::GetStepSize()
{
	return fGainParameter->ValueStep();
}


bool VolumeControl::IsMuted()
{
	int32 isMuted = 0;
	size_t size = sizeof(isMuted);

	fMuteParameter->GetValue(&isMuted, &size, NULL);

	return isMuted;
}


status_t VolumeControl::AdjustVolume(float adjustment)
{
	return SetVolume(GetVolume() + adjustment);
}


status_t VolumeControl::SetVolume(float volume)
{
	float minVolume = GetMinVolume();
	float maxVolume = GetMaxVolume();

	if (volume < minVolume)
		volume = minVolume;

	if (volume > maxVolume)
		volume = maxVolume;

	return fGainParameter->SetValue(&volume, sizeof(volume), 0);
}


status_t VolumeControl::ToggleMute()
{
	int32 isMuted = 0;
	size_t size = sizeof(isMuted);

	fMuteParameter->GetValue(&isMuted, &size, NULL);

	isMuted = isMuted == 1 ? 0 : 1;

	return fMuteParameter->SetValue(&isMuted, size, 0);
}


status_t VolumeControl::SetMute(bool muted)
{
	int32 isMuted = muted;
	return fMuteParameter->SetValue(&isMuted, sizeof(isMuted), 0);
}
