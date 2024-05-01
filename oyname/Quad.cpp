//#include "gidx.h"
/*
int main_off4()
{
	Engine::Graphics(1024, 768);

	// Creating camera object
	LPCAMERA camera;
	Engine::CreateCamera(&camera);
	// Positioning the camera
	Engine::MoveEntity(camera, 0.0f, 0.0f, -4.0f);

	// Creating light
	LPLIGHT light;
	Engine::CreateLight(&light, D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL);
	Engine::RotateEntity(light, 0.0f, 0.0f, 0.0f);

	LPTEXTURE texture = nullptr;
	Engine::LoadTexture(&texture, L"..\\oyname\\Texture\\test4.bmp");

	// Speichert Position, Rotation und Skalierung
	LPMESH quad = NULL;
	Engine::CreateMesh(&quad);
	Engine::EntityTexture(quad, texture);

	// Speicherplatz für die Vertices
	LPSURFACE quadData = NULL;
	Engine::CreateSurface(&quadData, quad);

	// Vertex für das Quad erstellen
	Engine::AddVertex(quadData, -1.0f, -1.0f, -1.0f); Engine::VertexNormal(quadData, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(quadData, 224, 224, 224);	Engine::VertexTexCoord(quadData, 0.0f, 1.0f);
	Engine::AddVertex(quadData, -1.0f, 1.0f, -1.0f);  Engine::VertexNormal(quadData, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(quadData, 224, 224, 224);	Engine::VertexTexCoord(quadData, 0.0f, 0.0f);
	Engine::AddVertex(quadData, 1.0f, -1.0f, -1.0f);  Engine::VertexNormal(quadData, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(quadData, 224, 224, 224);	Engine::VertexTexCoord(quadData, 1.0f, 1.0f);
	Engine::AddVertex(quadData, 1.0f, 1.0f, -1.0f);   Engine::VertexNormal(quadData, 0.0f, 0.0f, -1.0f);  Engine::VertexColor(quadData, 224, 224, 224);	Engine::VertexTexCoord(quadData, 1.0f, 0.0f);

	// Vertices zu einem Quad verknüpfen
	Engine::AddTriangle(quadData, 0, 1, 2); 
	Engine::AddTriangle(quadData, 3, 2, 1);

	// Buffer mit den Daten befüllen
	Engine::FillBuffer(quadData);

	while (gdx::MainLoop() && !(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
	{

		Engine::RenderWorld();
		Engine::Flip();
	}

	return gdx::ShutDown();
}
*/