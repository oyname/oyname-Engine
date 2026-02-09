#include "Shader.h"

Shader::Shader() :
    isActive(false),
    flagsVertex(0),
    inputlayoutVertex(nullptr),
    vertexShader(nullptr),
    pixelShader(nullptr),
    blobVS(nullptr),
    blobPS(nullptr)
{
    // materials ist jetzt ein vector - keine Initialisierung nötig
}

Shader::~Shader() {
    // Gib alle COM-Objekte frei
    Memory::SafeRelease(inputlayoutVertex);
    Memory::SafeRelease(vertexShader);
    Memory::SafeRelease(pixelShader);
    Memory::SafeRelease(blobVS);
    Memory::SafeRelease(blobPS);

    // materials Vector wird automatisch aufgeräumt
    // Die Material-Objekte selbst werden vom ObjectManager verwaltet, nicht hier löschen!
    materials.clear();
}

void Shader::UpdateShader(const gdx::CDevice* device)
{
    // Fehlerbehandlung: Prüfe auf nullptr
    if (device == nullptr) {
        Debug::Log("ERROR: Shader::UpdateShader - device is nullptr");
        return;
    }
    ID3D11DeviceContext* context = device->GetDeviceContext();
    if (context == nullptr) {
        Debug::Log("ERROR: Shader::UpdateShader - device context is nullptr");
        return;
    }

    // Prüfe ob Shader vollständig initialisiert
    if (!IsValid()) {
        Debug::Log("WARNING: Shader::UpdateShader - Shader not fully initialized");
        Debug::Log("  inputlayoutVertex: ", (inputlayoutVertex != nullptr ? "OK" : "MISSING"));
        Debug::Log("  vertexShader: ", (vertexShader != nullptr ? "OK" : "MISSING"));
        Debug::Log("  pixelShader: ", (pixelShader != nullptr ? "OK" : "MISSING"));
        return;
    }

    // Setze Input Layout
    context->IASetInputLayout(inputlayoutVertex);
    // Setze Vertex Shader
    context->VSSetShader(vertexShader, nullptr, 0);
    // Setze Pixel Shader
    context->PSSetShader(pixelShader, nullptr, 0);
    // Markiere als aktiv
    isActive = true;
}
