#pragma once

#include <windows.h>
#include <endpointvolume.h>

#include <atomic>


struct WindowsVolumeUpdater : public IAudioEndpointVolumeCallback
{
  std::atomic<bool>& _mutedHandle;
  std::atomic<float>& _decibelHandle;
  IAudioEndpointVolume* endpointVolume;

public:
  WindowsVolumeUpdater(std::atomic<bool> & mutedHandle, std::atomic<float> & decibelHandle);
  ~WindowsVolumeUpdater();

  HRESULT OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) override
  {
    _mutedHandle = pNotify->bMuted;
    float decibels;
    endpointVolume->GetMasterVolumeLevel(&decibels);
    _decibelHandle = decibels;
    return S_OK;
  }

  ULONG AddRef() override
  {
    return 0;
  }

  HRESULT QueryInterface(REFIID riid, void** ppvObject)
  {
    return E_NOINTERFACE;
  }

  ULONG Release() override
  {
    return 0;
  }
};