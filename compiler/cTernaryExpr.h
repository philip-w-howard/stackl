#pragma once

#include "cExpr.h"

class cTernaryExpr : public cExpr
{
    public:
        cTernaryExpr(cExpr *condition, cExpr *ifTrue, cExpr* ifFalse) : cExpr()
        {
            AddChild(condition);
            AddChild(ifTrue);
            AddChild(ifFalse);

            cTypeDecl *trueType = ifTrue->GetType();
            cTypeDecl *falseType = ifFalse->GetType();
            if (!cTypeDecl::IsCompatibleWith(trueType, falseType))
            {
                ThrowSemanticError("Incompatible types in ternary operation");
            }
        }

        virtual bool IsConst()      { return GetCondition()->IsConst() && GetTrue()->IsConst() && GetFalse()->IsConst(); }
        virtual int ConstValue()
        { 
            if (!IsConst()) return 0;
            return GetCondition()->ConstValue() 
                ? GetTrue()->ConstValue() 
                : GetFalse()->ConstValue();
        }

        virtual cTypeDecl* GetType()
        {
            cTypeDecl *trueType = GetTrue()->GetType();
            cTypeDecl *falseType = GetFalse()->GetType();
            return trueType->Size() > falseType->Size() ? trueType : falseType;
        }

        cExpr* GetCondition()   { return (cExpr*)GetChild(0); }
        cExpr* GetTrue()        { return (cExpr*)GetChild(1); }
        cExpr* GetFalse()       { return (cExpr*)GetChild(2); }

        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};