#include "UI_Design.h"
#include "ImGuiHelper.h"
#include "ImGUI\imgui.h"

// Value of GUI elements
UIParameters UIDesign::uiParams = {
Vec3{1.0,1.0,1.0}, // input Light Color;
Vec3{1.0,1.0,1.0}, // output Light Color;
0.7,			   // hybrid Threshold;
MIN_WAVELENGTH,	   // min Wavelength;
MAX_WAVELENGTH,	   // max Wavelength;
400.0,			   //  no Of Spectral Values;
5.0,			   // gaussian Min Width;
75.0,			   // gaussian Max Width;
				   //
Vec3{1.0,1.0,1.0}, // prev input Light Color;
0.7,			   // prev hybrid Threshold;
MIN_WAVELENGTH,	   // prev min Wavelength;
MAX_WAVELENGTH,	   // prev max Wavelength;
400.0,			   //  prev no Of Spectral Values;
5.0,			   // prev gaussian Min Width;
75.0,			   // prev gaussian Max Width;
				   //
true,			   // should RegenerateTexture;
90.0,			   // film Density;
90.0,			   // air Density;
1.56,			   // film IOR;
1.0,			   // air IOR;
4.75,		       // interference Power;
5,				   // interference Constant;
0,				   // no Of Butterflies;
90.0,			   // prev film Density;
90.0,			   // prev air Density;
1.56,			   // prev film IOR;
1.0,			   // prev air IOR;
4.75,			   // prev interference Power;
5,				   // prev interference Constant;
0,				   // prev no Of Butterflies;
0.75,			   // scale
false,			   // is MTL File Available;
false,			   // use MTL File;
28.0,			   // spectral Exponent;
0.3,			   // ambient Intensity;
1.5,			   // iridescence Intensity;
1.0,			   // transparency;
true,			   // iridescence Enabled;
true,			   // specular Enabled;
true,			   // ambient Enabled;
true,			   // use Normal Map;
true,			   // use Opacity Map;
false,			   // should Update No Of Butterflies;
0.0,			   // peak Wavelength;
0.0				   // Incidence Angle To Verify;
};

