#include "MasterVolumeAutoGain.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "WindowsVolumeUpdater.hpp"

MasterVolumeAutoGain::MasterVolumeAutoGain(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPrograms)), _windowsVolumeUpdater{_muted, _decibel}
{
  //GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50).GetVShifted(-12), "Automatically adjusts gain based", IText(25)));
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50).GetVShifted(12), "on Windows master volume.", IText(25)));
    //pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));
  };
#endif
}


float decibelsToGain(float decibels)
{
  return pow(10, (decibels / 20));
}

#if IPLUG_DSP
void MasterVolumeAutoGain::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  //const double gain = GetParam(kGain)->Value() / 100.;
  const float gain = _muted ? 0 : decibelsToGain(_decibel);

  const int nChans = NOutChansConnected();

  for (int c = 0; c < nChans; c++) {
    for (int s = 0; s < nFrames; s++) {
      outputs[c][s] = inputs[c][s] * gain;
    }
  }
}
#endif

