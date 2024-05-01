//#include "gidx.h"

// Engine::CreateTexture(&texture, width, height):
// Diese Funktion erstellt eine neue Textur mit den angegebenen Abmessungen (Breite und Höhe). Die Textur wird im Speicher reserviert 
// und die Referenz auf die Textur wird in der Variable "texture" gespeichert.
// 
// @param texture Ein Zeiger auf einen Zeiger, der auf die neu erstellte Textur zeigen soll.
// @param width Die Breite der zu erstellenden Textur.
// @param height Die Höhe der zu erstellenden Textur.

// Engine::LockBuffer(texture):
// Diese Funktion sperrt den Speicherbereich der Textur, um direkten Zugriff auf die Pixelinformationen zu ermöglichen. Dies ist notwendig, 
// um Pixelwerte zu ändern oder direkt auf die Textur zuzugreifen.
// 
// texture Ein Zeiger auf das Texture-Objekt, dessen Speicherbereich gesperrt werden soll.

// Engine::SetPixel(texture, x, y, r, g, b, a):
// Diese Funktion setzt den Pixel an den angegebenen Koordinaten (x, y) der Textur auf die angegebene Farbe. Die Farbe wird durch die 
// Rot-, Grün-, Blau- und Alpha-Komponenten definiert, wobei jeder Wert zwischen 0 und 255 liegt.
// 
// texture Ein Zeiger auf das Texture-Objekt, dessen Pixelwert gesetzt werden soll.
// x Die X-Koordinate des Pixels.
// y Die Y-Koordinate des Pixels.
// r Der Rotwert des Pixels (0-255).
// g Der Grünwert des Pixels (0-255).
// b Der Blauwert des Pixels (0-255).
// a Der Alphawert des Pixels (0-255).

// Engine::UnlockBuffer(texture):
// Diese Funktion entsperrt den Speicherbereich der Textur, nachdem der direkte Zugriff auf die Pixelinformationen abgeschlossen ist. 
// Dies ermöglicht es der Grafik-Engine wieder auf die Textur zuzugreifen oder mit ihr zu arbeiten.
// 
// texture Ein Zeiger auf das Texture-Objekt, dessen Speicherbereich entsperrt werden soll.

// GetColor(texture, x, y):
// Diese Funktion extrahiert die Farbinformationen eines Pixels an den angegebenen Koordinaten aus einem Texture-Objekt.
// 
// texture Ein Zeiger auf das Texture-Objekt, aus dem die Farbinformationen gelesen werden sollen.
// x Die X-Koordinate des Pixels.
// y Die Y-Koordinate des Pixels.
// Rückgabewert ein Color-Objekt, das die extrahierten Farbinformationen des Pixels enthält.

//void CreateCube(LPMESH* mesh, BRUSH* brush = nullptr);

// Diese Funktion nimmt einen Wert value, dessen ursprünglichen Bereich(in_min bis in_max) und den Bereich,
// in den er abgebildet werden soll(out_min bis out_max), und gibt den abgebildeten Wert zurück.Zum Beispiel
// wird der Wert value von seinem ursprünglichen Bereich in den neuen Bereich gemappt.
//double map(double value, double in_min, double in_max, double out_min, double out_max) {
//   return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//}

