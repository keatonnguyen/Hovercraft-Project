-- function to set custom mass for each part of the hovercraft model
function setCustomWeight(part, mass)
	local volume = part.Size.X * part.Size.Y * part.Size.Z
	local density = mass / volume
	part.CustomPhysicalProperties = PhysicalProperties.new(density, part.Elasticity, part.Friction)
end

-- function to calculate center of mass for the hovercraft
function calculateCenterOfMass(folder)
	local totalMass = 0
	local weightedSum = Vector3.new(0, 0, 0)

	-- list of part names and their weights and positions relative to hovercraftBase
	local parts = 
	{
		{name = "arduinoBoard", mass = 118.7, position = Vector3.new(0, 4, 5)},
		{name = "batteryOne", mass = 36, position = Vector3.new(7, 3, 15.75)},
		{name = "batteryTwo", mass = 36, position = Vector3.new(-7, 3, 15.75)},
		{name = "hovercraftBase", mass = 50, position = Vector3.new(0, 1, 0)}, 
		{name = "iMU", mass = 7.4, position = Vector3.new(0, 4, 15.5)},
		{name = "infraredSensor", mass = 3.53, position = Vector3.new(0, 5, -20)},
		{name = "leftUltrasonicSensor", mass = 9.53, position = Vector3.new(-11, 6, -7)},
		{name = "rightUltrasonicSensor", mass = 9.53, position = Vector3.new(11, 6, -7)},
		{name = "thrustFan", mass = 175.36, position = Vector3.new(0, 12, -17)},
		{name = "liftFan", mass = 199.94, position = Vector3.new(0, 3.75, -8)},
		{name = "servoMotor", mass = 44.14, position = Vector3.new(0, 4.25, -17)}
	}

	-- loop through parts in the "Hovercraft" folder and set weights
	for _, partInfo in pairs(parts) do
		local part = folder:FindFirstChild(partInfo.name)
		if part then
			setCustomWeight(part, partInfo.mass)
			totalMass = totalMass + partInfo.mass
			weightedSum = weightedSum + (partInfo.position * partInfo.mass)
		end
	end

	-- calculate center of mass
	if totalMass > 0 then
		local centerOfMass = weightedSum / totalMass
		return centerOfMass
	else
		return nil
	end
end

-- print center of mass for the hovercraft
local hovercraftFolder = workspace.Hovercraft
local com = calculateCenterOfMass(hovercraftFolder)
if com then
	print("Center of Mass: " .. tostring(com))

	-- create a small part at the center of mass to visualize it
	local comMarker = Instance.new("Part")
	comMarker.Size = Vector3.new(1, 1, 1)  
	comMarker.Position = com  
	comMarker.Anchored = true
	comMarker.Material = Enum.Material.Neon 
	comMarker.BrickColor = BrickColor.new("White")  -- visibility thing
	comMarker.Parent = workspace
else
	print("No parts found in the Hovercraft folder.")
end

