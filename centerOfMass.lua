-- Function to set custom mass for a part
function setCustomWeight(part, mass)
	local volume = part.Size.X * part.Size.Y * part.Size.Z
	local density = mass / volume
	part.CustomPhysicalProperties = PhysicalProperties.new(density, part.Elasticity, part.Friction)
end

-- Function to calculate center of mass for the hovercraft (inside the folder)
function calculateCenterOfMass(folder)
	local totalMass = 0
	local weightedSum = Vector3.new(0, 0, 0)

	-- List of part names and their respective desired weights and positions
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

	-- Loop through parts in the "Hovercraft" folder and assign weights
	for _, partInfo in pairs(parts) do
		local part = folder:FindFirstChild(partInfo.name)
		if part then
			-- Set custom weight
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
		return nil -- In case no parts found or no mass
	end
end

-- Example usage: Calculate and print center of mass for the hovercraft in the folder
local hovercraftFolder = workspace.Hovercraft  -- The folder containing your parts
local com = calculateCenterOfMass(hovercraftFolder)
if com then
	print("Center of Mass: " .. tostring(com))  -- Print CG coordinates in Output window

	-- Create a small part at the center of mass to visualize it
	local comMarker = Instance.new("Part")
	comMarker.Size = Vector3.new(1, 1, 1)  -- A small part to represent the center of mass
	comMarker.Position = com  -- Set the position to the calculated center of mass
	comMarker.Anchored = true  -- Make sure it stays in place
	comMarker.Material = Enum.Material.Neon  -- Set the material to Neon
	comMarker.BrickColor = BrickColor.new("White")  -- Set the color to White
	comMarker.Parent = workspace  -- Add it to the workspace so it appears in the game

else
	print("No parts found in the Hovercraft folder.")
end
