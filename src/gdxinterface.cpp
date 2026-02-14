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
    Release();

    HRESULT hr = S_OK;

    interfaceManager.CleanupResources();
    interfaceManager.SetDXGI_Format(DXGI_FORMAT_R8G8B8A8_UNORM);

    // Create Factory
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_factory);
    if (FAILED(hr))
    {
        Debug::LogHr(__FILE__, __LINE__, hr);
        return hr;   // wenn Funktion HRESULT zurückgibt
    }

    // Enumerate system information
    hr = GetSystemInfo();

    if (SUCCEEDED(hr))
    {
        m_bInitialized = true;
    }

    return hr;
}

void CInterface::Release()
{
    if (m_bInitialized)
        Memory::SafeRelease(m_factory);

    m_bInitialized = false;
}

HRESULT CInterface::GetSystemInfo()
{
    if (!m_factory)
        return E_FAIL;

    HRESULT hr = S_OK;
    IDXGIAdapter* adapter = nullptr;

    // Enumerate all graphics adapters
    for (UINT i = 0; m_factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        if (!adapter)
            continue;

        // Skip Windows software renderer
        if (GXUTIL::isWindowsRenderer(adapter))
        {
            Memory::SafeRelease(adapter);
            continue;
        }

        // Check hardware support
        bool hardwareSupported = GXUTIL::isHardwareSupported(adapter);
        interfaceManager.SetHardwareGPU(hardwareSupported);

        GXADAPTER gxAdapter;

        // Get adapter description
        hr = adapter->GetDesc(&gxAdapter.Desc);
        if (FAILED(hr))
        {
            Debug::LogHr(__FILE__, __LINE__, hr);
            Memory::SafeRelease(adapter);
            return hr;
        }

        // Enumerate outputs for this adapter
        hr = EnumerateAdapterOutputs(adapter, gxAdapter);
        if (FAILED(hr))
        {
            Debug::LogHr(__FILE__, __LINE__, hr);
            Memory::SafeRelease(adapter);
            return hr;
        }

        // Add configured adapter to interface manager
        interfaceManager.AddAdapter(gxAdapter);

        Memory::SafeRelease(adapter);
    }

    return hr;
}

HRESULT CInterface::EnumerateAdapterOutputs(IDXGIAdapter* adapter, GXADAPTER& gxAdapter)
{
    if (!adapter)
        return E_INVALIDARG;

    HRESULT hr = S_OK;
    IDXGIOutput* output = nullptr;

    for (UINT j = 0; adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND; ++j)
    {
        if (!output)
            continue;

        GXOUTPUT gxOutput;

        // Get output description
        hr = output->GetDesc(&gxOutput.OutputDesc);
        if (FAILED(hr))
        {
            Debug::LogHr(__FILE__, __LINE__, hr);
            Memory::SafeRelease(output);
            return hr;
        }

        // Enumerate display modes for this output
        hr = EnumerateDisplayModes(output, gxOutput);
        if (FAILED(hr))
        {
            Memory::SafeRelease(output);
            return hr;
        }

        // Add configured output to adapter
        gxAdapter.Outputs.push_back(gxOutput);

        Memory::SafeRelease(output);
    }

    return hr;
}

HRESULT CInterface::EnumerateDisplayModes(IDXGIOutput* output, GXOUTPUT& gxOutput)
{
    if (!output)
        return E_INVALIDARG;

    HRESULT hr = S_OK;
    DXGI_FORMAT format = interfaceManager.GetDXGI_Format();

    // First call: get number of display modes
    UINT numModes = 0;
    hr = output->GetDisplayModeList(format, 0, &numModes, nullptr);

    if (FAILED(hr) || numModes == 0)
    {
        return hr;
    }

    // Second call: get actual display mode data
    std::vector<DXGI_MODE_DESC> displayModes(numModes);
    hr = output->GetDisplayModeList(format, 0, &numModes, displayModes.data());

    if (FAILED(hr))
    {
        return hr;
    }

    // Process display modes and eliminate duplicates
    for (const auto& mode : displayModes)
    {
        GXDISPLAYMODE gxMode;
        gxMode.Width = mode.Width;
        gxMode.Height = mode.Height;

        // Find existing resolution in output
        auto resolutionIt = std::find_if(
            gxOutput.DisplayModes.begin(),
            gxOutput.DisplayModes.end(),
            [&gxMode](const GXDISPLAYMODE& dm) {
                return dm.Width == gxMode.Width && dm.Height == gxMode.Height;
            }
        );

        // Calculate frequency (float division, not integer)
        float frequency = mode.RefreshRate.Denominator != 0
            ? static_cast<float>(mode.RefreshRate.Numerator) / mode.RefreshRate.Denominator
            : 0.0f;

        GXFREQUENCY gxFrequency;
        gxFrequency.Denominator = mode.RefreshRate.Denominator;
        gxFrequency.Numerator = mode.RefreshRate.Numerator;
        gxFrequency.Frequency = frequency;

        if (resolutionIt != gxOutput.DisplayModes.end())
        {
            // Resolution exists, check if frequency is duplicate
            bool frequencyExists = std::any_of(
                resolutionIt->Frequencies.begin(),
                resolutionIt->Frequencies.end(),
                [&gxFrequency](const GXFREQUENCY& freq) {
                    return freq.Numerator == gxFrequency.Numerator &&
                        freq.Denominator == gxFrequency.Denominator;
                }
            );

            // Only add if frequency doesn't already exist
            if (!frequencyExists)
            {
                resolutionIt->Frequencies.push_back(gxFrequency);
            }
        }
        else
        {
            // New resolution, create new entry
            gxMode.Frequencies.push_back(gxFrequency);
            gxOutput.DisplayModes.push_back(gxMode);
        }
    }

    return hr;
}



