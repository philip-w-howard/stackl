#include "cDecl.h"
#include "cTypeDecl.h"
#include "cVarDecl.h"
#include "cFuncDecl.h"

// FIX THIS:: this is ugly, but it gets used
//
cTypeDecl *cDecl::GetType()
{ 
    return dynamic_cast<cTypeDecl*>(this); 
}

cVarDecl *cDecl::GetVar()
{ 
    return dynamic_cast<cVarDecl*>(this); 
}

cFuncDecl *cDecl::GetFunc()
{ 
    return dynamic_cast<cFuncDecl*>(this); 
}
