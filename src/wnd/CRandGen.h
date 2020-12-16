#ifndef CRANDGEN_H
#define CRANDGEN_H

#include <ctime>
#include <cmath>
#include <cstdlib>
#include <cstdio>

class CRandGen
{
public:
    static int Rand();
protected:
    static float m_randCount;
    static float m_randMaxFlag;
};

#endif // CRANDGEN_H
