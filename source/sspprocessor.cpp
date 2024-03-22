//------------------------------------------------------------------------
// Copyright(c) 2024 HonoursProject.
//------------------------------------------------------------------------

#include "sspprocessor.h"
#include "sspcids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include "params.h"
#include "lfo.h"
#include "apf.h"

using namespace Steinberg;

namespace HonoursProject {
//------------------------------------------------------------------------
// SingleStagePhaserProcessor
//------------------------------------------------------------------------
SingleStagePhaserProcessor::SingleStagePhaserProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kSingleStagePhaserControllerUID);
}

//------------------------------------------------------------------------
SingleStagePhaserProcessor::~SingleStagePhaserProcessor ()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
	addEventInput (STR16 ("Event In"), 1);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::process (Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			if (auto* paramQueue = data.inputParameterChanges->getParameterData(index))
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount();
				if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
				{
					switch (paramQueue->getParameterId())
					{
					case mixId:
						mix = (float)value;
						break;
					case lfoFreqId:
						lfo.setOscillationFrequency((float)value);
						break;
					case lfoAmpId:
						lfo.setOscillationAmplitude((float)value);
						break;
					case cornerFreqId:
						apf.setCornerFrequency((float)value);
						break;
					}
				}
			}
		}
	}
	
	//--- Here you have to implement your processing
	if (data.numSamples == 0 || data.numOutputs == 0)
		return kResultTrue;

	int32 numOfChannels = data.inputs[0].numChannels;
	Vst::Sample32** in = data.inputs[0].channelBuffers32;
	Vst::Sample32** out = data.outputs[0].channelBuffers32;

	float blend = mix * 0.5f;

	for (int32 ch = 0; ch < numOfChannels; ++ch)
	{
		Vst::Sample32* ptrIn = in[ch];
		Vst::Sample32* ptrOut = out[ch];
		Vst::Sample32 temp;
		for (int32 i = 0; i < data.numSamples; ++i)
		{
			//get new oscilaltor value for channel ch
			float newOscVal = lfo.next(ch);
			apf.setNewOscillatorValue(newOscVal);

			//run the sample through the APF
			temp = apf.processSample(*ptrIn, ch);

			//mix with original input based on mix percentage;
			temp = (temp * blend) + *ptrIn * (1 - blend);

			*ptrOut = temp;

			ptrIn++;
			ptrOut++;
		}
	}

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace HonoursProject
