-------------------------------------------------------------------------------
-- Coral Compiler Command-Line Front End
-------------------------------------------------------------------------------

local debug = require "debug"
local cmdline = require "co.compiler.cmdline"
local compiler = require( "co.compiler.v1" ).new()

-- garbage collection is unnecessary for a compiler
collectgarbage( "stop" )

-------------------------------------------------------------------------------
-- Command-Line Handler Functions
-------------------------------------------------------------------------------

local moduleName = nil
local askedForJSON = false
local askedForList = false
local askedForVersion = false

local flags = {
  -- Aliases
  g = 'generate',
  p = 'path',
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

function flags.generate( flag, name )
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
  compiler.outDir = dir
  return 1
end

function flags.version()
  print( "Coral Compiler v" .. co.version .. " (" .. co.buildKey .. ")" )
  askedForVersion = true
end

function flags.help()
  flags.version()
  print [[
Usage: coralc [options] [-g MODULE] [TYPE1] [TYPE2] ...
Description:
  Generates mappings for the list of types passed as command-line arguments.
  If -g is specified, the compiler will generate source code for a module,
  and the passed list of types will be considered extra module dependencies.
  If --json is passed along with -g, the compiler will write everything it
  knows about a module to a JSON file, instead of generating source code.
Available options:
  -p, --path EXTRA,DIRS  Add a list of repositories to the Coral path.
  -g, --generate MODULE  Generate source code for the specified module.
      --list             List module .cpp files without creating any file.
      --json             Generate a module JSON file instead of source code.
  -o, --outdir DIR       Output dir for generated files (default: ./generated).
  -v, --version          Show version information.]]
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
        compiler:generateJSON( moduleName )
      else
        local moduleSources = compiler:generateModule( moduleName )
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
