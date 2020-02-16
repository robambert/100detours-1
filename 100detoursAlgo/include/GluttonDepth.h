#ifndef GLUTTONDEPTH_H
#define GLUTTONDEPTH_H

#include "Algorithm.h"
#include "Inputs.h"

// deprecated
class GluttonDepth : public Algorithm
{
    public:

        GluttonDepth(Inputs *inputs);


    private:
    
        virtual void find_appointments();
};

#endif // GLUTTONDEPTH_H
