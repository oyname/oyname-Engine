# OYNAME Engine – Coding Rules

> Stand: Februar 2026

---

## 1. Namespaces

Es gibt zwei klar getrennte Namespaces:

| Namespace | Zweck | Beispiele |
|-----------|-------|-----------|
| `Engine::` | Öffentliche API für Spielcode | `CreateMesh()`, `PositionEntity()`, `UpdateWorld()` |
| `Core::` | Bootstrap und Lifecycle | Fenster, Pfade, Konfiguration, Timing |

Interne Engine-Klassen (`GDXEngine`, `GDXDevice`, `GDXInterface`) brauchen keinen eigenen Namespace – das `GDX`-Präfix macht sie bereits eindeutig.

**Regeln:**

- Spielcode ruft **nur** Funktionen aus dem `Engine`-Namespace auf. Direkter Zugriff auf interne Engine-Klassen aus Spielcode ist verboten.
- `Core::` kennt kein `ID3D11Device` und kein `IDXGISwapChain`.
- **Niemals** `using namespace DirectX` in einen Header schreiben.

```cpp
// FALSCH – in einem Header
using namespace DirectX;
XMVECTOR position;

// RICHTIG – in einem Header
DirectX::XMVECTOR position;
```

In `.cpp`-Dateien ist `using namespace DirectX;` am Anfang der Datei erlaubt.

---

## 2. Klassen- und Typnamen

| Kategorie | Konvention | Beispiele |
|-----------|-----------|-----------|
| Interne Engine-Klassen | `GDX`-Präfix, PascalCase | `GDXEngine`, `GDXDevice`, `GDXInterface` |
| Domain-Objekte | PascalCase, kein Präfix | `Mesh`, `Camera`, `Light`, `Entity`, `Surface`, `Material`, `Shader`, `Transform` |
| Manager-Klassen | PascalCase + `Manager`-Suffix | `ObjectManager`, `RenderManager`, `LightManager` |
| Reine Datenstrukturen | PascalCase | `MatrixSet`, `Core::Desc`, `LightBufferData` |
| Scoped Enums | `enum class` + PascalCase | `enum class RenderQueueType { Opaque, Transparent }` |
| Legacy-Enums / Flags | ALL\_CAPS | `D3DLIGHTTYPE`, `COLLISION` |


---

## 3. Methoden

**Alle Methoden PascalCase – ohne Ausnahme.**

```cpp
// RICHTIG
CreateMesh();
AddSurfaceToMesh();
DeleteCamera();
GetPosition();
SetWorldMatrix();
GetShaders();

// FALSCH
createMesh();
addSurfaceToMesh();
deleteCamera();
getPosition();
setWorldMatrix();
```

> Ausnahme: STL-kompatible Methoden wie `begin()`, `end()`, `size()` bleiben lowercase.

### Benennungsmuster

| Präfix | Bedeutung |
|--------|-----------|
| `Create...` | Erzeugt Objekt, alloziiert Speicher |
| `Delete...` | Gibt Objekt vollständig frei (inkl. Speicher) |
| `Add...` | Fügt Beziehung/Referenz hinzu (kein neuer Speicher) |
| `Remove...` | Entfernt Beziehung/Referenz (kein Speicher freigegeben) |
| `Get...` | Gibt vorhandenen Wert/Objekt zurück |
| `Set...` | Setzt Wert/Zustand |
| `Update...` | Aktualisiert internen Zustand (z.B. Matrizen, Buffer) |
| `Register...` | Trägt Objekt in interne Liste ein |
| `Unregister...` | Entfernt Objekt aus interner Liste |

---

## 4. Member-Variablen

| Kategorie | Konvention | Beispiele |
|-----------|-----------|-----------|
| Instanz-Member | `m_` + camelCase | `m_hwnd`, `m_screenWidth`, `m_bInitialized` |
| Statische Member | `s_` + camelCase | `s_instance` |
| Öffentliche Member (POD-Structs) | camelCase, kein Präfix | `transform`, `viewport`, `constantBuffer` |
| Lokale Variablen | camelCase, kein Präfix | `int indexCount;`, `Mesh* currentMesh;` |
| Konstanten / `#define` | ALL\_CAPS | `VERTEX_SHADER_FILE`, `SCREEN_DEPTH` |

---

## 5. Dateinamen

| Kategorie | Konvention | Beispiele |
|-----------|-----------|-----------|
| Klassen-Dateien | PascalCase, entspricht dem Klassennamen | `Mesh.h / Mesh.cpp`, `ObjectManager.h / ObjectManager.cpp` |
| Engine-interne Dateien | `gdx`-Präfix, lowercase | `gdxdevice.h/cpp`, `gdxengine.h/cpp`, `gdxutil.h/cpp` |
| Öffentliche API | — | `gidx.h` (einziger Header den Spielcode einbindet) |
| HLSL-Shader | PascalCase | `VertexShader.hlsl`, `PixelShader.hlsl` |

