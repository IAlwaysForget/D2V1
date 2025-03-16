#pragma once
#include "GUI.h"
#include "Fuser/Fuser.h"

typedef enum box_type
{
	NORMAL,
	CORNER
};

typedef enum sp_color_type
{
	BOX,
	FILLED_BOX,
	HEAD,
	SKELETON,
	NAME,
	DISTANCE,
	FOV_CIRCLE,
	SNAPLINE,
	TEAM
};

typedef enum hardware_type
{
	KMBOX,
	PICOW
};

class c_draw : public GUI
{
public:
	c_draw() : GUI(fuser)
	{
	}

	~c_draw() = default;

	Fuser fuser;

	void Init(Fuser& fuse);
	void Run();

	void Visuals();
	void Character();
	void Inventory();
	void Teleport();
	void Perks();
	void Miscellaneous();

	void DrawString(std::string text, ImVec2 pos, ImColor color);
	void DrawRect(ImVec2 min, ImVec2 max, float thickness, ImColor color);
	void DrawFilledRect(ImVec2 min, ImVec2 max, ImColor color);
	void DrawLine(ImVec2 p1, ImVec2 p2, ImColor color, float thickness);
	void DrawCircle(ImVec2 center, float raduis, ImColor color, int segments, float thickness);
	void DrawFilledCircle(ImVec2 center, float raduis, ImColor color, int segments);
};
