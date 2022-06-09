%% Reading in the file names
T = readtable("../logs/file_path_log.csv", "VariableNamingRule", "preserve", "Delimiter", ",");


% Change this to true to remove old plots on changing timestep
clear_on_update = true;

field_file_path = T.(1){1};
field = readtable("../" + field_file_path);
drone_file_path = T.(1){2};
drone_measurements = readtable("../" + drone_file_path);

num_timesteps = size(drone_measurements,1);
num_drones = size(T,1)-1;
num_sources = size(field, 1) -1;

colours = hsv(num_sources);
cmap = containers.Map();

drone_colours = [[0.3010 0.7450 0.9330];[0.6350 0.0780 0.1840]];


clf;
hFig = figure(1);
hFig.set("Name", "Field")

timestep = 1;

hSubplots = zeros(1,num_drones);

while (timestep <= num_timesteps)
    if(clear_on_update)
        for i = 1:num_drones
            cla(hSubplots(i))
        end
    end
    for drone = 1:num_drones
        
        %% Plotting the field

        
        hSubplots(drone) =subplot(num_drones, 1, drone);
        hold on
        daspect([1 1 1])


        axis([-10, field.x(1)+10, -10, field.y(1)+10])
        rectangle('Position',[0 0 field.x(1) field.y(1)])

        for i = 1:num_sources
            if(isKey(cmap, field.Name{i+1}) == false)
                cmap(field.Name{i+1}) = colours(i,:);
            end

            scatter(field.x(i+1), field.y(i+1), "MarkerFaceColor", cmap(field.Name{i+1}), "MarkerEdgeColor", cmap(field.Name{i+1}))
        end

        %% Plotting the drone & measurements
        drone_file_path = T.(1){drone+1};
    
        hold on;
        drone_measurements = readtable("../" + drone_file_path);
        
        % FIRST MEASUREMENT
    
        
    
        drone_x = drone_measurements.drone_x(timestep);
        drone_y = drone_measurements.drone_y(timestep);
        drone_pos = scatter(drone_x, drone_y, 'h',"MarkerFaceColor", 'w', "MarkerEdgeColor", drone_colours(drone,:));
        drone_path = line(drone_measurements.drone_x(1:timestep),drone_measurements.drone_y(1:timestep), "Color",drone_colours(drone,:));
    
   
    
        
    
        for n= 1:num_sources
            radius = drone_measurements.(2*n+2)(timestep);
            if(radius > 0)
                line(drone_x + radius*sin(linspace(0,2*pi,100)),drone_y + radius*cos(linspace(0,2*pi,100)), "Color", cmap(drone_measurements.(2*n+1){timestep}));
            end
            
            
    
        end
        
    
        hold off
    end

    timestep = timestep+1;

end








