#pragma once
//*******************************************************
// Purpose: Class for short circuiting binary expressions
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <string>

#include "cExpr.h"
#include "cBinaryExpr.h"

class cShortCircuitExpr : public cBinaryExpr
{
  public:
    cShortCircuitExpr(cExpr *left, int op, cExpr *right, bool shortOnTrue) 
        : cBinaryExpr(left, op, right)
    {
        mShortOnTrue = shortOnTrue;
    }

    bool ShortOnTrue()  { return mShortOnTrue; }

    virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

  protected:
    bool mShortOnTrue;      // short circuit if left op is true
                            // NOTE: also uses members from cBinaryExpr
};

