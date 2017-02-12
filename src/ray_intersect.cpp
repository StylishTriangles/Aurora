#include "../include/ray_intersect.h"

void screenPosToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight,
      QMatrix4x4 ViewMatrix, QMatrix4x4 ProjectionMatrix, QVector3D& out_origin, QVector3D& out_direction)
{
	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
    QVector4D lRayStart_NDC(
		((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY/(float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
    QVector4D lRayEnd_NDC(
		((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f,
		((float)mouseY/(float)screenHeight - 0.5f) * 2.0f,
		0.0,
		1.0f
    );
	// Faster way (just one inverse)
    QMatrix4x4 M = (ProjectionMatrix * ViewMatrix).inverted();
    QVector4D lRayStart_world = M * lRayStart_NDC; lRayStart_world /= lRayStart_world.w();
    QVector4D lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /= lRayEnd_world.w();

    QVector3D lRayDir_world(lRayEnd_world - lRayStart_world);
    lRayDir_world = (lRayDir_world).normalized();

    out_origin = QVector3D(lRayStart_world);
    out_direction = (lRayDir_world).normalized();
}

bool testRayOBBIntersection(QVector3D ray_origin, QVector3D ray_direction, QVector3D aabb_min,
                            QVector3D aabb_max, QMatrix4x4 ModelMatrix, float& intersection_distance){
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games
	float tMin = 0.0f;
	float tMax = 100000.0f;
    QVector3D OBBposition_worldspace(ModelMatrix.column(3).x(), ModelMatrix.column(3).y(), ModelMatrix.column(3).z());
    QVector3D delta = OBBposition_worldspace - ray_origin;
	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
        QVector3D xaxis(ModelMatrix.column(0).x(), ModelMatrix.column(0).y(), ModelMatrix.column(0).z());
        float e = QVector3D::dotProduct(xaxis, delta);
        float f = QVector3D::dotProduct(ray_direction, xaxis);
		if ( fabs(f) > 0.001f ){ // Standard case
            float t1 = (e+aabb_min.x())/f; // Intersection with the "left" plane
            float t2 = (e+aabb_max.x())/f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections
			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1>t2){
                t1-=t2;t2+=t1;t1=-t1+t2; // swap t1 and t2
			}
			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if ( t2 < tMax )
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if ( t1 > tMin )
				tMin = t1;
			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin )
				return false;
        }
        else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if(-e+aabb_min.x() > 0.0f || -e+aabb_max.x() < 0.0f)
				return false;
		}
	}
	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
        QVector3D yaxis(ModelMatrix.column(1).x(), ModelMatrix.column(1).y(), ModelMatrix.column(1).z());
        float e = QVector3D::dotProduct(yaxis, delta);
        float f = QVector3D::dotProduct(ray_direction, yaxis);
		if ( fabs(f) > 0.001f ){
            float t1 = (e+aabb_min.y())/f;
            float t2 = (e+aabb_max.y())/f;

            if (t1>t2){
                t1-=t2;t2+=t1;t1=-t1+t2;
            }

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

        }
        else{
            if(-e+aabb_min.y() > 0.0f || -e+aabb_max.y() < 0.0f)
				return false;
		}
	}
	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
        QVector3D zaxis(ModelMatrix.column(2).x(), ModelMatrix.column(2).y(), ModelMatrix.column(2).z());
        float e = QVector3D::dotProduct(zaxis, delta);
        float f = QVector3D::dotProduct(ray_direction, zaxis);
		if ( fabs(f) > 0.001f ){
            float t1 = (e+aabb_min.z())/f;
            float t2 = (e+aabb_max.z())/f;
            if (t1>t2){
                t1-=t2;t2+=t1;t1=-t1+t2;
            }
			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;
        }
        else{
            if(-e+aabb_min.z() > 0.0f || -e+aabb_max.z() < 0.0f)
				return false;
		}
	}
	intersection_distance = tMin;
	return true;
}
