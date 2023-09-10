#pragma once
#include "RendererBase.h"

using namespace NCL::Rendering;

class RendererFactory
{
	virtual RendererBase* GetNewRenderer() = 0;
};

