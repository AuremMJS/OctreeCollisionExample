///////////////
// Main.cpp //
/////////////
#include "Application.h"

// Main function
int main() {

	// Instance to Vulkan Application
	Application app;

	try {
		// Run the application to initialize and run the Vulkan objects
		app.run();
	}
	catch (const std::exception& e) {

		// Print the exception description to command prompt
		std::cerr << e.what() << std::endl;

		// return EXIT_FAILURE as the program has failed
		return EXIT_FAILURE;
	}

	// return EXIT_SUCCESS as the program has run successfully
	return EXIT_SUCCESS;
}