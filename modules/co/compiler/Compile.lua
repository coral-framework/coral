-------------------------------------------------------------------------------
-- Coral Compiler Command-Line Front End
-------------------------------------------------------------------------------

local debug = require "debug"
local cmdline = require "co.compiler.cmdline"

-- garbage collection is unnecessary for a compiler
collectgarbage( "stop" )

-------------------------------------------------------------------------------
-- Command-Line Handler Functions
-------------------------------------------------------------------------------

local outDir = nil
local moduleName = nil
local askedForJSON = false
local askedForList = false
local askedForVersion = false
local compilerModule = "co.compiler.v1"

local flags = {
  -- Aliases
  p = 'path',
  m = 'module',
  o = 'outdir',
  v = 'version',
  h = 'help',
}

function flags.path( flag, dirList )
  if not dirList then
    return nil, "missing comma-separated list of dirs (Coral repositories)"
  end
  co.addPath( dirList )
  return 1
end

function flags.module( flag, name )
  if not name then return nil, "missing module name" end
  moduleName = name
  return 1
end

function flags.json()
  askedForJSON = true
end

function flags.list()
  askedForList = true
end

function flags.outdir( flag, dir )
  if not dir then return nil, "missing output directory" end
  outDir = dir
  return 1
end

function flags.v2()
  compilerModule = "co.compiler.v2" -- switch to the v2 implementation
end

function flags.version()
  print( "Coral Compiler v" .. co.version .. " (" .. co.buildKey .. ")" )
  askedForVersion = true
end

function flags.help()
  flags.version()
  print [[
Usage: coralc [options] ... [-m module] [type] ...
Description:
  Generates mappings for the list of types passed as command-line arguments.
  If -m is specified, the compiler will generate source code for a module,
  and the passed list of types will be considered extra module dependencies.
  If --json is passed along with -m, the compiler will write everything it
  knows about a module to a JSON file, instead of generating source code.
Available options:
  -p path    List of repositories to add to the Coral path.
  -m module  Module for which the compiler should generate source code; or...
     --list  Print a list of the .cpp files that would be generated.
     --json  Write available module data to a JSON file.
  -o dir     Change the output dir for generated files (default: ./generated).
  -v         Show version information.]]
end

-------------------------------------------------------------------------------
-- Launcher Component
-------------------------------------------------------------------------------

local Component = co.Component( "co.compiler.Compile" )

function Component:main( args )
  if #args == 0 then
    flags.help()
    return 0
  end

  local types, errorString = cmdline.process( args, flags )
  if not types then
    print( errorString )
    return -2
  end

  if askedForVersion then
    return 0
  end

  if askedForJSON and askedForList then
    print( "Error: --json and --list are mutually exclusive options" )
    return -2
  end

  if not moduleName and ( askedForJSON or askedForList ) then
    print( "Error: please specify a module using -g modulename" )
    return -2
  end

  local compiler = require(compilerModule).new()
  if outDir then
    compiler.outDir = outDir
  end
  if moduleName then
    compiler.moduleName = moduleName
  end
  if askedForList then
    compiler.simulation = true
  else
    compiler.log = print
  end

  local ok, err = xpcall( function()
    for i = 1, #types do
      compiler:addType( types[i] )
    end

    if moduleName then
      if askedForJSON then
        compiler:generateJSON()
      else
        local moduleSources = compiler:generateModule()
        if askedForList then
          for i, sourceFile in ipairs( moduleSources ) do
            print( sourceFile )
          end
        end
      end
    else
      compiler:generateMappings()
    end
  end, debug.traceback )

  if not ok then
    print( "*** Error ***" )
    local exceptionType, exceptionMsg = co.getException( err )
    print( exceptionMsg )
    return -1
  end

  return 0
end
