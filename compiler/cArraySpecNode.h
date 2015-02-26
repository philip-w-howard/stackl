#pragma once
//*******************************************************
// Purpose: Class for the specification of the dimensions of an array
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <assert.h>
#include <string>

#include "cAstNode.h"

class cArraySpecNode: public cAstNode
{
  public:
    cArraySpecNode(int value) : cAstNode()
    {
        mDepth = 0;
        mSizes[mDepth] = value;
        mDepth++;

        assert(mDepth <= MAX_INDICES);
    }

    // return the number of dimensions
    int GetDepth()
    { return mDepth; }

    // add another dimention to the array
    void AddNode(int value)
    {
        mSizes[mDepth] = value;
        mDepth++;

        assert(mDepth <= MAX_INDICES);
    }

    virtual std::string toString()
    {
        std::string result("(ARRAYSPEC:");
        for (int ii=0; ii<mDepth; ii++)
        {
            result += " " + std::to_string(mSizes[ii]);
        }
        result += ")";

        return result;
    }

    // return the total number of elements in the array
    virtual int NumElements() 
    {
        int size = mSizes[0];
        for (int ii=1; ii<mDepth; ii++)
        {
            size *= mSizes[ii];
        }
        return size;
    }
  protected:
    // KLUDGE: this should probably be a list
    static const int MAX_INDICES = 20;  
    int mDepth;                     // number of dimentions
    int mSizes[MAX_INDICES];        // size of each dimension
};

