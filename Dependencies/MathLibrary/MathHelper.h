#pragma once
#include <vector>

// Class for Math Helper function
class MathHelper
{
public:
	// Constructor and destructor
	MathHelper();
	~MathHelper();

	// Function to integrate discrete values using trapezoidal rule
	static double IntegrateValues(std::vector<double> values);
};

