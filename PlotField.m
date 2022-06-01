function PlotField(drone_pos, field)
    close all
    drone_colours = ['r','g','m'];
    % Draw signal sources
    figure

    [ss_rows,ss_cols] = find(field);
    hold on
    for ss_n = 1:length(ss_rows)
        scatter(ss_cols(ss_n), ss_rows(ss_n), 'bo')
    end
    
    % draw drones
    dr_x = drone_pos(:,1);
    dr_y = drone_pos(:,2);



    % draw ranges to drones
    for dr_num = 1:length(drone_pos)
        scatter(dr_x(dr_num), dr_y(dr_num), 100 , drone_colours(dr_num), 'h', 'LineWidth', 1)
        RSSI = SignalStrength(dr_x(dr_num), dr_y(dr_num), field);
        RSSI
        for i = 1:length(RSSI)
            drawCircle(dr_x(dr_num), dr_y(dr_num), RSSI(i), drone_colours(dr_num))
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
