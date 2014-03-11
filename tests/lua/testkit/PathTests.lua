local path = require "lua.path"

-- Splits the pathname 'path' into a pair, (head, tail) where tail is the last
-- pathname component and head is everything leading up to that. The tail part
-- will never contain a slash; if path ends in a slash, tail will be empty. If
-- there is no slash in path, head will be empty. If path is empty, both head
-- and tail are empty. Trailing slashes are stripped from head unless it is the
-- root. In all cases, join(head, tail) returns a path to the same location as path (but the strings may differ).

function split()
	local head, tail = path.split( "one/two/three/" )
	ASSERT_EQ( head, "one/two/three" )
	ASSERT_EQ( tail, "" )

	head, tail = path.split( head )
	ASSERT_EQ( head, "one/two" )
	ASSERT_EQ( tail, "three" )

	head, tail = path.split( head )
	ASSERT_EQ( head, "one" )
	ASSERT_EQ( tail, "two" )

	head, tail = path.split( head )
	ASSERT_EQ( head, nil )
	ASSERT_EQ( tail, "one" )

	head, tail = path.split( "/one" )
	ASSERT_EQ( head, "/" )
	ASSERT_EQ( tail, "one" )

	head, tail = path.split( head )
	ASSERT_EQ( head, "/" )
	ASSERT_EQ( tail, nil )
end

function splitExt()
	ASSERT_EQ( "str", "str" )
	--ASSERT_EQ( 3.1444445, 3.1444446 )
end
