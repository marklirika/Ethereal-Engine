#pragma once
#include "ECS/ethereal_components.h"
#include "memory/ethereal_frame_info.h"

namespace ethereal {
	class AudioSystem {
	public:
		AudioSystem() = default;
		~AudioSystem();

		AudioSystem(const AudioSystem&) = delete;
		AudioSystem& operator=(const AudioSystem&) = delete;
		AudioSystem(const AudioSystem&&) = delete;
		AudioSystem& operator=(const AudioSystem&&) = delete;

		static void init();
		static void load(AudioComponent& auido, const std::string& filepath);
		static void play(AudioComponent& auido);
		static void update(FrameInfo& info);

	private:
		static FMOD::System* system;
	};

} // namespace ecs