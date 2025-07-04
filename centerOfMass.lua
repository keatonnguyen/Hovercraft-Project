-- set weight
function setCustomWeight(part, mass)
	local volume = part.Size.X * part.Size.Y * part.Size.Z
	local density = mass / volume
	part.CustomPhysicalProperties = PhysicalProperties.new(density, part.Elasticity, part.Friction)
end

-- main function
function calculateCenterOfMass(folder)
	local totalMass = 0
	local weightedSum = Vector3.new(0, 0, 0)

	-- hovercraft parts' weight & position
	local parts = {
		{name = "arduinoBoard", mass = 118.7, position = Vector3.new(0, 4, -12)},
		{name = "batteryOne", mass = 36, position = Vector3.new(2, 6, -12)},
		{name = "batteryTwo", mass = 36, position = Vector3.new(-2, 6, -12)},
		{name = "hovercraftBase", mass = 0, position = Vector3.new(0, 1, 0)},  -- Assuming default mass for hovercraftBase
		{name = "iMU", mass = 7.4, position = Vector3.new(0, 4.53, -18)},
		{name = "infraredSensor", mass = 3.53, position = Vector3.new(0, 5, 16)},
		{name = "leftUltrasonicSensor", mass = 9.53, position = Vector3.new(15, 6, 7)},
		{name = "rightUltrasonicSensor", mass = 9.53, position = Vector3.new(-15, 6, 7)},
		{name = "thrustFan", mass = 175.36, position = Vector3.new},
		{name = "liftFan", mass = 199.94, position = Vector3.new(0, 3.75, 0)},
		{name = "servoMotor", mass = 44.14, position = Vector3.new}
	}

	-- assign weights to each parts
	for _, partInfo in pairs(parts) do
		local part = folder:FindFirstChild(partInfo.name)
		if part then
			setCustomWeight(part, partInfo.mass)

			-- Update total mass and weighted sum
			totalMass = totalMass + partInfo.mass
			weightedSum = weightedSum + (partInfo.position * partInfo.mass)
		end
	end

	-- Calculate center of mass
	if totalMass > 0 then
		local centerOfMass = weightedSum / totalMass
		return centerOfMass
	else
		return nil
	end
end

--Calculate and print center of mass for the hovercraft
local hovercraftFolder = workspace.Hovercraft
local com = calculateCenterOfMass(hovercraftFolder)
if com then
	print("Center of Mass: " .. tostring(com))  -- Print CG coordinates

	-- create a small visible marker
	local comMarker = Instance.new("Part")
	comMarker.Size = Vector3.new(1, 1, 1) 
	comMarker.Position = com 
	comMarker.Anchored = true 
	comMarker.Material = Enum.Material.Neon 
	comMarker.BrickColor = BrickColor.new("White") 
	comMarker.Parent = workspace 

else
	print("No parts found in the Hovercraft folder.")
end