// Function to draw the GUI elements
void UIDesign::DrawUI()
{
	float lightColor[3] = {uiParams.inputLightColor.x,uiParams.inputLightColor.y,uiParams.inputLightColor.z };
	if (ImGui::CollapsingHeader("Spectral Config"))
	{
		ImGui::SliderFloat("Minimum Wavelength", &uiParams.minWavelength, MIN_WAVELENGTH, MAX_WAVELENGTH);
		ImGui::SliderFloat("Maximum Wavelength", &uiParams.maxWavelength,uiParams.minWavelength, MAX_WAVELENGTH);
		ImGui::SliderInt("No of Spectral Values", &uiParams.noOfSpectralValues, 50, 400);
		ImGui::SliderFloat("Hybrid Threshold", &uiParams.hybridThreshold, 0.0, 1.0);
		ImGui::SliderFloat("Minimum Gaussian Width", &uiParams.gaussianMinWidth, 1.0,uiParams.maxWavelength -uiParams.minWavelength);
		ImGui::SliderFloat("Maximum Gaussian Width", &uiParams.gaussianMaxWidth,uiParams.gaussianMinWidth,uiParams.maxWavelength -uiParams.minWavelength);
		ImGui::ColorEdit3("Light Color", lightColor);
		uiParams.inputLightColor.x = lightColor[0];
		uiParams.inputLightColor.y = lightColor[1];
		uiParams.inputLightColor.z = lightColor[2];
		if (uiParams.previnputLightColor.x !=uiParams.inputLightColor.x
			||uiParams.previnputLightColor.y !=uiParams.inputLightColor.y
			||uiParams.previnputLightColor.z !=uiParams.inputLightColor.z
			||uiParams.prevhybridThreshold !=uiParams.hybridThreshold
			||uiParams.prevminWavelength !=uiParams.minWavelength
			||uiParams.prevmaxWavelength !=uiParams.maxWavelength
			||uiParams.prevnoOfSpectralValues !=uiParams.noOfSpectralValues
			||uiParams.prevgaussianMaxWidth !=uiParams.gaussianMaxWidth
			||uiParams.prevgaussianMinWidth !=uiParams.gaussianMinWidth)
		{
			uiParams.shouldRegenerateIridescentColours = true;

			uiParams.previnputLightColor =uiParams.inputLightColor;
			uiParams.prevhybridThreshold =uiParams.hybridThreshold;
			uiParams.prevminWavelength =uiParams.minWavelength;
			uiParams.prevmaxWavelength =uiParams.maxWavelength;
			uiParams.prevnoOfSpectralValues =uiParams.noOfSpectralValues;
			uiParams.prevgaussianMaxWidth =uiParams.gaussianMaxWidth;
			uiParams.prevgaussianMinWidth =uiParams.gaussianMinWidth;
		}
	}
	if (ImGui::CollapsingHeader("Iridescent Config"))
	{
		float filmDensity =uiParams.filmDensity;
		float airDensity =uiParams.airDensity;
		float filmIOR =uiParams.filmIOR;
		float airIOR =uiParams.airIOR;
		float interferencePower =uiParams.interferencePower;
		float interferenceConstant =uiParams.interferenceConstant;
		float prevFilmDensity =uiParams.filmDensity;

		ImGui::SliderFloat("Film Density", &uiParams.filmDensity, 50.0, 500.0);
		ImGui::SliderFloat("Air Density", &uiParams.airDensity, 50.0, 500.0);
		ImGui::SliderFloat("Film Index of Refraction", &uiParams.filmIOR, 1.0, 3.0);
		ImGui::SliderFloat("Air Index of Refraction", &uiParams.airIOR, 1.0, 3.0);
		ImGui::SliderFloat("Interference Power", &uiParams.interferencePower, 1.0, 100.0);
		ImGui::SliderFloat("Interference Constant", &uiParams.interferenceConstant, 1.0, 20.0);

		if (uiParams.prevfilmDensity !=uiParams.filmDensity
			||uiParams.prevairDensity !=uiParams.airDensity
			||uiParams.prevfilmIOR !=uiParams.filmIOR
			||uiParams.prevairIOR !=uiParams.airIOR
			||uiParams.previnterferencePower !=uiParams.interferencePower
			||uiParams.previnterferenceConstant !=uiParams.interferenceConstant

			||uiParams.previnputLightColor.x !=uiParams.inputLightColor.x
			||uiParams.previnputLightColor.y !=uiParams.inputLightColor.y
			||uiParams.previnputLightColor.z !=uiParams.inputLightColor.z
			||uiParams.prevminWavelength !=uiParams.minWavelength
			||uiParams.prevmaxWavelength !=uiParams.maxWavelength
			||uiParams.prevnoOfSpectralValues !=uiParams.noOfSpectralValues
			||uiParams.prevgaussianMaxWidth !=uiParams.gaussianMaxWidth
			||uiParams.prevgaussianMinWidth !=uiParams.gaussianMinWidth)
		{
			uiParams.shouldRegenerateIridescentColours = true;
			uiParams.prevfilmDensity =uiParams.filmDensity;
			uiParams.prevairDensity =uiParams.airDensity;
			uiParams.prevfilmIOR =uiParams.filmIOR;
			uiParams.prevairIOR =uiParams.airIOR;
			uiParams.previnterferencePower =uiParams.interferencePower;
			uiParams.previnterferenceConstant =uiParams.interferenceConstant;

			uiParams.previnputLightColor =uiParams.inputLightColor;
			uiParams.prevminWavelength =uiParams.minWavelength;
			uiParams.prevmaxWavelength =uiParams.maxWavelength;
			uiParams.prevnoOfSpectralValues =uiParams.noOfSpectralValues;
			uiParams.prevgaussianMaxWidth =uiParams.gaussianMaxWidth;
			uiParams.prevgaussianMinWidth =uiParams.gaussianMinWidth;
		}
	}
	if (ImGui::CollapsingHeader("Light Properties"))
	{

		if (uiParams.isMTLFileAvailable)
		{
			ImGui::Checkbox("Use light properties from MTL File", &uiParams.useMTLFile);
		}
		if (!uiParams.useMTLFile)
		{
			ImGui::SliderFloat("Spectral Exponent", &uiParams.spectralExponent, 1.0, 300.0);
		}

		ImGui::SliderFloat("Ambient Intensity", &uiParams.ambientIntensity, 0.0, 1.0);
		ImGui::SliderFloat("Iridescence Intensity", &uiParams.iridescenceIntensity, 0.0, 5.0);
	}
	lightColor[0] =uiParams.outputLightColor.x;
	lightColor[1] =uiParams.outputLightColor.y;
	lightColor[2] =uiParams.outputLightColor.z;
	ImGui::SliderInt("", &uiParams.noOfButterflies, 0, 6);
	int noOfButterflies = pow(10,uiParams.noOfButterflies);
	ImGui::Text(" No of Butterflies = %d", noOfButterflies);
	if (ImGui::Button("Redraw Butterflies"))
	{
		uiParams.shouldUpdateNoOfButterflies = true;
		uiParams.prevnoOfButterflies =uiParams.noOfButterflies;
	}
	ImGui::ColorEdit3("Output Light Color", lightColor);
	ImGui::Checkbox("Ambient Enabled", &uiParams.ambientEnabled);
	ImGui::Checkbox("Iridescence Enabled", &uiParams.iridescenceEnabled);
	ImGui::Checkbox("Specular Enabled", &uiParams.specularEnabled);
	ImGui::Checkbox("Use Normal Map", &uiParams.useNormalMap);
	ImGui::Checkbox("Use Opacity Map", &uiParams.useOpacityMap);
	ImGui::SliderFloat("Scale", &uiParams.scale, 0.1, 50.0);
	ImGui::SliderFloat("Transparency", &uiParams.transparency, 0.1, 1.0);
	ImGui::SliderInt("Incidence Angle to verify", &uiParams.IncidencAngleToVerify, 0, 89);
	ImGui::Text("Peak Wavelength at %d = %f",
		uiParams.IncidencAngleToVerify,
		uiParams.peakWavelength);
	ImGui::Text("Spectral Error (RGB) %.3f %.3f %.3f",
		uiParams.inputLightColor.x -uiParams.outputLightColor.x,
		uiParams.inputLightColor.y -uiParams.outputLightColor.y,
		uiParams.inputLightColor.z -uiParams.outputLightColor.z
	);

	ImGui::Text("Light Position %.3f %.3f %.3f",
		uiParams.lightPosition.x,
		uiParams.lightPosition.y,
		uiParams.lightPosition.z
	);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}