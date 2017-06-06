#include <string>
using std::string;

#include "lineitem.h"
#include "sourcefile.h"

int process_popvar(SourceFile &source, SourceFile &dest)
{
    int count = 0;
    LineItem *item;
    size_t current;
    string offset;
    while (!source.EndOfFile())
    {
        current = source.current();
        item = source.next();

        if (item->first() == "PUSH")
        {
            offset = item->second();

            if (source.next()->first() != "PUSHFP")
            {
                dest.Insert(source.next(current)->text());
                continue;
            }
            if (source.next()->first() != "PLUS")
            {
                dest.Insert(source.next(current)->text());
                continue;
            }
            if (source.next()->first() != "POPVARIND")
            {
                dest.Insert(source.next(current)->text());
                continue;
            }

            dest.Insert("POPVAR " + offset + "\n");
            count++;
        }
        else
        {
            dest.Insert(item->text());
        }
    }

    return count;
}



