#include "stackl_debugger.h"

int main()
{
	stackl_debugger sdb( "syscalls" );
	while( true )
	{
		sdb.query_user( 4156, 300 );
	}
	return 0;
}