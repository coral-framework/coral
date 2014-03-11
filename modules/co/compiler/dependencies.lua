--[[

	This module returns a factory function for a "dependency walker": an iterator
	that performs a breadth-first traversal on the dependency graph of a type set.

	A dependency walker is created with an initial set of types, all considered to be
	at "distance zero". All types directly reachable from the initial set (across a
	single "dependecy edge") are at distance one; those only reachable through two
	"dependency edges" are at distance two, and so forth. A maxDistance can be specified
	as a second parameter to the factory function, to exclude types at greater distances.

	iterator = dependencies( types[, maxDistance] )

--]]

local isBasicType = {
	TK_ANY		= true,
	TK_BOOL		= true,
	TK_INT8		= true,
	TK_UINT8	= true,
	TK_INT16	= true,
	TK_UINT16	= true,
	TK_INT32	= true,
	TK_UINT32	= true,
	TK_FLOAT	= true,
	TK_DOUBLE	= true,
	TK_STRING	= true,
}

local enqueueDependencies = {}

-- only compound types have dependencies
function enqueueDependencies.TK_ENUM( enqueue, type, nextDistance ) end
function enqueueDependencies.TK_EXCEPTION( enqueue, type, nextDistance ) end

local function enqueueRecordType( enqueue, type, nextDistance )
	for i, field in ipairs( type.fields ) do
		enqueue( field.type, nextDistance )
	end
end

local function enqueueClassType( enqueue, type, nextDistance )
	enqueueRecordType( enqueue, type, nextDistance )
	for i, method in ipairs( type.methods ) do
		local returnType = method.returnType
		if returnType then enqueue( returnType, nextDistance ) end
		for i, param in ipairs( method.parameters ) do
			enqueue( param.type, nextDistance )
		end
		for i, exception in ipairs( method.exceptions ) do
			enqueue( exception, nextDistance )
		end
	end
end

function enqueueDependencies.TK_STRUCT( enqueue, type, nextDistance )
	enqueueRecordType( enqueue, type, nextDistance )
end

function enqueueDependencies.TK_NATIVECLASS( enqueue, type, nextDistance )
	enqueueClassType( enqueue, type, nextDistance )
end

function enqueueDependencies.TK_INTERFACE( enqueue, type, nextDistance )
	if type.baseType then
		enqueue( type.baseType, nextDistance )
	end
	enqueueClassType( enqueue, type, nextDistance )
end

function enqueueDependencies.TK_COMPONENT( enqueue, type, nextDistance )
	for i, itf in ipairs( type.ports ) do
		enqueue( itf.type, nextDistance )
	end
end

-- Coroutine-based iterator.
local function walkDependencies( types, maxDistance )
	-- map of types that have already been visited, or are just one 'distance' away;
	local visited = {} -- visited[type] == distance

	-- queue of types to be returned by the coroutine
	local queue, qFront, qBack = {}, 1, 0

	-- Flag a type as visited, and queue it for iteration.
	local function enqueue( type, distance )
		local kind = type.kind
		-- If the passed type is a co.IArray, the enqueued type is the array's elementType.
		if kind == 'TK_ARRAY' then
			type = type.elementType
			kind = type.kind
		end
		-- A type is ignored if it's a basic type, or if it was already visited.
		if not isBasicType[kind] and not visited[type] then
			visited[type] = distance
			qBack = qBack + 1
			queue[qBack] = type
		end
	end

	-- enqueue the initial set of types, at distance zero.
	if type( next( types ) ) == 'number' then
		-- assume 'types' is an array of types
		for i, t in ipairs( types ) do
			enqueue( t, 0 )
		end
	else
		-- assume 'types' is a map of types to their distances
		for t, d in pairs( types ) do
			enqueue( t, d )
		end
	end

	while qFront <= qBack do
		local type = queue[qFront]
		local distance = visited[type]
		coroutine.yield( type, distance )
		if distance < maxDistance then
			enqueueDependencies[type.kind]( enqueue, type, distance + 1 )
		end
		qFront = qFront + 1
	end
end

-- Factory: 'types' may be either an array of types, or a map of types to their distances.
local function dependencies( types, maxDistance )
	assert( type( types ) == 'table' )
	if not maxDistance then maxDistance = math.huge end
	return coroutine.wrap( function() walkDependencies( types, maxDistance ) end )
end

return dependencies
