#pragma once

#include "gdxutil.h"
#include <algorithm>

namespace gdx {
    struct GXFREQUENCY {
        unsigned int Denominator;
        unsigned int Numerator;
        unsigned int Frequency;
    };

    struct GXDISPLAYMODE {
        unsigned int Width;
        unsigned int Height;
        std::vector<GXFREQUENCY> Frequencies;
    };

    struct GXOUTPUT {
        DXGI_OUTPUT_DESC OutputDesc;
        std::vector<GXDISPLAYMODE> DisplayModes;
    };

    struct GXADAPTER {
        DXGI_ADAPTER_DESC Desc;
        std::vector<GXOUTPUT> Outputs;
    };

    struct GXINTERFACE 
    {
    public:
        std::vector<GXADAPTER> Adapters;

    private:
        DXGI_FORMAT format;                 // DXGI-Format
        unsigned int m_maxFrequency;
        unsigned int m_maxNumerator;
        unsigned int m_maxDenominator;
        bool hardwareGPU;                   

        void GetNumeratorDenominator(unsigned int adapterIndex, unsigned int monitorIndex, unsigned int width, unsigned int height)
        {
            m_maxFrequency = 0;
            m_maxNumerator = 0;
            m_maxDenominator = 0;
            if (monitorIndex < GetCountOutput(adapterIndex)) {
                const auto& output = this->Adapters[adapterIndex].Outputs[monitorIndex];
                for (const auto& mode : output.DisplayModes) {
                    if (mode.Width == width && mode.Height == height) {
                        for (const auto& freq : mode.Frequencies) {
                            unsigned int currentFrequency = freq.Numerator / freq.Denominator;
                            if (currentFrequency > m_maxFrequency) {
                                m_maxFrequency = currentFrequency;
                                m_maxNumerator = freq.Numerator;
                                m_maxDenominator = freq.Denominator;
                            }
                        }
                    }
                }
            }
        }

    public:
        GXINTERFACE()
            : Adapters(),                // Empty constructor call for the vector
            format(DXGI_FORMAT_UNKNOWN), // Default initialization of DXGI_FORMAT (0)
            hardwareGPU(false),          // Initialization to false
            m_maxFrequency(0),
            m_maxNumerator(0),
            m_maxDenominator(0)
        {
            CleanupResources();
            SetDXGI_Format(DXGI_FORMAT_R8G8B8A8_UNORM);
        }

        ~GXINTERFACE(){ CleanupResources(); }

        void CleanupResources()
        {
            // Löschen der Frequenzen
            for (auto& adapter : this->Adapters)
            {
                for (auto& output : adapter.Outputs)
                {
                    for (auto& displayMode : output.DisplayModes)
                    {
                        displayMode.Frequencies.clear();
                    }
                    output.DisplayModes.clear();
                }
                adapter.Outputs.clear();
            }
            this->Adapters.clear();

            format = DXGI_FORMAT_UNKNOWN;
            hardwareGPU = false;
        }

        void AddAdapter(const GXADAPTER& adapter) 
        {
            this->Adapters.push_back(adapter);
        }

        bool GfxModeExists(int width, int height, int frequency, int adapterindex, int monitorindex)
        {
            const gdx::GXOUTPUT& output = this->Adapters[adapterindex].Outputs[monitorindex];
            for (const auto& displayMode : output.DisplayModes)
            {
                if (displayMode.Width == width && displayMode.Height == height)
                {
                    for (const auto& gfxFrequency : displayMode.Frequencies)
                    {
                        if (gfxFrequency.Frequency == frequency)
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        bool GfxModeExists(int width, int height, int adapterindex, int monitorindex)
        {
            const gdx::GXOUTPUT& output = this->Adapters[adapterindex].Outputs[monitorindex];
            for (const auto& displayMode : output.DisplayModes)
            {
                if (displayMode.Width == width && displayMode.Height == height)
                {
                    return true;
                }
            }
            return false;
        }
        
        size_t  GetCountOutput(int adapterindex)
        {
            return this->GetAdapter(adapterindex).Outputs.size();
        }
        
        size_t  GetCountDisplayModes(int adapterindex, int monitorindex)
        {
            return this->Adapters[adapterindex].Outputs[monitorindex].DisplayModes.size();
        }
        
        unsigned int GetGfxModeWidth(int modus, int adapterindex, int monitorindex)
        {
            return this->Adapters[adapterindex].Outputs[monitorindex].DisplayModes[modus].Width;
        }
        
        unsigned int GetGfxModeHeight(int modus, int adapterindex, int monitorindex)
        {
            return this->Adapters[adapterindex].Outputs[monitorindex].DisplayModes[modus].Height;
        }
        
        unsigned int GetGfxModeFrequency(unsigned int modus, int adapterindex, int monitorindex)
        {
            return this->Adapters[adapterindex].Outputs[monitorindex].DisplayModes[modus].Frequencies[0].Frequency;
        }
        
        std::string GetGfxDriverName(int adapterindex)
        {
            return GXUTIL::WideToUtf8(this->Adapters[adapterindex].Desc.Description);
        }
        
        const GXADAPTER& GetAdapter(size_t index) const
        {
            if (index < Adapters.size())
                return Adapters[index];
            else
                return Adapters[0];
        }
        
        size_t GetNumAdapters() const 
        {
            return Adapters.size();
        }
        
        bool GetHardwareGPU()
        {
            return hardwareGPU;
        }
        
        DXGI_FORMAT GetDXGI_Format()
        {
            return format;
        }

        void SetHardwareGPU(bool support)
        {
            hardwareGPU = support;
        }
                    
        void SetDXGI_Format(DXGI_FORMAT dxgi_format)
        {
            format = dxgi_format;
        }

        unsigned int GetNumerator(unsigned int adapterIndex, unsigned int monitorIndex, unsigned int width, unsigned int height)
        {
            GetNumeratorDenominator(adapterIndex, monitorIndex, width, height);
            return m_maxNumerator;
        }

        unsigned int GetDenominator(unsigned int adapterIndex, unsigned int monitorIndex, unsigned int width, unsigned int height)
        {
            GetNumeratorDenominator(adapterIndex, monitorIndex, width, height);
            return m_maxDenominator;
        }

        unsigned int GetMaxFrequnecy(unsigned int adapterIndex, unsigned int monitorIndex, unsigned int width, unsigned int height)
        {
            GetNumeratorDenominator(adapterIndex, monitorIndex, width, height);
            return m_maxFrequency;
        }

    };

    class CInterface {
    private:
        // Main system enumeration
        HRESULT GetSystemInfo();

        // Helper: Enumerate all outputs for an adapter
        HRESULT EnumerateAdapterOutputs(IDXGIAdapter* adapter, GXADAPTER& gxAdapter);

        // Helper: Enumerate all display modes for an output
        HRESULT EnumerateDisplayModes(IDXGIOutput* output, GXOUTPUT& gxOutput);

        // Member variables
        bool m_bInitialized;
        IDXGIFactory* m_factory;

    public:
        GXINTERFACE interfaceManager;        

    public:
        CInterface();
        ~CInterface();

        HRESULT Init(unsigned int bpp);
        void Release();

        IDXGIFactory* GetFactory() { return m_factory; }
    };
}