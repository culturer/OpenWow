#include "stdafx.h"

// General
#include "MDX_Part_Transparency.h"

void MDX_Part_Transparency::init(File& f, M2TextureWeight& mtd, uint32_t* global)
{
	trans.init(mtd.weight, f, global);
}