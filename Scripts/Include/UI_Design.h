#pragma once
#include "Matrix3.h"
#include "Constants.h"

// Values of for UI elements
struct UIParameters
{
	Vec3 inputLightColor;
	Vec3 outputLightColor;
	float hybridThreshold;
	float minWavelength;
	float maxWavelength;
	int  noOfSpectralValues;
	float gaussianMinWidth;
	float gaussianMaxWidth;

	Vec3 previnputLightColor;
	float prevhybridThreshold;
	float prevminWavelength;
	float prevmaxWavelength;
	int  prevnoOfSpectralValues;
	float prevgaussianMinWidth;
	float prevgaussianMaxWidth;

	bool shouldRegenerateIridescentColours;
	float filmDensity;
	float airDensity;
	float filmIOR;
	float airIOR;
	float interferencePower;
	float interferenceConstant;
	int noOfButterflies;

	float prevfilmDensity;
	float prevairDensity;
	float prevfilmIOR;
	float prevairIOR;
	float previnterferencePower;
	float previnterferenceConstant;
	int prevnoOfButterflies;

	float scale;
	bool isMTLFileAvailable;
	bool useMTLFile;
	float spectralExponent;
	float ambientIntensity;
	float iridescenceIntensity;
	float transparency;
	bool iridescenceEnabled;
	bool specularEnabled;
	bool ambientEnabled;
	bool useNormalMap;
	bool useOpacityMap;
	Vec4 lightPosition;
	bool shouldUpdateNoOfButterflies;
	float peakWavelength;
	int IncidencAngleToVerify;
};

class UIDesign
{
public:
	static UIParameters uiParams;
	static void DrawUI();
};
