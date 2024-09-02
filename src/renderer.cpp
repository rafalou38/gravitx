#include <iostream>
#include <vector>
#include <unordered_map>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "consts.hpp"
#include "Entity.hpp"
#include "simulator.hpp"
#include "UI.hpp"
#include "gravitx.hpp"

#include "renderer.hpp"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

Renderer::Renderer(AppComponents components)
{
    this->components = components;

#ifdef D3D
    resetCamera();
    camera = {0};

    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                         // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;      // Camera projection type

    updateCameraPos();
#endif
}

void Renderer::initialiseSkybox(string path, int factor)
{
    if (factor == 0)
        return;
    // Load skybox model
    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    skybox = LoadModelFromMesh(cube);

    const int useHDR = 1;

    // Load skybox shader and set required locations
    // NOTE: Some locations are automatically set at shader loading
    skybox.materials[0].shader = LoadShader(TextFormat("shaders/glsl%i/skybox.vs", GLSL_VERSION),
                                            TextFormat("shaders/glsl%i/skybox.fs", GLSL_VERSION));

    int valueA[] = {MATERIAL_MAP_CUBEMAP};
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "environmentMap"), valueA, SHADER_UNIFORM_INT);

    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "doGamma"), &useHDR, SHADER_UNIFORM_INT);
    SetShaderValue(skybox.materials[0].shader, GetShaderLocation(skybox.materials[0].shader, "vflipped"), &useHDR, SHADER_UNIFORM_INT);

    // Load cubemap shader and setup required shader locations
    Shader shdrCubemap = LoadShader(TextFormat("shaders/glsl%i/cubemap.vs", GLSL_VERSION),
                                    TextFormat("shaders/glsl%i/cubemap.fs", GLSL_VERSION));

    const int equirectangularMap = 0;

    SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), &equirectangularMap, SHADER_UNIFORM_INT);

    char skyboxFileName[256] = {0};

    Texture2D panorama;

    TextCopy(skyboxFileName, path.c_str());

    // Load HDR panorama (sphere) texture
    panorama = LoadTexture(skyboxFileName);

    // Generate cubemap (texture with 6 quads-cube-mapping) from panorama HDR texture
    // NOTE 1: New texture is generated rendering to texture, shader calculates the sphere->cube coordinates mapping
    // NOTE 2: It seems on some Android devices WebGL, fbo does not properly support a FLOAT-based attachment,
    // despite texture can be successfully created.. so using PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 instead of PIXELFORMAT_UNCOMPRESSED_R32G32B32A32
    // 10:1k 11:2k 12:4k 13:8k 14:16k
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = GenTextureCubemap(shdrCubemap, panorama, pow(2, factor), PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    UnloadTexture(panorama); // Texture not required anymore, cubemap already generated

    skybox_loaded = true;
}

TextureCubemap Renderer::GenTextureCubemap(Shader shader, Texture2D panorama, int size, int format)
{
    TextureCubemap cubemap = {0};

    rlDisableBackfaceCulling(); // Disable backface culling to render inside the cube

    // STEP 1: Setup framebuffer
    //------------------------------------------------------------------------------------------
    unsigned int rbo = rlLoadTextureDepth(size, size, true);
    cubemap.id = rlLoadTextureCubemap(0, size, format);

    unsigned int fbo = rlLoadFramebuffer();
    rlFramebufferAttach(fbo, rbo, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X, 0);

    // Check if framebuffer is complete with attachments (valid)
    if (rlFramebufferComplete(fbo))
        TraceLog(LOG_WARNING, "FBO: [ID %i] Framebuffer object created successfully", fbo);
    //------------------------------------------------------------------------------------------

    // STEP 2: Draw to framebuffer
    //------------------------------------------------------------------------------------------
    // NOTE: Shader is used to convert HDR equirectangular environment map to cubemap equivalent (6 faces)
    rlEnableShader(shader.id);

    // Define projection matrix and send it to shader
    Matrix matFboProjection = MatrixPerspective(90.0 * DEG2RAD, 1.0, RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
    rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_PROJECTION], matFboProjection);

    // Define view matrix for every side of the cubemap
    Matrix fboViews[6] = {
        MatrixLookAt((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){1.0f, 0.0f, 0.0f}, (Vector3){0.0f, -1.0f, 0.0f}),
        MatrixLookAt((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){-1.0f, 0.0f, 0.0f}, (Vector3){0.0f, -1.0f, 0.0f}),
        MatrixLookAt((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, (Vector3){0.0f, 0.0f, 1.0f}),
        MatrixLookAt((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, -1.0f, 0.0f}, (Vector3){0.0f, 0.0f, -1.0f}),
        MatrixLookAt((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 0.0f, 1.0f}, (Vector3){0.0f, -1.0f, 0.0f}),
        MatrixLookAt((Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 0.0f, -1.0f}, (Vector3){0.0f, -1.0f, 0.0f})};

    rlViewport(0, 0, size, size); // Set viewport to current fbo dimensions
    // return cubemap;

    // Activate and enable texture for drawing to cubemap faces
    rlActiveTextureSlot(0);
    rlEnableTexture(panorama.id);

    for (int i = 0; i < 6; i++)
    {
        // Set the view matrix for the current cube face
        rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_VIEW], fboViews[i]);

        // Select the current cubemap face attachment for the fbo
        // WARNING: This function by default enables->attach->disables fbo!!!
        rlFramebufferAttach(fbo, cubemap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X + i, 0);
        rlEnableFramebuffer(fbo);

        // Load and draw a cube, it uses the current enabled texture
        rlClearScreenBuffers();
        rlLoadDrawCube();

        // ALTERNATIVE: Try to use internal batch system to draw the cube instead of rlLoadDrawCube
        // for some reason this method does not work, maybe due to cube triangles definition? normals pointing out?
        // TODO: Investigate this issue...
        // rlSetTexture(panorama.id); // WARNING: It must be called after enabling current framebuffer if using internal batch system!
        // rlClearScreenBuffers();
        // DrawCubeV(Vector3Zero(), Vector3One(), WHITE);
        // rlDrawRenderBatchActive();
    }
    //------------------------------------------------------------------------------------------

    // STEP 3: Unload framebuffer and reset state
    //------------------------------------------------------------------------------------------
    rlDisableShader();        // Unbind shader
    rlDisableTexture();       // Unbind texture
    rlDisableFramebuffer();   // Unbind framebuffer
    rlUnloadFramebuffer(fbo); // Unload framebuffer (and automatically attached depth texture/renderbuffer)

    // Reset viewport dimensions to default
    rlViewport(0, 0, windowsSize.x, windowsSize.y);
    // rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    rlEnableBackfaceCulling();
    //------------------------------------------------------------------------------------------

    cubemap.width = size;
    cubemap.height = size;
    cubemap.mipmaps = 1;
    cubemap.format = format;

    return cubemap;
}

