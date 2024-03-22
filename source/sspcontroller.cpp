//------------------------------------------------------------------------
// Copyright(c) 2024 HonoursProject.
//------------------------------------------------------------------------

#include "sspcontroller.h"
#include "sspcids.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "params.h"

using namespace Steinberg;

namespace HonoursProject {

//------------------------------------------------------------------------
// SingleStagePhaserController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Here you could register some parameters
	parameters.addParameter(STR16("Mix"), nullptr, 0, 1.0f, Vst::ParameterInfo::kCanAutomate, mixId);
	parameters.addParameter(STR16("LFO Frequency"), nullptr, 0, 0.0f, Vst::ParameterInfo::kCanAutomate, lfoFreqId);
	parameters.addParameter(STR16("LFO Depth"), nullptr, 0, 0.0f, Vst::ParameterInfo::kCanAutomate, lfoAmpId);
	parameters.addParameter(STR16("APF Corner Frequency"), nullptr, 0, 0.0f, Vst::ParameterInfo::kCanAutomate, cornerFreqId);

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserController::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleStagePhaserController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API SingleStagePhaserController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "sspeditor.uidesc");
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
} // namespace HonoursProject
