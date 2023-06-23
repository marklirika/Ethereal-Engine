#include "core/application.h"
#include "utility/open_simplex_noise.h"

//std 
#include <cstdlib>
#include <stdexcept>
#include <iostream>

int main() {

	ethereal::Application app{};	
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}