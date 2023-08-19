#ifndef GELLY_GELLY_H
#define GELLY_GELLY_H

#include <GellyEngine.h>
#include <GellyRenderer.h>

struct GellyInitParams {
    int maxParticles;
    int maxColliders;
    int width;
    int height;
    SharedTextures sharedTextures;
};

class Gelly {
private:
    GellyScene* scene;
    GellyRenderer* renderer;
public:
    explicit Gelly(const GellyInitParams& params);
    void LoadMap(const char* mapName);
    void Update(float deltaTime);
    void Render();
    ~Gelly();
};

#endif //GELLY_GELLY_H
