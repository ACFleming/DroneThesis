%% Reading in the file names
T = readtable("../logs/file_path_log.csv", "VariableNamingRule", "preserve", "Delimiter", ",");


%% Plotting the field
file_path = T.(1){1};

clf;
hFig = figure(1);
hFig.set("Name", "Field")

source_locations = readtable("../" + file_path);

% Draw signal sources

hold on
daspect([1 1 1])

axis([-10, source_locations.x(1)+10, -10, source_locations.y(1)+10])
rectangle('Position',[0 0 source_locations.x(1) source_locations.y(1)])

for i = 2:size(source_locations,1)
    scatter(source_locations.x(i), source_locations.y(i), 'bo')
end
%% Plotting the drone measurements



for drone = 2:size(T,1)
    file_path = T.(1){drone};

    hold on;
    drone_measurements = readtable("../" + file_path);
    
    % FIRST MEASUREMENT

    row = 1;

    drone_x = drone_measurements.drone_x(row);
    drone_y = drone_measurements.drone_y(row);
    drone_pos = scatter(drone_x, drone_y);
    set(drone_pos, "XData",drone_x);
    set(drone_pos, "YData",drone_y);


    % cols of drone measurements are drone x, drone y, id 0, dist 0 etc.
    % all cols -2 is all measurements. 1 dist per pair of cols
    num_sources = (size(drone_measurements,2)-2)/2;
    signal_ranges = zeros(num_sources);

    

    for n= 1:num_sources
        signal_ranges(n) = line(drone_x,drone_y);
        radius = drone_measurements.(2*n+2)(row);
        set(signal_ranges(n), "XData", drone_x + radius*sin(linspace(0,2*pi,100)));
        set(signal_ranges(n), "YData", drone_y + radius*cos(linspace(0,2*pi,100)));
        drawnow;

    end

    for row  = 2:size(drone_measurements,1)    
        drone_x = drone_measurements.drone_x(row);
        drone_y = drone_measurements.drone_y(row);
        set(drone_pos, "XData",drone_x);
        set(drone_pos, "YData",drone_y);      
    
        for n= 1:num_sources
            radius = drone_measurements.(2*n+2)(row);
            set(signal_ranges(n), "XData", drone_x + radius*sin(linspace(0,2*pi,100)));
            set(signal_ranges(n), "YData", drone_y + radius*cos(linspace(0,2*pi,100)));
            drawnow;
    
        end

    end


    hold off

end





