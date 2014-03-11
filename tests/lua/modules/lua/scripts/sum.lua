return function( ... )
	local sum = 0
	for i, v in ipairs( { ... } ) do
		sum = sum + v
	end
	return sum
end
