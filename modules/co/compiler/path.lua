-------------------------------------------------------------------------------
-- Module for path and directory operations. Requires LuaFileSystem.
-------------------------------------------------------------------------------

local lfs = require "lfs"
local attrib = lfs.attributes
local currentDir = lfs.currentdir

local DIR_SEP = package.config:sub(1, 1)
local isWindows = (DIR_SEP == '\\')

-- Returns whether a path is (one of) the system's root path.
local function isRoot(path)
	if isWindows then
		return #path <= 3 and path:sub(2, 1) == ':'
	else
		return path == '/'
	end
end

-- Returns whether a path is absolute (i.e. starts with a root path).
local function isAbs(path)
	if isWindows then
		return path:sub(2, 1) == ':'
	else
		return path:sub(1, 1) == '/'
	end
end

-- Returns whether path is a file.
local function isDir( path )
	return attrib(path, 'mode') == 'directory'
end

-- Returns whether a path is a directory.
local function isFile(path)
	return attrib(path, 'mode') == 'file'
end

-- Returns whether a path exists (be it a dir, a file, or whatever).
local function exists(path)
	return attrib(path, 'mode') ~= nil
end

-- Returns a normalized path with the following transformations:
--   On Windows, converts dir separators to '/' (except for the root drive).
--   On UNIX, reduces "/./" to "/".
--   Reduces "/dir/../" to "/".
--   Reduces "//" to "/".
--   Removes trailing '/'.
local function normalize(path)
	-- add a temporary trailing slash to facilitate the transformations
	path = path .. '/'

	-- system-specific bits
	if isWindows then
		path = path:gsub('\\' , '/')
		path = path:gsub('^(%a):/', '%1:\\')
	else
		path = path:gsub('/%./', '/')
	end

	-- all "//" to "/"
	path = path:gsub('//', '/')

	-- all "/dir/../" to "/"
	path = path:gsub('/[^/]+/%.%./', '/')

	-- remove trailing slash
	return path:sub(1, -2)
end

-- Returns the absolute version of a path.
local function absolutize(path)
	if isAbs(path) then return path end
	return normalize(assert(currentDir()) .. '/' .. path)
end

-- Joins one or more path components.
-- Example: join( "/usr", "local", "lib" ) => "/usr/local/lib"
local function join(...)
	return table.concat({...}, '/')
end

-- Splits the pathname 'path' into a pair, (head, tail) where tail is the last
-- pathname component and head is everything leading up to that. The tail part
-- will never contain a slash; if path ends in a slash, tail will be empty. If
-- there is no slash in path, head will be empty. If path is empty, both head
-- and tail are empty. Trailing slashes are stripped from head unless it is the
-- root. In all cases, join(head, tail) returns a path to the same location as
-- path (but the strings may differ).
local function split(path)
	path = normalize( path )
	local head, tail = path:match("^(.*)/([^/]*)$")
	return head, tail or path
end

-- Splits the pathname path into a pair (root, ext) such that root+ext=path,
-- and ext is empty or begins with a period and contains at most one period.
-- Ignores leading dots in the basename; splitExt('.x') => ('.x', nil).
local function splitExt(path)
	local basePath, baseName = split(path)
	local base, ext = baseName:match("^(%.?.*)(%.[^%.]*)$" )
	base = base or baseName
	if basePath then
		base = basePath .. '/' .. base
	end
	return base, ext
end

-- Splits a fully qualified typeName into (moduleName, baseName)
local function splitType(typeName)
  return typeName:match("^(.-)%.?([^.]+)$")
end

-- Creates a directory hierarchy.
local function _makePath(path)
	if not path or isRoot(path) or isDir(path) then
		return
	end
	_makePath(path:match('(.+)/[^/]+$'))
	if not lfs.mkdir(path) then
		error("error creating directory '" .. path .. "'")
	end
end

local function makePath(path)
	_makePath(normalize(path))
end

local coralPath = {}
local function addPath(str)
	for repo in str:gmatch("([^,:;]+)[,:;]*") do
		repo = absolutize(repo)
		if not isDir(repo) then
			error("cannot add '" .. repo .. "' to the Coral path (not a dir)")
		end
		coralPath[#coralPath+1] = repo
	end
end
addPath(os.getenv('CORAL_PATH') or '')

-- Searches for a file within a module in the Coral path.
-- Returns the absolute path to the file, or nil if it doesn't exist.
local function findModuleFile(moduleName, fileName)
	local suffix = (moduleName:gsub('%.', '/') .. '/' .. fileName)
	for i, dir in ipairs(coralPath) do
		local path = dir .. '/' .. suffix
		if isFile(path) then
			return path
		end
	end
end

local M = {
	-- Filesystem Analysis
	isRoot = isRoot,
	isAbs = isAbs,
	isDir = isDir,
	isFile = isFile,
	exists = exists,
	-- Path Manipulation
	normalize = normalize,
	absolutize = absolutize,
	join = join,
	split = split,
	splitExt = splitExt,
	splitType = splitType,
	makePath = makePath,
	-- Coral Path Operations
	addPath = addPath,
	getPath = function() return coralPath end,
	findModuleFile = findModuleFile,
}

return M
