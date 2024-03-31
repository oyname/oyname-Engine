#include "gdxinterface.h"

using namespace gdx;

CInterface::CInterface(): m_bInitialized(true), m_factory(nullptr)
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

    // Strukturen befüllen
    return GetSystemInfo();
}

void CInterface::Release()
{
    if(m_bInitialized)
        Memory::SafeRelease(m_factory);
    
    m_bInitialized = false;
}

HRESULT CInterface::GetSystemInfo()
{
    HRESULT hr = S_OK;

    // Enumeriere die Adapter
    IDXGIAdapter* adapter = nullptr;
    for (UINT i = 0; m_factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        hr = S_OK; // HRESULT für jede Iteration zurücksetzen

        // Keinen Windows Renderer
        if (Common::isWindowsRenderer(adapter))
            continue;

        // Prüfen ob Eingestellter Adapter Hardwareunterstützung hat
        interfaceManager.SetHardwareGPU(Common::isHardwareSupported(adapter));

        GXADAPTER gxAdapter;
        // Informationen über den Adapter abrufen
        hr = adapter->GetDesc(&gxAdapter.Desc);
        if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
        {
            Memory::SafeRelease(adapter); // Adapter freigeben, bevor die Funktion verlassen wird
            return hr;
        }

        // Enumeriere die Ausgänge des Adapters
        IDXGIOutput* output = nullptr;
        for (UINT j = 0; adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND; ++j)
        {
            hr = S_OK; // HRESULT für jeden Ausgang zurücksetzen

            GXOUTPUT gxOutput;
            // Informationen über den Ausgang abrufen
            hr = output->GetDesc(&gxOutput.OutputDesc);
            if (FAILED(Debug::GetErrorMessage(__FILE__, __LINE__, hr)))
            {
                Memory::SafeRelease(output); // Ausgabe freigeben, bevor die Funktion verlassen wird
                Memory::SafeRelease(adapter); // Adapter freigeben, bevor die Funktion verlassen wird
                return hr;
            }

            // Anzeigemodi des Ausgangs abrufen
            std::vector<DXGI_MODE_DESC> displayModes;
            UINT numModes = 0;
            hr = output->GetDisplayModeList(interfaceManager.GetDXGI_Format(), 0, &numModes, nullptr);
            if (SUCCEEDED(hr) && numModes > 0) {
                displayModes.resize(numModes);
                hr = output->GetDisplayModeList(interfaceManager.GetDXGI_Format(), 0, &numModes, displayModes.data());
                if (SUCCEEDED(hr))
                {
                    // Durchlaufe die Anzeigemodi
                    for (const auto& mode : displayModes) {
                        GXDISPLAYMODE gxMode;
                        gxMode.Width = mode.Width;
                        gxMode.Height = mode.Height;

                        // Überprüfe, ob die Auflösung und Frequenz bereits vorhanden sind
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
                    Memory::SafeRelease(output); // Ausgabe freigeben, bevor die Funktion verlassen wird
                    Memory::SafeRelease(adapter); // Adapter freigeben, bevor die Funktion verlassen wird
                    return hr;
                }
            }

            // gxOutput zu gxAdapter.Outputs hinzufügen
            gxAdapter.Outputs.push_back(gxOutput);

            // Ausgabe freigeben
            Memory::SafeRelease(output);
        }

        // gxAdapter zu systemInfo.Adapters hinzufügen
        interfaceManager.AddAdapter(gxAdapter);

        // Adapter freigeben
        Memory::SafeRelease(adapter);
    }

    this->m_bInitialized = true;

    return hr;
}




