#include "player.h"

void CPlayer::FireWeapon()
{
	CRocket *newRocket = new CRocket;

	newRocket->pitch = camera->pitch;
	newRocket->direction = direction;
	newRocket->position = position;
	newRocket->forward = camera->lookAt.UnitVector();

	newRocket->SetAudioSystem(audioSys);
	newRocket->LoadAudio(audioSys, "explode.wav", false);
	newRocket->AttachTo(terrain);
}