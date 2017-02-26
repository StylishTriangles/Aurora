#ifndef RAY_INTERSECT
#define RAY_INTERSECT
#include <QMatrix4x4>
#include <QVector>
#include <GL/gl.h>


void screenPosToWorldRay(
    int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
    int screenWidth, int screenHeight,  // Window size, in pixels
    QMatrix4x4 ViewMatrix,               // Camera position and orientation
    QMatrix4x4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
    QVector3D& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
    QVector3D& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
);

bool testRayOBBIntersection(
    QVector3D ray_origin,        // Ray origin, in world space
    QVector3D ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
    QVector3D aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
    QVector3D aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
    QMatrix4x4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
    float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
);

GLfloat testRayPreciselyIntersection(
    const QVector<GLfloat>& geometry,
    QVector3D ray_origin,
    QVector3D ray_direction,
    QMatrix4x4 ModelMatrix,
    int stride,
    int vertexPos
);

#endif
