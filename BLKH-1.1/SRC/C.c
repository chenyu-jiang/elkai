#include "LKH.h"

/* Functions for computing lower bounds for the distance functions */

int c_ATT(Node * Na, Node * Nb)
{
    int dx = (int) (ceil(0.31622 * fabs(Na->X - Nb->X))),
        dy = (int) (ceil(0.31622 * fabs(Na->Y - Nb->Y)));
    return dx > dy ? dx : dy;
}

int c_CEIL_2D(Node * Na, Node * Nb)
{
    int dx = (int) ceil(fabs(Na->X - Nb->X)),
        dy = (int) ceil(fabs(Na->Y - Nb->Y));
    return dx > dy ? dx : dy;
}

int c_CEIL_3D(Node * Na, Node * Nb)
{
    int dx = (int) ceil(fabs(Na->X - Nb->X)),
        dy = (int) ceil(fabs(Na->Y - Nb->Y)),
        dz = (int) ceil(fabs(Na->Z - Nb->Z));
    if (dy > dx)
        dx = dy;
    if (dz > dx)
        dx = dz;
    return dx;
}

int c_EUC_2D(Node * Na, Node * Nb)
{
    int dx = (int) (fabs(Na->X - Nb->X) + 0.5),
        dy = (int) (fabs(Na->Y - Nb->Y) + 0.5);
    return dx > dy ? dx : dy;
}

int c_EUC_3D(Node * Na, Node * Nb)
{
    int dx = (int) (fabs(Na->X - Nb->X) + 0.5),
        dy = (int) (fabs(Na->Y - Nb->Y) + 0.5),
        dz = (int) (fabs(Na->Z - Nb->Z) + 0.5);
    if (dy > dx)
        dx = dy;
    if (dz > dx)
        dx = dz;
    return dx;
}

#define PI 3.141592
#define RRR 6378.388

int c_GEO(Node * Na, Node * Nb)
{
    int da = (int) Na->X, db = (int) Nb->X;
    double ma = Na->X - da, mb = Nb->X - db;
    int dx =
        (int) (RRR * PI / 180.0 * fabs(da - db + 5.0 * (ma - mb) / 3.0) +
               1.0);
    return dx;
}

#undef M_PI
#define M_PI 3.14159265358979323846264
#define M_RRR 6378388.0

int c_GEOM(Node * Na, Node * Nb)
{
    int dx = (int) (M_RRR * M_PI / 180.0 * fabs(Na->X - Nb->X) + 1.0);
    return dx;
}

#define f (1 - 1 / 298.257)

int c_GEO_MEEUS(Node * Na, Node * Nb)
{
    int da = (int) Na->X, db = (int) Nb->X;
    double ma = Na->X - da, mb = Nb->X - db;
    int dx =
        (int) (RRR * M_PI / 180.0 * fabs(da - db + 5.0 * (ma - mb) / 3.0) *
               f + 0.5);
    return dx;
}

int c_GEOM_MEEUS(Node * Na, Node * Nb)
{
    int dx = (int) (M_RRR * M_PI / 180.0 * fabs(Na->X - Nb->X) * f + 0.5);
    return dx;
}