Renderer::~Renderer()
{
}
void Renderer::resetCamera()
{
    cameraPos = {
        .d = 100,
        .theta = DEG2RAD * -270,
        .phi = DEG2RAD * 270};
}

#ifdef D3D
void Renderer::updateCameraPos()
{
    cameraFlipped = fmod(abs(cameraPos.phi), 2 * PI) > PI;

    camera.position.x = cameraPos.d * cos(cameraPos.theta) * sin(cameraPos.phi);
    camera.position.y = cameraPos.d * cos(cameraPos.phi);
    camera.position.z = cameraPos.d * sin(cameraPos.theta) * sin(cameraPos.phi);

    if (cameraFlipped)
        camera.up.y = -1;
    else
        camera.up.y = 1;
}
#endif

void Renderer::update()
{
#ifdef D3D
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !components.ui->isCursorInWindow())
    {
        Vector2 mousePositionDelta = GetMouseDelta();

        if (cameraFlipped)
            cameraPos.theta -= mousePositionDelta.x / windowsSize.x * 2 * PI;
        else
            cameraPos.theta += mousePositionDelta.x / windowsSize.x * 2 * PI;

        cameraPos.phi -= mousePositionDelta.y / windowsSize.y * 2 * PI;
        if (cameraPos.phi < 0)
            cameraPos.phi = 2 * PI + cameraPos.phi;

        updateCameraPos();
    }
#endif
}

