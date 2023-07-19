#include "audio_system.h"

namespace ethereal {
	FMOD::System* AudioSystem::system = nullptr;

	AudioSystem::~AudioSystem() {
		if (system) {
			system->release();
			system->close();
			system = nullptr;
		}
	}

	void AudioSystem::init() {
		FMOD_RESULT result = FMOD::System_Create(&system);
		if (result != FMOD_OK) exit(-1);

		result = system->init(512, FMOD_INIT_NORMAL, 0);
		if (result != FMOD_OK) exit(-1);
	}

	void AudioSystem::load(AudioComponent& auido, const std::string& filepath) {
		system->createSound(filepath.c_str(), FMOD_DEFAULT, 0, &auido.sound);
	}

	void AudioSystem::play(AudioComponent& audio) {
		system->playSound(audio.sound, 0, false, &audio.channel);
	}

	void AudioSystem::update(FrameInfo& info) {
		auto view = info.scene.getRegistry().view<AudioComponent>();
		for (auto entity : view) {
			auto& audio = view.get<AudioComponent>(entity);
			if (audio.type == AudioComponent::AudioType::MUSIC_TRACK) {
				audio.channel->isPlaying(&audio.playing);
				if (!audio.playing) {
					play(audio);
				}
			}
		}

		system->update();
	}
}  // namespace ethereal
