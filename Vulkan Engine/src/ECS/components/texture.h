#pragma once
#include "render/ethereal_texture.h"
#include <memory>

namespace ethereal {
	
	struct TextureComponent {
		std::shared_ptr<EtherealTexture> texture;
	};
}