#ifdef D3D
void Renderer::render3D(Simulator *sim)
{
    ClearBackground(BLACK);
    BeginMode3D(camera);

#ifdef SKY_BOX
    if (skybox_loaded)
    {
        rlDisableBackfaceCulling();
        rlDisableDepthMask();
        DrawModel(skybox, (Vector3){0, 0, 0}, 1.0f, WHITE);
        rlEnableBackfaceCulling();
        rlEnableDepthMask();
    }
#endif

#ifdef GIZMOS
    // DrawGrid(20, scale * 100000.0f);
    DrawLine3D({0, 0, 0}, {100, 0, 0}, ColorAlpha(RED, 0.3));
    DrawLine3D({0, 0, 0}, {0, 100, 0}, ColorAlpha(GREEN, 0.3));
    DrawLine3D({0, 0, 0}, {0, 0, 100}, ColorAlpha(BLUE, 0.3));
#endif

    // prevCnt = sim->iterCnt;
    // for (Entity *entity : sim->entities)
    // {
    //     entity->swap();
    // }

    Vector3l center = {
        sim->origin->position_freeze->x,
        // 0,
        sim->origin->position_freeze->y,
        // 0,
        sim->origin->position_freeze->z};
    Vector3 pos;

    for (Entity *entity : sim->entities)
    {
        pos = Vector3{
            (float)((entity->position_freeze->x - center.x) * scale),
            (float)((entity->position_freeze->y - center.y) * scale),
            (float)((entity->position_freeze->z - center.z) * scale)};

        auto points = sim->lines.find((size_t)entity)->second;
        if (points->size() >= 2)
        {
            Vector3 startPos;
            Vector3 endPos;

            for (size_t i = 0; i < points->size() - 1; i += 1)
            {
                startPos = {
                    (float)((points->at(i).x - center.x) * scale),
                    (float)((points->at(i).y - center.y) * scale),
                    (float)((points->at(i).z - center.z) * scale),
                };
                if (i + 1 >= points->size())
                    break;

                try
                {
                    endPos = {
                        (float)((points->at(i + 1).x - center.x) * scale),
                        (float)((points->at(i + 1).y - center.y) * scale),
                        (float)((points->at(i + 1).z - center.z) * scale),
                    };
                }
                catch (const std::exception &e)
                {
                    break;
                }

                Color color = ColorAlpha(entity->color, 0.6 * i / (points->size() - 1));
                DrawLine3D(
                    startPos,
                    endPos,
                    color);
            }
            Color color = ColorAlpha(entity->color, 0.6);
            startPos = {
                (float)((points->back().x - center.x) * scale),
                (float)((points->back().y - center.y) * scale),
                (float)((points->back().z - center.z) * scale),
            };
            endPos = pos;
            DrawLine3D(startPos, endPos, color);
        }

        pos = Vector3{
            (float)((entity->position_freeze->x - center.x) * scale),
            (float)((entity->position_freeze->y - center.y) * scale),
            (float)((entity->position_freeze->z - center.z) * scale)};
        if (entity->texturePath != "")
        {
            DrawModel(entity->getModel(), pos, max(0.1f, entity->radius * scale), WHITE); // 20
        }
        else
        {
            DrawModel(entity->getModel(), pos, max(0.1f, entity->radius * scale), entity->color);
        }
    }

    EndMode3D();

    vector<Vector2> labels = {};
    for (Entity *entity : sim->entities)
    {
        pos = Vector3{
            (float)((entity->position_freeze->x - center.x) * scale),
            (float)((entity->position_freeze->y - center.y) * scale),
            (float)((entity->position_freeze->z - center.z) * scale)};
        Vector2 screenPos = GetWorldToScreen(pos, camera);

        for (auto labelPos : labels)
        {
            if (abs(screenPos.y - labelPos.y) < ENTITY_LABEL_SIZE +2 and abs(screenPos.x - labelPos.x) < 50)
                screenPos.y += ENTITY_LABEL_SIZE;
        }

        labels.push_back(screenPos);

        DrawText(
            entity->label.c_str(),
            (int)screenPos.x,
            (int)screenPos.y + ENTITY_LABEL_SIZE + 2,
            ENTITY_LABEL_SIZE,
            WHITE);
    }
}
#endif

void Renderer::render(Simulator *sim)
{
    Vector3l center = {
        sim->origin->position->x,
        sim->origin->position->y,
        0};
    ClearBackground(BLACK);
    for (Entity *entity : sim->entities)
    {
        if (entity->texturePath == "")
        {
            DrawCircle(
                round((entity->position->x - center.x) * scale + windowsSize.x / 2),
                round((entity->position->y - center.y) * scale + windowsSize.y / 2),
                entity->radius * scale,
                entity->color);
        }
        else
        {
            auto texture = entity->getTexture();
            float textureScale = (entity->radius * scale * 2.0f) / texture->width;
            DrawTextureEx(
                *texture,
                {
                    (float)round((entity->position->x - center.x) * scale + windowsSize.x / 2) - entity->radius * scale,
                    (float)round((entity->position->y - center.y) * scale + windowsSize.y / 2) - entity->radius * scale,
                },
                0, textureScale, WHITE);
        }

        auto points = sim->lines.find((size_t)entity)->second;
        if (points->size() >= 2)
        {
            for (size_t i = 0; i < points->size() - 1; i += 1)
            {
                Color color = ColorAlpha(entity->color, 0.6 * i / (points->size() - 1));
                DrawLine(
                    round((points->at(i).x - center.x) * scale + windowsSize.x / 2),
                    round((points->at(i).y - center.y) * scale + windowsSize.y / 2),
                    round((points->at(i + 1).x - center.x) * scale + windowsSize.x / 2),
                    round((points->at(i + 1).y - center.y) * scale + windowsSize.y / 2),
                    color);
            }
            Color color = ColorAlpha(entity->color, 0.6);
            DrawLine(
                round((points->back().x - center.x) * scale + windowsSize.x / 2),
                round((points->back().y - center.y) * scale + windowsSize.y / 2),
                round((entity->position->x - center.x) * scale + windowsSize.x / 2),
                round((entity->position->y - center.y) * scale + windowsSize.y / 2),
                color);
        }
    }
}
