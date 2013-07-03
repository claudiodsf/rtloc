#ifndef GEOMETRY_H
#define GEOMETRY_H
/*------------------------------------------------------------/ */
/* Linear Algebra  */
/*------------------------------------------------------------/ */

/* Vector2D */

typedef struct
{
     double x, y;
}
Vect2D;


/* Vector3D */

typedef struct
{
     double x, y, z;
}
Vect3D;


/* Matrix3D */

typedef struct
{
     double xx, xy, xz;
     double yx, yy, yz;
     double zx, zy, zz;
}
Mtrx3D;


/* */
/*------------------------------------------------------------/ */
#endif //GEOMETRY_H
