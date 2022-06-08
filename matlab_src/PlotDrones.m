function PlotDrones(file_path) 

    hold on;
    drone_measurements = readmatrix("../" + file_path, "NumHeaderLines", 1);
    
    for row  = 1:size(drone_measurements,1)
        drone_x = drone_measurements(row,1);
        drone_y = drone_measurements(row,2);
        scatter(drone_x, drone_y);
        cmap = hsv(size(drone_measurements,2)-2);
        for m = 3:2:size(drone_measurements,2)
            if(drone_measurements(row,m+1) > 0)
                drawCircle(drone_x, drone_y,drone_measurements(row,m+1), cmap(floor(m/2),:,:))
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