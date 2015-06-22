#pragma once
//*******************************************************
// Purpose: Utility functions for working with lists
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************

#include <list>

using std::list;

//*********************************************
// Get a specific item out of a list
template <class Type>
Type ListGetItem(list<Type> *mList, int index)
{
    for (typename list<Type>::iterator it = mList->begin(); 
            it != mList->end(); it++)
    {
        if (index-- == 0) return *it;
    }

    return NULL;
}
//*********************************************
// Perform the toSting function on a list of AST nodes
// lineFeeds will place a \n between items if set to true
template <class Type>
std::string ListToString(list<Type> *mList, bool lineFeeds)
{
    std::string result = "";

    // Need to do the first one separate to avoid and extra " " at the end
    typename list<Type>::iterator it = mList->begin(); 
    result += (*it)->toString();
    if (lineFeeds) result += "\n";

    for (it++; it != mList->end(); it++)
    {
        if (!lineFeeds) result += " ";
        result += (*it)->toString();
        if (lineFeeds) result += "\n";
    }
    return result;
}
//*********************************************
// Perform the toSting function on a list of AST nodes
// lineFeeds will place a \n between items if set to true
template <class Type>
int ListComputeOffsets(list<Type> *mList, int offset)
{
    typename list<Type>::iterator it; 

    for (it=mList->begin(); it != mList->end(); it++)
    {
        offset = (*it)->ComputeOffsets(offset);
    }
    return offset;
}
