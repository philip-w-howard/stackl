#pragma once
#include "variable.h"
#include "operation.h"

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <algorithm>
using std::find;
#include "operation.h"
#include "../machine.h"

class expression
{
public:
    expression( const string& exp );
    string eval( Machine_State* cpu );
private:
    string reduce( Machine_State* cpu, string val, size_t open_paren, size_t close_paren );
protected:

    class op_precedence
    {
    public:
        op_precedence( const vector<operation>& ops ): _ops( ops ) {}
        bool in( const operation& op ) const { return find( _ops.begin(), _ops.end(), op ) != _ops.end(); }
    private:
        vector<operation> _ops;
    };

    class op_tree
    {
    protected:
        struct op_node
        {
            op_node* left;
            op_node* right;
            string val;
            operation op;
        };

    public:
        op_tree( const string& txt, const vector<op_precedence>& heirarchy ): _heirarchy( heirarchy )
        {
            insert( _head, txt );
        }

        void insert( op_node*& loc, const string& txt )
        {
            size_t idx = find_op_idx( txt, _heirarchy.size() - 1 );
            if( idx == 0 || idx == string::npos )
            {
                loc = new op_node{ nullptr, nullptr, txt, '\0' };
            }
            else
            {
                string left = txt.substr( 0, idx );
                string right = txt.substr( idx + 1 );
                loc = new op_node{ nullptr, nullptr, "", txt[idx] };
                insert( loc->left, left );
                insert( loc->right, right );
            }
        }

        string eval( Machine_State* cpu )
        {
            return eval( cpu, _head );
        }

        string eval( Machine_State* cpu, op_node* node )
        {
            if( node->op == '\0' )
                return node->val;
            return node->val = std::to_string( node->op.calc( cpu, eval( cpu, node->left ), eval( cpu, node->right ) ) );
        }

        size_t find_op_idx( const string& txt, int32_t precedence )
        {
            if( precedence < 0 )
                return string::npos;

            bool found_non_op = false;
            for( size_t i = 0; i < txt.length(); ++i )
            {
                if( found_non_op && _heirarchy[precedence].in( txt[i] ) )
                    return i;
                else found_non_op = true;
            }

            return find_op_idx( txt, precedence - 1 );
        }

        void del( op_node* node )
        {
            if( node )
            {
                del( node->left );
                del( node->right );
                delete node;
            }
        }

        ~op_tree()
        {
            del( _head );
        }
    private:
        op_node* _head = nullptr;
        vector<op_precedence> _heirarchy;
    };

    string _exp;
    const vector<op_precedence> _heirarchy = {
        op_precedence( { '^' } ),
        op_precedence( { '%' } ),
        op_precedence( { '*', '/' } ),
        op_precedence( { '+', '-' } )
    };
};