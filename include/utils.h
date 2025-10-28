#include <GL/freeglut.h>

#include "GL/stb_image.h"
#include "game.h"
// #include <stdio.h>
#include <math.h>
// #include <string>
// #include <vector>
// #include <algorithm>

// ----------------------------------------------------------------------------------------------------------------

void drawRect(float initX, float initY, float width, float height, float r,
              float g, float b, float transparency = 1.0f);
void drawSpikes(float initX, float initY, float width, float height, float r,
                float g, float b);
void drawText(float x, float y, const char* text,
              void* font = GLUT_BITMAP_HELVETICA_18);
bool isPointInside(float pointX, float pointY, float rectX, float rectY,
                   float rectWidth, float rectHeight);
int getTextWidth(const char* text, void* font);
bool lineRectIntersection(float x1, float y1, float x2, float y2,
                          const Platform& plataform, float& hitX, float& hitY);
void clipLineToView(float startX, float startY, float& endX, float& endY,
                    float VIEW_WIDTH, float VIEW_HEIGHT, float cameraLeft,
                    float cameraBottom);
void drawVector(float initX, float initY, float vX_physics, float vY_physics,
                float scale, float r, float g, float b, const char* vectorName);
bool checkRectangleCollision(float x1, float y1, float w1, float h1, float x2,
                             float y2, float w2, float h2);
void drawTextCentered(float cx, float y, const char* text, void* font);

GLuint loadTexture(const char* filepath);
void drawTexturedRect(float x, float y, float w, float h, GLuint textureID,
                      bool flipH = false, bool flipV = true);
