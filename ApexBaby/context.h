#pragma once

#include "Config/Config.h"
#include "UI/Draw.h"
#include "Utils/Logging.h"
#include "Cheat/API/API.h"

class c_context
{
public:
	c_context() : m_logger(std::make_shared<c_logger>()), m_draw(std::make_shared<c_draw>()), m_destiny_api(std::make_shared<c_destiny_api>())
	{
	}

	~c_context()
	{
	}

	//Settings
	std::shared_ptr<c_logger> m_logger;

	/*Drawing*/
	std::shared_ptr<c_draw> m_draw;

	//Destiny API
	std::shared_ptr<c_destiny_api> m_destiny_api;

	/*Settings, normal settings*/
	c_settings m_settings;
};

extern c_context ctx;
