function com = calculateCenterOfMass()
    parts = 
    {
        'arduinoBoard',         118.7,  [0, 5, 4];   
        'batteryOne',           36,     [-3, 12, 6];  
        'batteryTwo',           36,     [3, 12, 6];   
        'hovercraftBase',       0,      [0, 0, 1];    
        'iMU',                  7.4,    [0, 15.5, 4];  
        'infraredSensor',       3.53,   [0, -16, 5];  
        'leftUltrasonicSensor', 9.53,   [-11, -7, 6]; 
        'rightUltrasonicSensor',9.53,   [11, -7, 6];  
        'thrustFan',            175.36, [0, -14, 12]; 
        'liftFan',              199.94, [0, 0, 3.75]; 
        'servoMotor',           44.14,  [0, -14, 4.25]; 
    };

    totalMass = 0;
    weightedSum = [0, 0, 0];

    for i = 1:size(parts, 1)
        mass = parts{i, 2};
        position = parts{i, 3}; 
        totalMass = totalMass + mass;
        weightedSum = weightedSum + mass .* position;
    end

    if totalMass > 0
        com = weightedSum / totalMass;
        fprintf('Center of Mass: [%.2f, %.2f, %.2f]\n', com(1), com(2), com(3));
        figure;
        hold on;
        grid on;
        xlabel('X');
        ylabel('Z');
        zlabel('Y');
        title('Hovercraft Center of Mass Visualization');

        for i = 1:size(parts,1)
            pos = parts{i,3};
            scatter3(pos(1), pos(2), pos(3), 50, 'b', 'filled');
            text(pos(1), pos(2), pos(3), parts{i,1}, 'VerticalAlignment','bottom','HorizontalAlignment','right');
        end

        scatter3(com(1), com(2), com(3), 100, 'r', 'filled');
        text(com(1), com(2), com(3), 'Center of Mass', 'FontWeight','bold', 'Color','r');

        hold off;
    else
        com = [];
        disp('No valid parts with mass found.');
    end
end
