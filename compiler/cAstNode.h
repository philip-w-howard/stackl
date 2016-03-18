#pragma once
//**************************************
// cAstNode.h
//
// pure virtual base class for all AST nodes
//
// Author: Phil Howard 
// phil.howard@oit.edu
//
// Date: Mar. 14, 2016
//

#include <string>
#include <vector>
#include <cassert>
using std::string;
using std::vector;

#include "cVisitor.h"

extern int yylineno;

// called to throw a semantic error
extern int semantic_error(std::string error);

void fatal_error(std::string msg);
class cAstNode
{
    public:
        typedef vector<cAstNode*>::iterator iterator;

        cAstNode() 
        {
            mSemanticError = false;
            mLineNumber = yylineno;
        }

        // return the source line number from when the node was created
        int LineNumber() { return mLineNumber; }

        void AddChild(cAstNode *child)
        {
            m_children.push_back(child);
        }

        void SetChild(int index, cAstNode *child)
        {
            m_children[index] = child;
        }

        iterator FirstChild()
        {
            return m_children.begin();
        }

        iterator LastChild()
        {
            return m_children.end();
        }

        bool HasChildren()      { return !m_children.empty(); }

        int NumChildren()       { return (int)m_children.size(); }
        cAstNode* GetChild(int child)
        {
            if (child >= (int)m_children.size()) return nullptr;
            return m_children[child];
        }

        virtual string AttributesToString()   { return string(""); }
        virtual void Visit(cVisitor *visitor) = 0;

        // return true if a semantic error was detected in this node
        virtual bool HasSemanticError() { return mSemanticError; }

         virtual int ComputeOffsets(int base) { return base; }
         virtual void GenerateCode() {}
    protected:
        vector<cAstNode *> m_children;  // list of statements
        bool mSemanticError;            // This node contains a semantic error
        int mLineNumber;                // line number of source when 
                                        // node was created

        void ThrowSemanticError(std::string msg)
        {
            semantic_error(msg);
            mSemanticError = true;
        }


};
