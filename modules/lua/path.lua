-------------------------------------------------------------------------------
-- Module for portable path/directory operations.
-- Requires LuaFileSystem.
-------------------------------------------------------------------------------

local lfs = require "lfs"
local attrib = lfs.attributes

local substr = string.sub
local strGSub = string.gsub
local strMatch = string.match
local tblConcat = table.concat

local DIR_SEP = package.config:sub( 1, 1 )
local isWindows = ( DIR_SEP == '\\' )

-- Returns whether a path is (one of) the system's root path.
local function isRoot( path )
	if isWindows then
		return #path <= 3 and path:sub( 2, 1 ) == ':'
	else
		return path == '/'
	end
end

-- Returns whether a path is absolute (i.e. starts with a root path).
local function isAbs( path )
	if isWindows then
		return substr( path, 2, 1 ) == ':'
	else
		return substr( path, 1, 1 ) == '/'
	end
end

-- Returns whether path is a file.
local function isDir( path )
	return attrib( path, 'mode' ) == 'directory'
end

-- Returns whether a path is a directory.
local function isFile( path )
	return attrib( path, 'mode' ) == 'file'
end

-- Returns whether a path exists (be a dir, a file, or whatever).
local function exists( path )
	return attrib( path, 'mode' ) ~= nil
end

-- Returns a normalized path with the following transformations:
-- 	 * On Windows, converts directory separators to '/' (except for the root drive separator, i.e. "c:\").
--   * On UNIX, reduces "/./" to "/".
--	 * Reduces "/dir/../" to "/".
--   * Reduces "//" to "/".
--   * Removes trailing '/'.
local function normalize( path )
	-- add a temporary trailing slash to facilitate the transformations
	path = path .. '/'

	-- system-specific bits
	if isWindows then
		path = strGSub( path, '\\' , '/' )
		path = strGSub( path, '^(%a):/', '%1:\\' )
	else
		path = strGSub( path, '/%./', '/' )
	end

	-- all "//" to "/"
	path = strGSub( path, '//', '/' )

	-- all "/dir/../" to "/"
	path = strGSub( path, '/[^/]+/%.%./', '/' )

	-- remove trailing slash
	return substr( path, 1, -2 )
end

-- Joins one or more path components. Example: join( "/usr", "local", "lib" ) => "/usr/local/lib"
local function join( ... )
	return tblConcat( { ... }, '/' )
end

-- Splits the pathname 'path' into a pair, (head, tail) where tail is the last
-- pathname component and head is everything leading up to that. The tail part
-- will never contain a slash; if path ends in a slash, tail will be empty. If
-- there is no slash in path, head will be empty. If path is empty, both head
-- and tail are empty. Trailing slashes are stripped from head unless it is the
-- root. In all cases, join(head, tail) returns a path to the same location as path (but the strings may differ).
local function split( path )
	path = normalize( path )
	local head, tail = strMatch( path, "^(.*)/([^/]*)$" )
	return head, tail or path
end

-- Split the pathname path into a pair (root, ext) such that root + ext == path,
-- and ext is empty or begins with a period and contains at most one period.
-- Leading periods on the basename are ignored; splitExt('.cshrc') returns ('.cshrc', nil).
local function splitExt( path )
	local basePath, baseName = split( path )
	local base, ext = strMatch( baseName, "^(%.?.*)(%.[^%.]*)$" )
	base = base or baseName
	if basePath then
		base = basePath .. '/' .. base
	end
	return base, ext
end

-- Creates a directory hierarchy.
local function _makePath( path )
	if not path or isRoot( path ) or isDir( path ) then
		return
	end
	_makePath( path:match( '(.+)/[^/]+$' ) )
	if not lfs.mkdir( path ) then
		error( "error creating directory '" .. path .. "'" )
	end
end

local function makePath( path )
	_makePath( normalize( path ) )
end

local M = {
	isRoot = isRoot,
	isAbs = isAbs,
	isDir = isDir,
	isFile = isFile,
	exists = exists,
	normalize = normalize,
	join = join,
	split = split,
	splitExt = splitExt,
	makePath = makePath,
}

return M
