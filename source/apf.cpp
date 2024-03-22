#include "apf.h"
#include <corecrt_math_defines.h>
#include <cmath>
#include <pluginterfaces/vst/vsttypes.h>

AllPassFilter::AllPassFilter()
{
}

AllPassFilter::~AllPassFilter()
{
}


Steinberg::Vst::Sample32 AllPassFilter::processSample(Steinberg::Vst::Sample32 input, Steinberg::int32 channelNumber)
{
	if (channelNumber > 1) //ignore all channels except Left and Right
	{
		float result = 0.0f;
		return result;
	}
	//testing if 2nd order is what im looking for
	float bandwith = getModulatedCornerFreq() / steepness;

	float alpha = (tan(bandwith * M_PI / sampleRate) - 1) / (tan(bandwith * M_PI / sampleRate) + 1);
	float beta = -1.0f * cos(2.0f * M_PI * getModulatedCornerFreq() / sampleRate);

	float a0 = -1.0f * alpha;
	float a1 = beta * (1 - alpha);
	float a2 = 1;
	float b1 = a1;
	float b2 = -1.0f * alpha;

	float& x1 = bufferedX1[channelNumber];
	float& x2 = bufferedX2[channelNumber];
	float& y1 = bufferedY1[channelNumber];
	float& y2 = bufferedY2[channelNumber];

	float output = a0 * input + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;

	//move buffers
	x2 = x1;
	x1 = input;
	y2 = y1;
	y1 = output;

	return output;
}


double AllPassFilter::getModulatedCornerFreq()
{
	double nyquistFreq = sampleRate / 2;
	double range = nyquistFreq / 2;
	double modulated = cornerFrequency + (range * oscillatorValue);

	if (modulated >= (nyquistFreq * steepness))
	{
		modulated = (nyquistFreq * steepness) - 10;
	}
	if (modulated < minFrequency)
	{
		modulated = minFrequency;
	}
	return modulated;
}

float AllPassFilter::calculateCoefficient()
{
	double modulatedCornerFreq = getModulatedCornerFreq();

	float tangent = tan(M_PI * (cornerFrequency / sampleRate));

	float coefficient = (tangent - 1) / (tangent + 1);
	return coefficient;
}
