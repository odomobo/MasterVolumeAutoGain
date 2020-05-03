
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#include <atomic>
#include <cmath>

#include <WindowsVolumeUpdater.hpp>

WindowsVolumeUpdater::WindowsVolumeUpdater(std::atomic<bool>& mutedHandle, std::atomic<float>& decibelHandle)
  : _mutedHandle{ mutedHandle }, _decibelHandle{ decibelHandle }
{
  CoInitialize(NULL);
  IMMDeviceEnumerator* deviceEnumerator = NULL;
  CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)& deviceEnumerator);
  IMMDevice* defaultDevice = NULL;
  deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
  deviceEnumerator->Release();
  deviceEnumerator = NULL;

  defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)& endpointVolume);
  defaultDevice->Release();
  defaultDevice = NULL;

  BOOL mute;
  endpointVolume->GetMute(&mute);

  float decibels;
  endpointVolume->GetMasterVolumeLevel(&decibels);

  _mutedHandle = mute;
  _decibelHandle = decibels;

  endpointVolume->RegisterControlChangeNotify(this);
}

WindowsVolumeUpdater::~WindowsVolumeUpdater()
{
  endpointVolume->UnregisterControlChangeNotify(this);
  endpointVolume->Release();
  CoUninitialize();
}