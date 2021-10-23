#include "Application.h"


// Function to create fetch iridescent colours from the spectra
void Application::FetchIridescentColoursFromSpectra() {

	VkDeviceSize imageSize = spectralParameters.textureWidth*spectralParameters.textureHeight;

	void* mappedMemory = NULL;
	// Map the buffer memory, so that we can read from it on the CPU.
	vkMapMemory(device->logicalDevice, ComputeBuffers->memory, 0, ComputeBufferSize, 0, &mappedMemory);

	Pixel* pmappedMemory = (Pixel *)mappedMemory;

	std::vector<double> spectra;

	Vec3 xyz;
	Vec3 rgb;
	Vec3 outColor;
	int peakWavelengthIndex = 0;
	float peakWavelength = 0;
	peakWavelengths.clear();

	// Loop through the spectrum
	for (int i = 0; i < imageSize; i++) {
		double value = pmappedMemory[i].r;
		spectra.push_back(value);
		int j = i / (int)spectralParameters.textureWidth;

		if (peakWavelengthIndex == 0 || spectra[peakWavelengthIndex] < spectra[i % (int)spectralParameters.textureWidth])
		{
			peakWavelengthIndex = i % (int)spectralParameters.textureWidth;
			peakWavelength = pmappedMemory[i].b;
		}
		if (i % (int)spectralParameters.textureWidth == spectralParameters.textureWidth - 1)
		{
			// Convert spectra to XYZ
			xyz = GetXYZColorFromSpectra(spectra);

			// Convert XYZ to RGB colour
			rgb = ConvertXYZtoRGB(xyz);

			// Set the iridescent colour in this incident angle
			iridescentColors.colors[j].x = rgb.x;
			iridescentColors.colors[j].y = rgb.y;
			iridescentColors.colors[j].z = rgb.z;
			iridescentColors.colors[j].w = 1.0;

			// Fetch the peak wavelength
			peakWavelengths.push_back(peakWavelength);
			peakWavelength = 0;
			peakWavelengthIndex = 0;
			spectra.clear();
		}
	}
	spectra.clear();

	// Fetch the colour from the original spectra
	for (int i = 0; i < spectralParameters.noOfSpectralValues; i++) {
		double value = pmappedMemory[i].g;
		spectra.push_back(value);
	}
	xyz = GetXYZColorFromSpectra(spectra);
	rgb = ConvertXYZtoRGB(xyz);
	UIDesign::uiParams.outputLightColor = rgb;
	ball.lightingConstants.lightAmbient = Vec4{ UIDesign::uiParams.inputLightColor.x, UIDesign::uiParams.inputLightColor.y, UIDesign::uiParams.inputLightColor.z, 1.0 };
	spectra.clear();

	// Done reading, so unmap.
	vkUnmapMemory(device->logicalDevice, ComputeBuffers->memory);

}

// Function to find the fourier matrix
void Application::FindFourierMatrix()
{
	double minWavelength = MIN_WAVELENGTH;
	double maxWavelength = MAX_WAVELENGTH;
	double diffWavelength = maxWavelength - minWavelength;
	int n = 400;
	double del = (maxWavelength - minWavelength) / n;
	double k = 1;
	std::vector<double> vals;
	for (int i = 0; i < 9; i++)
	{
		vals.clear();
		for (int j = 0; j < n; j++)
		{
			double wavelength = minWavelength + j;
			double x, b;
			double angle = 2 * 3.14 * ((wavelength - minWavelength) / diffWavelength);
			Vec3 colorMatchFunc = ColorMatchingFunctionMap.at(wavelength);
			if (i / 3 == 0)
			{
				x = colorMatchFunc.x;
			}
			else if (i / 3 == 1)
			{
				x = colorMatchFunc.y;
			}
			else if (i / 3 == 2)
			{
				x = colorMatchFunc.z;
			}

			if (i % 3 == 0)
			{
				b = 1;
			}
			else if (i % 3 == 1)
			{
				b = cos(angle);
			}
			else if (i % 3 == 2)
			{
				b = sin(angle);
			}
			double val = x * b;
			vals.push_back(val);
		}
		double integralValue = MathHelper::IntegrateValues(vals);
		FourierMatrix.values[i / 3][i % 3] = k * integralValue;
	}
}

