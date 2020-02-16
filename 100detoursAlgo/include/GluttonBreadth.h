#ifndef GLUTTONBREADTH_H
#define GLUTTONBREADTH_H

#include "Algorithm.h"
#include "Inputs.h"

// deprecated
class GluttonBreadth : public Algorithm
{
    public:

        GluttonBreadth(Inputs *inputs);


    private:
    
        virtual void find_appointments();
};

#endif // GLUTTONBREADTH_H
