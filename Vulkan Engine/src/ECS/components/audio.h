#pragma once
#include "fmod.hpp"

namespace ethereal {
	struct AudioComponent {
        enum class AudioType {
            SOUND_TRACK,
            MUSIC_TRACK
        };

    ~AudioComponent() {
        if (channel) {
            channel->stop();
            channel = nullptr;
        }

        if (sound) {
            sound->release();
            sound = nullptr;
        }
    }

	bool playing = false;
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* channel = nullptr;
    AudioType type;
};

} // namespace ethreal