// Function to create texture image
void Application::ReadColorMatchingXMLFile(const char* filename) {

	int texWidth, texHeight, texChannels;

	std::ifstream inputFile;
	inputFile.open(filename);
	if (!inputFile.is_open())
	{
		throw std::runtime_error("failed to open color matching xml!");
	}

	std::string tempString = "";

	//Read the header
	int tempInt = 0;


	while (tempString != "<Record>")
	{
		inputFile.ignore(200, '\n');
		inputFile >> tempString;
	}

	while (tempString != "</Records>")
	{
		double key = 0.0;
		float x, y, z;
		inputFile >> tempString;
		int startIndex = tempString.find('>');
		int lastIndex = tempString.find_last_of('<');
		std::string valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		key = std::stod(valString);

		inputFile >> tempString;
		startIndex = tempString.find('>');
		lastIndex = tempString.find_last_of('<');
		valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		x = std::stod(valString);

		inputFile >> tempString;
		startIndex = tempString.find('>');
		lastIndex = tempString.find_last_of('<');
		valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		y = std::stod(valString);

		inputFile >> tempString;
		startIndex = tempString.find('>');
		lastIndex = tempString.find_last_of('<');
		valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		z = std::stod(valString);

		Vec3 ColorMatchValue = { x,y,z };
		ColorMatchingFunctionMap.emplace(key, ColorMatchValue);
		inputFile >> tempString;
		inputFile >> tempString;
	}

	inputFile.close();
}

// Function to create texture image
void Application::ReadChromaticityXMLFile(const char* filename) {

	int texWidth, texHeight, texChannels;
	Vec3 White = { 0.31271 ,0.32902 ,0.35827 };
	Vec3 VioletVec = { 0.71819 ,0.28181 ,0.0 };
	std::ifstream inputFile;
	inputFile.open(filename);
	if (!inputFile.is_open())
	{
		throw std::runtime_error("failed to open chromaticity xml file!");
	}

	std::string tempString = "";

	//Read the header
	int tempInt = 0;


	while (tempString != "<Record>")
	{
		inputFile.ignore(200, '\n');
		inputFile >> tempString;
	}

	while (tempString != "</Records>")
	{
		std::string key = "";
		float x, y, z;
		double wavelength;
		inputFile >> tempString;
		int startIndex = tempString.find('>');
		int lastIndex = tempString.find_last_of('<');
		std::string valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		wavelength = std::stod(valString);

		inputFile >> tempString;
		startIndex = tempString.find('>');
		lastIndex = tempString.find_last_of('<');
		valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		x = std::stod(valString);

		inputFile >> tempString;
		startIndex = tempString.find('>');
		lastIndex = tempString.find_last_of('<');
		valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		y = std::stod(valString);

		inputFile >> tempString;
		startIndex = tempString.find('>');
		lastIndex = tempString.find_last_of('<');
		valString = tempString.substr(startIndex + 1, lastIndex - startIndex - 1);
		z = std::stod(valString);

		Vec3 Coord = { x,y,z };
		double hue = getHueFromXYZ(Coord);
		key = std::to_string((double)hue);
		key = key.substr(0, key.find('.') + 3);

		HueAngleWavelengthMap.emplace(key, wavelength);
		HueAngleCoordMap.emplace(key, Coord);
		inputFile >> tempString;
		inputFile >> tempString;
	}

	inputFile.close();
}

