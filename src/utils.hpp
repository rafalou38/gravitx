#ifndef G
#define G (6.67430 * pow(10, -11))  //  m^3 kg^−1 s^−2
typedef unsigned int uint;

typedef struct Vector3l {
    long double x;                // Vector x component
    long double y;                // Vector y component
    long double z;                // Vector z component
} Vector3l;


// (Cumulative)
typedef struct DetailedTime {
    unsigned int years;
    unsigned int days;
    unsigned int hours;
    unsigned int minutes;
} DetailedTime;

#endif