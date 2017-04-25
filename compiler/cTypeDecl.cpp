#include "cTypeDecl.h"
#include "cArrayType.h"
#include "cPointerType.h"

extern int Do_Type_Checks;

bool cTypeDecl::IsCompatibleWith(cTypeDecl *left, cTypeDecl *right)
{
    if (!Do_Type_Checks) return true;

    if (left == right) return true;
    if (left->IsInt() && right->IsInt() && left->Size() > right->Size())
        return true;
    if (left->IsPointer() && right->IsInt()) return true;
    if (left->IsPointer() && 
            (right->IsPointer() || right->GetType()->IsFunc()))
    {
        cPointerType *ptr = dynamic_cast<cPointerType*>(left);
        if (ptr == nullptr) 
        {
            left->ThrowSemanticError("Fatal compiler error: IsPointer "
                    "doesn't have a pointer base type");
            return false;
        }

        // any pointer promotes to void
        if (ptr->ParentType()->IsVoid()) return true;
    }

    if (left->IsPointer() && right->IsArray())
    {
        cPointerType *ptr = dynamic_cast<cPointerType*>(left);
        if (ptr == nullptr) 
        {
            left->ThrowSemanticError("Fatal compiler error: IsPointer "
                    "doesn't have a pointer base type");
            return false;
        }

        cArrayType *array = dynamic_cast<cArrayType*>(right);
        if (array == nullptr) 
        {
            right->ThrowSemanticError("Fatal compiler error: IsArray "
                    "doesn't have an array base type");
            return false;
        }

        if (ptr->ParentType() == array->ParentType()) return true;
    }
    return false;
}
