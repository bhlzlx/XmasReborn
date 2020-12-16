#include "CRandGen.h"

float CRandGen::m_randCount = 0;
float CRandGen::m_randMaxFlag = 10;
//srand((unsigned int)time(NULL));

int CRandGen::Rand()
{

    m_randCount++;
    if(m_randCount>m_randMaxFlag)
    {
        m_randCount = 0;
        m_randMaxFlag = rand();
        srand((unsigned int)time(NULL));
    }
    return rand();
}