> **Faustregel:** Eine Klasse = eine `.h` und eine `.cpp`. Keine Header die mehrere unverwandte Klassen enthalten.

---

## 6. DirectX Buffer-Regeln

Dies sind die häufigsten Fehlerquellen im Projekt.

| Usage Flag | Erlaubtes Update | Verboten |
|------------|-----------------|---------|
| `D3D11_USAGE_DEFAULT` | `UpdateSubresource()` | `Map / Unmap` |
| `D3D11_USAGE_DYNAMIC` + `D3D11_CPU_ACCESS_WRITE` | `Map(WRITE_DISCARD)` ... `Unmap` | `UpdateSubresource` |
| `D3D11_USAGE_IMMUTABLE` | Nur bei Erstellung | Alles danach |

**Wann was verwenden:**

- **Constant Buffer** → immer `DYNAMIC` + `Map/Unmap`
- **Vertex/Index Buffer (statisch)** → `DEFAULT` oder `IMMUTABLE` + `UpdateSubresource`
- **Vertex/Index Buffer (dynamisch)** → `DYNAMIC` + `Map/Unmap`

---

## 7. Ressourcen-Verwaltung

- DirectX COM-Objekte werden mit `ComPtr<T>` verwaltet (automatische Freigabe).
- Engine-eigene Objekte (`Mesh*`, `Camera*`, `Light*` ...) werden **immer** über den zuständigen Manager freigegeben, nie direkt:

```cpp
objectManager->DeleteMesh(mesh);   // RICHTIG
delete mesh;                       // FALSCH
```

- `SafeRelease()` nur für COM-Objekte ohne `ComPtr` verwenden.
- Kein `new`/`delete` im Spielcode. Ausschließlich Engine-API verwenden.

---

## 8. Manager-Abhängigkeiten

```
GDXEngine  (zentrale Koordination)
│
├── ObjectManager       erzeugt / löscht alle Objekte
├── RenderManager       liest Shader-Liste via GetShaders()
├── ShaderManager       unabhängig
├── BufferManager       unabhängig
├── LightManager        unabhängig
├── CameraManager       unabhängig
└── TextureManager      unabhängig
```

**Regeln:**

- Manager kommunizieren **nicht** direkt untereinander.
- Braucht ein Manager Daten eines anderen, bekommt er sie über einen **öffentlichen Getter** – niemals über `friend` oder direkten Zugriff auf private Member.
- Braucht ein Manager eine Hardware-Ressource (`GDXDevice`), bekommt er sie vom `GDXEngine`-Konstruktor **direkt übergeben** – nicht über einen anderen Manager.

```cpp
// FALSCH – RenderManager greift auf privates Member von ObjectManager zu
RenderManager(ObjectManager& om) {
    om.m_shaders;        // direkter privater Zugriff
    om.m_device;         // GDXDevice hat in ObjectManager nichts verloren
}

// RICHTIG – saubere Übergabe
RenderManager(ObjectManager& om, LightManager& lm, GDXDevice& device) {
    om.GetShaders();     // öffentlicher Getter
    device.GetDeviceContext();  // direkt übergeben
}
```

---

## 9. Debug-Ausgaben

Jede `Debug::Log`-Meldung beginnt mit dem Dateinamen:

```cpp
Debug::Log("gdxdevice.cpp: Shadow Buffer erstellt");
Debug::LogError("rendermanager.cpp: Mesh hat keine Surface");
```

| Funktion | Verwendung |
|----------|-----------|
| `Debug::Log(...)` | Normaler Status |
| `Debug::LogWarning(...)` | Unerwarteter Zustand, nicht fatal |
| `Debug::LogError(...)` | Fehler mit sichtbarem Einfluss |
| `Debug::LogHr(...)` | HRESULT-Fehler |

> Per-Frame-Ausgaben müssen hinter einem Debug-Flag stehen. Im Release-Build darf kein `Debug::Log` aktiv pro Frame laufen.

---

## 10. Sonstige Regeln

- Shader-Dateipfade als Konstanten in `gdxengine.h` definieren, nicht hartcodiert im Code.
- Kommentare auf Englisch. Bestehende deutsche Kommentare bleiben, werden aber nicht weitergeführt.
- Eine Funktion, eine Aufgabe. Wenn ein Kommentar das Wort „und" enthält, sollte die Funktion aufgeteilt werden.
- Keine globalen Variablen außer `Engine::engine` (der Singleton-Pointer).
