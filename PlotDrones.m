function PlotDrones(drone_pos)  
% draw ranges to drones
    for dr_num = 1:length(drone_pos)
        scatter(dr_x(dr_num), dr_y(dr_num), 100 , drone_colours(dr_num), 'h', 'LineWidth', 1)
        RSSI = SignalStrength(dr_x(dr_num), dr_y(dr_num), field);
        disp(RSSI)
        for i = 1:length(RSSI)
            if RSSI(i) < 100 
                drawCircle(dr_x(dr_num), dr_y(dr_num), RSSI(i), drone_colours(dr_num))
            end
            
        end
    end
    hold off
end


function drawCircle(x,y,radius, colour)
    n = 100;
    t = linspace(0,2*pi,n);
    x_points = x + radius*sin(t);
    y_points = y + radius*cos(t);
    line(x_points,y_points, 'Color', colour, 'LineWidth', 10/radius, 'LineStyle' ,'--')
end