//int main()
//{
//    // Das erste Brush wird von der Engine bei Start erstellt
//    Engine::Graphics(1280, 720);
//
//    LPTEXTURE texture = nullptr;
//
//    Engine::CreateTexture(&texture, 256, 256);
//    Engine::LockBuffer(texture);
//
//    //for (int x = 0; x < 256; x++) {
//    //    for (int y = 0; y < 256; y++) {
//    //        // Wenn sowohl x und y gerade oder ungerade sind, setze den Pixel auf Schwarz
//    //        if ((x / 32) % 2 == (y / 32) % 2) {
//    //            Engine::SetPixel(texture, x, y, 255, 255, 255, 255); // Schwarz
//    //        }
//    //        else {
//    //            Engine::SetPixel(texture, x, y, 0, 0, 0, 255); // Weiß
//    //        }
//    //    }
//    //}
//    const int MAX_ITERATIONS = 100;
//    for (int x = 0; x < 256; x++) {
//        for (int y = 0; y < 256; y++) {
//            double cx = map(x, 0, 256, -2.5, 1); // Mapping der Pixelkoordinaten auf den Mandelbrot-Bereich
//            double cy = map(y, 0, 256, -1, 1);
//
//            double zx = 0;
//            double zy = 0;
//            int iteration = 0;
//
//            while (zx * zx + zy * zy < 4 && iteration < MAX_ITERATIONS) {
//                double temp = zx * zx - zy * zy + cx;
//                zy = 2 * zx * zy + cy;
//                zx = temp;
//                iteration++;
//            }
//
//            if (iteration == MAX_ITERATIONS) {
//                Engine::SetPixel(texture, x, y, 0, 0, 0, 255); // Schwarz für Punkte im Mandelbrot-Set
//            }
//            else {
//                Engine::SetPixel(texture, x, y, 255, 255, 255, 255); // Weiß für Punkte außerhalb des Mandelbrot-Set
//            }
//        }
//    }
//
//    for (int x = 0; x < 256; x++) {
//        for (int y = 0; y < 256; y++) {
//            if (Engine::GetColor(texture, x, y) == Engine::Color(255, 255, 255, 255)) {
//                // Gradientenberechnung basierend auf den Pixelkoordinaten
//                int gradientR = map(x, 0, 256, 0, 255);
//                int gradientG = map(y, 0, 256, 0, 255);
//                int gradientB = 128; // Konstanter Wert für Blau
//
//                // Aktualisierung des Pixels mit dem Gradienten
//                Engine::SetPixel(texture, x, y, gradientR, gradientG, gradientB, 255);
//            }
//        }
//    }
//
//    Engine::UnlockBuffer(texture);
//
//
//
//    // Creating camera object
//    LPCAMERA camera;
//    Engine::CreateCamera(&camera);
//    // Positioning the camera
//    Engine::RotateEntity(camera, 60.0f, 0.0f, 0.0f);
//    Engine::MoveEntity(camera, 0.0f, 0.0f, -5.0f);
//
//    // Creating light
//    LPLIGHT light;
//    Engine::CreateLight(&light, D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL);
//    Engine::RotateEntity(light, 0.0f, 0.0f, 0.0f);
//
//    // Create cube
//    LPMESH cube;
//    CreateCube(&cube);
//    Engine::EntityTexture(cube, texture); // Die Textur wird zum Brush, dass zu cube gehört angehängt. In diesem Fall dem Standard-Brush
//
//
//    while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) // Main loop
//    {
//        Engine::Cls(80, 80, 80);
//
//        Engine::TurnEntity(cube, 0.3f, 0.0, 0.1f);
//
//        Engine::RenderWorld();
//
//        Engine::Flip();
//    }
//
//    // Shutdown the engine
//    return gdx::ShutDown();
//}
//
//// Function definitions
//void CreateCube(LPMESH* mesh, BRUSH* brush)
//{
//    LPSURFACE wuerfel = NULL;
//
//    Engine::CreateMesh(mesh, brush);
//    Engine::CreateSurface(&wuerfel, (*mesh));
//
//    // Definition der Eckpunkte für jede Seite des Würfels
//    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//
//    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 0.0f, 1.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//
//    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, -1.0f, 0.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//
//    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 1.0f, 0.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//
//    Engine::AddVertex(wuerfel, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, -1.0f, -1.0f, 1.0f);  Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, -1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, -1.0f, 0.0f);  Engine::VertexColor(wuerfel, 224, 224, 224);
//
//    Engine::AddVertex(wuerfel, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, -1.0f, 1.0f, 1.0f);   Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//    Engine::AddVertex(wuerfel, 1.0f, 1.0f, 1.0f);    Engine::VertexNormal(wuerfel, 0.0f, 1.0f, 0.0f);   Engine::VertexColor(wuerfel, 224, 224, 224);
//
//    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);
//
//    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);
//
//    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);
//
//    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);
//
//    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);
//
//    Engine::VertexTexCoord(wuerfel, 0.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 0.0f, 0.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 1.0f);
//    Engine::VertexTexCoord(wuerfel, 1.0f, 0.0f);
//
//    // Definition der Dreiecke für jede Seite des Würfels
//    Engine::AddTriangle(wuerfel, 0, 1, 2);  // Seite A
//    Engine::AddTriangle(wuerfel, 3, 2, 1);
//
//    Engine::AddTriangle(wuerfel, 6, 5, 4);  // Seite B
//    Engine::AddTriangle(wuerfel, 6, 7, 5);
//
//    Engine::AddTriangle(wuerfel, 8, 9, 10);  // Seite C
//    Engine::AddTriangle(wuerfel, 10, 9, 11);
//
//    Engine::AddTriangle(wuerfel, 14, 13, 12); // Seite D
//    Engine::AddTriangle(wuerfel, 14, 15, 13);
//
//    Engine::AddTriangle(wuerfel, 16, 17, 18);  // Seite E
//    Engine::AddTriangle(wuerfel, 18, 17, 19);
//
//    Engine::AddTriangle(wuerfel, 21, 22, 23);  // Seite F
//    Engine::AddTriangle(wuerfel, 22, 21, 20);
//
//    Engine::FillBuffer(wuerfel);
//}