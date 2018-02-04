#include <math.h>

double miaux_distance(double x1, double y1, double x2, double y2)
{
    double x = x2 - x1, y = y2 - y1;
    return sqrt(x * x + y * y);
}

double miaux_fit(
    double v, double oldmin, double oldmax, double newmin, double newmax)    
{
    return newmin + ((v - oldmin) / (oldmax - oldmin)) * (newmax - newmin);
}