// Function to convert RGB to XYZ colour
Vec3 Application::ConvertRGBtoXYZ(Vec3 colorInRGB)
{
	Vec3 XYZ;
	Matrix3 M;
	M.values[0][0] = 0.412453;
	M.values[0][1] = 0.357580;
	M.values[0][2] = 0.180423;
	M.values[1][0] = 0.212671;
	M.values[1][1] = 0.715160;
	M.values[1][2] = 0.072169;
	M.values[2][0] = 0.019334;
	M.values[2][1] = 0.119193;
	M.values[2][2] = 0.950227;
	XYZ = M * colorInRGB;

	return XYZ;
}

// Function to convert XYZ to RGB colour
Vec3 Application::ConvertXYZtoRGB(Vec3 colorInXYZ)
{
	Vec3 RGB;
	Matrix3 M;
	M.values[0][0] = 0.4124564;
	M.values[0][1] = 0.3575761;
	M.values[0][2] = 0.1804375;
	M.values[1][0] = 0.2126729;
	M.values[1][1] = 0.7151522;
	M.values[1][2] = 0.0721750;
	M.values[2][0] = 0.0193339;
	M.values[2][1] = 0.1191920;
	M.values[2][2] = 0.9503041;
	RGB = M.Inverse() * colorInXYZ;
	RGB.x = RGB.x < 0.0 ? 0.0 : RGB.x;
	RGB.y = RGB.y < 0.0 ? 0.0 : RGB.y;
	RGB.z = RGB.z < 0.0 ? 0.0 : RGB.z;
	RGB.x = RGB.x > 1.0 ? 1.0 : RGB.x;
	RGB.y = RGB.y > 1.0 ? 1.0 : RGB.y;
	RGB.z = RGB.z > 1.0 ? 1.0 : RGB.z;

	return RGB;
}

// Function to find fourier coefficients
void Application::FindFourierCoefficients(Vec3 colorInXYZ)
{
	Matrix3 AInverse = FourierMatrix.Inverse();
	FourierCoeffs = AInverse * colorInXYZ;
}

// Function to convert Spectra to XYZ colour
Vec3 Application::GetXYZColorFromSpectra(std::vector<double> spectra)
{
	double minWavelength = UIDesign::uiParams.minWavelength;
	double maxWavelength = UIDesign::uiParams.maxWavelength;
	double diffWavelength = maxWavelength - minWavelength;
	int n = UIDesign::uiParams.noOfSpectralValues;
	double del = (maxWavelength - minWavelength) / n;
	double k = 1;
	std::vector<double> xvals, yvals, zvals;
	for (int j = 0; j < n; j++)
	{
		double wavelength = minWavelength + j * del;
		Vec3 colorMatchValue;
		if (ColorMatchingFunctionMap.count(wavelength) > 0)
			colorMatchValue = ColorMatchingFunctionMap.at(wavelength);
		else
			colorMatchValue = ColorMatchingFunctionMap.lower_bound(wavelength)->second;
		double val = spectra[j] * colorMatchValue.x;
		xvals.push_back(val);
		val = spectra[j] * colorMatchValue.y;
		yvals.push_back(val);
		val = spectra[j] * colorMatchValue.z;
		zvals.push_back(val);
	}
	double x = k * MathHelper::IntegrateValues(xvals);
	double y = k * MathHelper::IntegrateValues(yvals);
	double z = k * MathHelper::IntegrateValues(zvals);
	double sum = x + y + z;

	return { (float)x,(float)y,(float)z };
}

