local LuaScriptedA = co.Component "moduleB.LuaScriptedA"

function LuaScriptedA:getFoo()
	return "A's foo"
end

local LuaScriptedB = co.Component "moduleB.LuaScriptedB"

function LuaScriptedB:getFoo()
	return "B's foo"
end
