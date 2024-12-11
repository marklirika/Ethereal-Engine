#include "utils.h"

void eHelp::timer::start() {
	start_point = std::chrono::steady_clock::now();
}

float eHelp::timer::dt() {
	end_point = std::chrono::steady_clock::now();
	return delta_time = std::chrono::duration<float, std::chrono::seconds::period>(end_point - start_point).count();
}

void eHelp::timer::reset() {
	start_point = end_point;
}