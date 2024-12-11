#pragma once
#include <chrono>
#include <functional>
namespace eHelp {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hash_combine(seed, rest), ...);
	};

	struct timer {
		void start();
		void reset();
		float dt();

		std::chrono::steady_clock::time_point start_point{};
		std::chrono::steady_clock::time_point end_point{};
		float delta_time{ 0.f };
	};
}
