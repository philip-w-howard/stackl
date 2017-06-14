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
            string op = source.next()->first();

            if (op == "POPVARIND")
            {
                dest.Insert("POPVAR " + offset + "\n");
                count++;
            } 
            else if (op == "POPCVARIND")
            {
                dest.Insert("POPCVAR " + offset + "\n");
                count++;
            } 
            else
            {
                dest.Insert(source.next(current)->text());
                continue;
            }
        }
        else
        {
            dest.Insert(item->text());
        }
    }

    return count;
}
//*********************************************************
int process_times_1(SourceFile &source, SourceFile &dest)
{
    int count = 0;
    LineItem *item;
    size_t current;
    string offset;
    while (!source.EndOfFile())
    {
        current = source.current();
        item = source.next();

        if (item->first() == "PUSH" && item->second() == "1")
        {
            if (source.next()->first() != "TIMES")
            {
                dest.Insert(source.next(current)->text());
            }
            else
            {
                count++;
            }
        }
        else
        {
            dest.Insert(item->text());
        }
    }

    return count;
}
//*********************************************************
int process_plus_0(SourceFile &source, SourceFile &dest)
{
    int count = 0;
    LineItem *item;
    size_t current;
    string offset;
    while (!source.EndOfFile())
    {
        current = source.current();
        item = source.next();

        if (item->first() == "PUSH" && item->second() == "0")
        {
            if (source.next()->first() != "PLUS")
            {
                dest.Insert(source.next(current)->text());
            }
            else
            {
                count++;
            }
        }
        else
        {
            dest.Insert(item->text());
        }
    }

    return count;
}
//*********************************************************
int process_const(SourceFile &source, SourceFile &dest)
{
    int count = 0;
    LineItem *item1;
    LineItem *item2;
    size_t current;
    string value1;
    string value2;
    string op;
    int final_value;

    while (!source.EndOfFile())
    {
        current = source.current();
        item1 = source.next();
        value1 = item1->second();

        if (item1->first() == "PUSH" && value1[0] != '@') 
        {
            item2 = source.next();
            value2 = item2->second();
            if (item2->first() != "PUSH" && value2[0] != '@')
            {
                dest.Insert(source.next(current)->text());
                continue;
            }

            op = source.next()->first();
            if (op == "PLUS")
            {
                final_value = stoi(value1) + stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "MINUS")
            {
                final_value = stoi(value1) - stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "TIMES")
            {
                final_value = stoi(value1) * stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "DIVIDE")
            {
                final_value = stoi(value1) / stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "MOD")
            {
                final_value = stoi(value1) % stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "EQ")
            {
                final_value = (stoi(value1) == stoi(value2));
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "NE")
            {
                final_value = stoi(value1) != stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "GT")
            {
                final_value = stoi(value1) > stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "LT")
            {
                final_value = stoi(value1) < stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "GE")
            {
                final_value = stoi(value1) >= stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "LE")
            {
                final_value = stoi(value1) <= stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "AND")
            {
                final_value = stoi(value1) && stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "OR")
            {
                final_value = stoi(value1) || stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "BAND")
            {
                final_value = stoi(value1) & stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "BOR")
            {
                final_value = stoi(value1) | stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "BXOR")
            {
                final_value = stoi(value1) ^ stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "SHIFTL")
            {
                final_value = stoi(value1) << stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else if (op == "SHIFTR")
            {
                final_value = stoi(value1) >> stoi(value2);
                dest.Insert("PUSH " + std::to_string(final_value) + "\n");
                count++;
            }
            else
            {
                dest.Insert(source.next(current)->text());
            }
        }
        else
        {
            dest.Insert(item1->text());
        }
    }

    return count;
}

//*********************************************************
int process_unary_const(SourceFile &source, SourceFile &dest)
{
    int count = 0;
    LineItem *item1;
    size_t current;
    string value1;
    string op;
    int final_value;

    while (!source.EndOfFile())
    {
        current = source.current();
        item1 = source.next();
        value1 = item1->second();

        if (item1->first() == "PUSH" && value1[0] != '@') 
        {
            op = source.next()->first();
            if (op == "COMP")
            {
                final_value = ~stoi(value1);
                dest.Insert("PUSH " + std::to_string(final_value));
                count++;
            }
            else if (op == "NOT")
            {
                final_value = !stoi(value1);
                dest.Insert("PUSH " + std::to_string(final_value));
                count++;
            }
            else
            {
                dest.Insert(source.next(current)->text());
            }
        }
        else
        {
            dest.Insert(item1->text());
        }
    }

    return count;
}

