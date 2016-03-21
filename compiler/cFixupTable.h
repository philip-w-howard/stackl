#pragma once

#include <fstream>
#include <unordered_map>
#include <list>
#include <string>

#include "cAstNode.h"
#include "cCodeGen.h"

class cFixupTable
{
  public:
    static const std::string GlobalLabel;

    cFixupTable()
    {}

    void AddLabelRef(std::string label, int loc)
    {
        std::pair<std::string,int> mypair (label, loc);
        mSource.insert(mypair);
    }
    void AddLabelValue(std::string label, int loc)
    {
        mDest[label] = loc;
    }

    void FixupOutput(std::ofstream &output)
    {
        std::string name;

        std::list<string_item>::iterator str_it;
        for (str_it=mStringList.begin(); str_it!=mStringList.end(); str_it++)
        {
            SetLabelValue((*str_it).mLabel);
            EmitActualString( (*str_it).mString );
        }

        SetLabelValue(GlobalLabel);
        std::list<string_item>::iterator glb_it;
        for (glb_it=mGlobalList.begin(); glb_it!=mGlobalList.end(); glb_it++)
        {
            EmitActualGlobal( (*glb_it).mLabel, (*glb_it).mSize );
        }

        std::unordered_multimap<std::string, int>::iterator it;
        std::unordered_map<std::string, int>::const_iterator found;
        for (it=mSource.begin(); it!=mSource.end(); it++)
        {
            name = (*it).first;

            found = mDest.find(name);
            if (found != mDest.end())
            {
                output << "C " << (*it).first << " " 
                    << (*found).first << "\n";
                output << "F " << (*it).second << " " 
                    << (*found).second << "\n";
            } else {
                fatal_error("no destination for " + name + " in fixup table");
            }
        }
    }

    void FixupOutput(cCodeGen *coder)
    {
        std::string name;

        std::list<string_item>::iterator str_it;
        for (str_it=mStringList.begin(); str_it!=mStringList.end(); str_it++)
        {
            coder->SetLabelValue((*str_it).mLabel);
            coder->EmitActualString( (*str_it).mString );
        }

        coder->SetLabelValue(GlobalLabel);
        std::list<string_item>::iterator glb_it;
        for (glb_it=mGlobalList.begin(); glb_it!=mGlobalList.end(); glb_it++)
        {
            coder->EmitActualGlobal( (*glb_it).mLabel, (*glb_it).mSize );
        }

        std::unordered_multimap<std::string, int>::iterator it;
        std::unordered_map<std::string, int>::const_iterator found;
        for (it=mSource.begin(); it!=mSource.end(); it++)
        {
            name = (*it).first;

            found = mDest.find(name);
            if (found != mDest.end())
            {
                coder->EmitComment( (*it).first + " " +  (*found).first);
                coder->EmitFixup( (*it).second, (*found).second);
            } else {
                fatal_error("no destination for " + name + " in fixup table");
            }
        }
    }

    void FixupAddString(std::string str, std::string label)
    {
        string_item item;
        item.mString = str;
        item.mLabel = label;
        mStringList.push_back(item);
    }

    void FixupAddGlobal(std::string label, int size)
    {
        string_item item;
        item.mLabel = label;
        item.mSize = size;
        mGlobalList.push_back(item);
    }

  protected:
    class string_item
    {
      public:
        std::string mString;
        std::string mLabel;
        int mSize;
    };

    std::unordered_multimap<std::string, int> mSource;
    std::unordered_map<std::string, int> mDest;
    std::list<string_item> mStringList;
    std::list<string_item> mGlobalList;
};
