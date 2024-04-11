#include "gdxinterface.h"

using namespace gdx;

CInterface::CInterface() : m_bInitialized(true), m_factory(nullptr)
{

}

CInterface::~CInterface()
{
    Release();
}

HRESULT CInterface::Init(unsigned int bpp)
{
    HRESULT hr = S_OK;

    interfaceManager.CleanupResources();
    interfaceManager.SetDXGI_Format(DXGI_FORMAT_R8G8B8A8_UNORM);

    // Create Factory
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_factory);
    if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
    {
        return hr;
    }

    // Fill structures
    return GetSystemInfo();
}

void CInterface::Release()
{
    if (m_bInitialized)
        Memory::SafeRelease(m_factory);

    m_bInitialized = false;
}

HRESULT CInterface::GetSystemInfo()
{
    HRESULT hr = S_OK;

    // Enumerate adapters
    IDXGIAdapter* adapter = nullptr;
    for (UINT i = 0; m_factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        hr = S_OK; // Reset HRESULT for each iteration

        // Exclude Windows renderer
        if (GXUTIL::isWindowsRenderer(adapter))
            continue;

        // Check if selected adapter has hardware support
        interfaceManager.SetHardwareGPU(GXUTIL::isHardwareSupported(adapter));

        GXADAPTER gxAdapter;
        // Get information about the adapter
        hr = adapter->GetDesc(&gxAdapter.Desc);
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        {
            Memory::SafeRelease(adapter); // Release adapter before exiting function
            return hr;
        }

        // Enumerate adapter's outputs
        IDXGIOutput* output = nullptr;
        for (UINT j = 0; adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND; ++j)
        {
            hr = S_OK; // Reset HRESULT for each output

            GXOUTPUT gxOutput;
            // Get information about the output
            hr = output->GetDesc(&gxOutput.OutputDesc);
            if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
            {
                Memory::SafeRelease(output); // Release output before exiting function
                Memory::SafeRelease(adapter); // Release adapter before exiting function
                return hr;
            }

            // Get display modes of the output
            std::vector<DXGI_MODE_DESC> displayModes;
            UINT numModes = 0;
            hr = output->GetDisplayModeList(interfaceManager.GetDXGI_Format(), 0, &numModes, nullptr);
            if (SUCCEEDED(hr) && numModes > 0) {
                displayModes.resize(numModes);
                hr = output->GetDisplayModeList(interfaceManager.GetDXGI_Format(), 0, &numModes, displayModes.data());
                if (SUCCEEDED(hr))
                {
                    // Iterate through display modes
                    for (const auto& mode : displayModes) {
                        GXDISPLAYMODE gxMode;
                        gxMode.Width = mode.Width;
                        gxMode.Height = mode.Height;

                        // Check if resolution and frequency already exist
                        bool found = false;
                        for (auto& displayMode : gxOutput.DisplayModes) {
                            if (displayMode.Width == gxMode.Width && displayMode.Height == gxMode.Height) {
                                for (auto& frequency : displayMode.Frequencies) {
                                    if (frequency.Denominator == mode.RefreshRate.Denominator &&
                                        frequency.Numerator == mode.RefreshRate.Numerator) {
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found) {
                                    GXFREQUENCY frequency = {};
                                    frequency.Denominator = mode.RefreshRate.Denominator;
                                    frequency.Numerator = mode.RefreshRate.Numerator;
                                    frequency.Frequency = gxMode.Width != 0 ? (mode.RefreshRate.Numerator / mode.RefreshRate.Denominator) : 0;
                                    displayMode.Frequencies.push_back(frequency);
                                }
                                break;
                            }
                        }
                        if (!found) {
                            GXFREQUENCY frequency = {};
                            frequency.Denominator = mode.RefreshRate.Denominator;
                            frequency.Numerator = mode.RefreshRate.Numerator;
                            frequency.Frequency = gxMode.Width != 0 ? (mode.RefreshRate.Numerator / mode.RefreshRate.Denominator) : 0;
                            gxMode.Frequencies.push_back(frequency);
                            gxOutput.DisplayModes.push_back(gxMode);
                        }
                    }
                }
                else
                {
                    Memory::SafeRelease(output); // Release output before exiting function
                    Memory::SafeRelease(adapter); // Release adapter before exiting function
                    return hr;
                }
            }

            // Add gxOutput to gxAdapter.Outputs
            gxAdapter.Outputs.push_back(gxOutput);

            // Release output
            Memory::SafeRelease(output);
        }

        // Add gxAdapter to systemInfo.Adapters
        interfaceManager.AddAdapter(gxAdapter);

        // Release adapter
        Memory::SafeRelease(adapter);
    }

    this->m_bInitialized = true;

    return hr;
}