// Function to update spectral parameters
void Application::UpdateSpectralParameters()
{
	Vec3 colorInXyz = ConvertRGBtoXYZ(UIDesign::uiParams.inputLightColor);

	FindFourierCoefficients(colorInXyz);

	Vec3 hsv;
	hsv.x = colorInXyz.x / (colorInXyz.x + colorInXyz.y + colorInXyz.z);
	hsv.y = colorInXyz.y / (colorInXyz.x + colorInXyz.y + colorInXyz.z);
	hsv.z = colorInXyz.z / (colorInXyz.x + colorInXyz.y + colorInXyz.z);
	hsv = CalculateSaturationAndDominantWavelength(hsv);
	hsv.y = hsv.y > 1.0 ? 1.0 : hsv.y;
	std::string key = std::to_string((double)hsv.x);
	key = key.substr(0, key.find('.') + 3);
	double dominantWavelength;
	if (HueAngleWavelengthMap.count(key) <= 0)
	{
		key = std::to_string((int)hsv.x);
		dominantWavelength = HueAngleWavelengthMap.lower_bound(key)->second;
	}
	else
		dominantWavelength = HueAngleWavelengthMap.at(key);


	spectralParameters.textureHeight = 90.0;
	spectralParameters.textureWidth = UIDesign::uiParams.noOfSpectralValues;
	spectralParameters.maxSpectralValue = 1.0;
	spectralParameters.FourierCoefficients = FourierCoeffs;
	spectralParameters.dominantWavelength = dominantWavelength;
	spectralParameters.gaussianMaxWidth = UIDesign::uiParams.gaussianMaxWidth;
	spectralParameters.gaussianMinWidth = UIDesign::uiParams.gaussianMinWidth;
	spectralParameters.hybridThreshold = UIDesign::uiParams.hybridThreshold;
	spectralParameters.maxWavelength = UIDesign::uiParams.maxWavelength;
	spectralParameters.minWavelength = UIDesign::uiParams.minWavelength;
	spectralParameters.noOfSpectralValues = UIDesign::uiParams.noOfSpectralValues;
	spectralParameters.saturation = hsv.y;
	spectralParameters.filmDensity = UIDesign::uiParams.filmDensity;
	spectralParameters.airDensity = UIDesign::uiParams.airDensity;
	spectralParameters.filmIOR = UIDesign::uiParams.filmIOR;
	spectralParameters.airIOR = UIDesign::uiParams.airIOR;
	spectralParameters.interferencePower = UIDesign::uiParams.interferencePower;
	spectralParameters.interferenceConstant = UIDesign::uiParams.interferenceConstant;

	SpectralParametersBuffer->SetData(device, &spectralParameters, sizeof(spectralParameters));
}

// Function to calculate saturation and dominant wavelength
Vec3 Application::CalculateSaturationAndDominantWavelength(Vec3 colorInXYZ)
{
	Vec3 hsv;
	Vec3 White = { 0.31271 ,0.32902 ,0.35827 };
	Vec3 VioletVec = { 0.16638 ,0.01830 ,0.81532 };
	Vec3 dominantWavelengthCoord;
	Vec3 vec1 = White - colorInXYZ;
	Vec3 vec2 = White - VioletVec;
	if (vec1.Magnitude() == 0)
		hsv.x = 0;
	else
		hsv.x = vec1.Angle(&vec2);
	std::string key = std::to_string((double)hsv.x);
	key = key.substr(0, key.find('.') + 3);

	if (HueAngleCoordMap.count(key) <= 0)
	{
		key = std::to_string((int)hsv.x);
		dominantWavelengthCoord = HueAngleCoordMap.lower_bound(key)->second;
	}
	else
		dominantWavelengthCoord = HueAngleCoordMap.at(key);

	Vec3 vec3 = White - dominantWavelengthCoord;
	hsv.y = vec1.Magnitude() / vec3.Magnitude();
	hsv.z = 0.0;
	return hsv;
}

// Function to get hue to XYZ colour
double Application::getHueFromXYZ(Vec3 colorInXYZ)
{
	Vec3 White = { 0.31271 ,0.32902 ,0.35827 };
	Vec3 VioletVec = { 0.16638 ,0.01830 ,0.81532 };

	Vec3 vec1 = White - colorInXYZ;
	Vec3 vec2 = White - VioletVec;

	return vec1.Angle(&vec2);
}