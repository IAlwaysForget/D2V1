
#include "context.h"
#include "Cheat/API/API.h"
#include "Cheat/Cache/Cache.h"
#include "Cheat/Structs/math.h"
#include <DMALibrary/Memory/Memory.h>
#include "Movement.h"
namespace Features::Movement {
	Vector GetBackwardVector(const VMatrix& viewMatrix)
	{
		//Thank you mister from 2004 
		//TLDR transpose matrix, get 2nd row and you get backward vector unless you invert
		VMatrix transposedViewMatrix = viewMatrix;

		transposedViewMatrix.transpose();
		Vector forwardVector(transposedViewMatrix[2][0], transposedViewMatrix[2][1], transposedViewMatrix[2][2]);
		forwardVector.Normalize();

		return forwardVector;
	}

	Vector noclip_pos = Vector(0, 0, 0);
	void HandleNoclip(Entity local) {
		auto backward = GetBackwardVector(g_Cache.GetMatrix());

		bool down = /*false*/true;

		if (noclip_pos.DistTo(local.pos) > ctx.m_settings.movement_noclip_fast_speed() || noclip_pos.DistTo(local.pos) > ctx.m_settings.movement_noclip_normal_speed()) {
			noclip_pos = local.pos;
		}

		auto multiplier = ctx.m_settings.movement_noclip_normal_speed();
		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) { //Shift
			multiplier = ctx.m_settings.movement_noclip_fast_speed();
		}


		if (GetAsyncKeyState(0x57) & 0x8000) //W
		{
			noclip_pos -= (backward / 4) * multiplier;
			down = true;
		}

		if (GetAsyncKeyState(0x53) & 0x8000) //S
		{
			noclip_pos += (backward / 4) * multiplier;
			down = true;
		}

		if (GetAsyncKeyState(0x41) & 0x8000) //A
		{
			Vector upward(0.0f, 0.0f, 1.0f);
			noclip_pos += (backward.cross(upward) / 4) * multiplier;
			down = true;
		}

		if (GetAsyncKeyState(0x44) & 0x8000) //D
		{
			Vector upward(0.0f, 0.0f, 1.0f);
			noclip_pos -= (backward.cross(upward) / 4) * multiplier;
			down = true;
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) //Space
		{
			noclip_pos.z += 1.f * multiplier;
			down = true;
		}
		if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) //Shift
		{
			noclip_pos.z -= 1.f * multiplier;
			down = true;
		}
		if (down)
		{
			auto handle = mem.CreateScatterHandle();
			if (ctx.m_settings.movement_noclip_style() == 1) { //Noclip
				mem.AddScatterWriteRequest(handle, local.entity_addr + 0x1a0, &noclip_pos, sizeof(noclip_pos));
				mem.AddScatterWriteRequest(handle, local.entity_addr + 0x1b0, &noclip_pos, sizeof(noclip_pos));
				mem.AddScatterWriteRequest(handle, local.entity_addr + 0x1c0, &noclip_pos, sizeof(noclip_pos));
				mem.AddScatterWriteRequest(handle, local.entity_addr + 0x250, &noclip_pos, sizeof(noclip_pos));
				mem.AddScatterWriteRequest(handle, local.entity_addr + 0x260, &noclip_pos, sizeof(noclip_pos));

				//Velocity
				Vector vel = Vector(0, 0, 0);

				mem.AddScatterWriteRequest(handle, local.entity_addr + 0x230, &vel, sizeof(vel));
			}
			else {//Fly
				//Velocity
				Vector vel = ((Vector(0, 0, 0) - backward) * 6) * multiplier;

				mem.AddScatterWriteRequest(handle, local.entity_addr + 0x230, &vel, sizeof(vel));
			}

			mem.ExecuteWriteScatter(handle);
			mem.CloseScatterHandle(handle);
		}
		else {
			noclip_pos = local.pos;
		}
	}
}