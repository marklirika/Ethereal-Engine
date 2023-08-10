#include "utils.h"

void eHelp::timer::start() {
	start_point = std::chrono::high_resolution_clock::now();
}

float eHelp::timer::dt() {
	end_point = std::chrono::high_resolution_clock::now();
	return delta_time = std::chrono::duration<float, std::chrono::seconds::period>(start_point - end_point).count();
}

void eHelp::timer::reset() {
	start_point = end_point;
}
