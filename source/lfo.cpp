#include "lfo.h"
#include <corecrt_math_defines.h>
#include <cmath>

LowFrequencyOscillator::LowFrequencyOscillator()
{
}

LowFrequencyOscillator::~LowFrequencyOscillator()
{
}

float LowFrequencyOscillator::next(int channelNumber)
{
	float asRadian = static_cast<float>(counterPerChannel[channelNumber]) * 2 * M_PI;
	float sinValue = sin(asRadian);
	updateCounter(channelNumber);
	return sinValue * oscillationAmplitude;
}

void LowFrequencyOscillator::resetCounter()
{
	counterPerChannel[0] = 0.0f;
	counterPerChannel[1] = 0.0f;
}

void LowFrequencyOscillator::updateCounter(int channelNumber)
{
	float increment = static_cast<float>(oscillationFrequency / sampleRate);
	float counter = counterPerChannel[channelNumber];
	counterPerChannel[channelNumber] = fmod((counter + increment), 1.0f);
};