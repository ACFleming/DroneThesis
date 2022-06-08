function PlotField(file_path)
    close all

    source_locations = readtable("../" + file_path);

    % Draw signal sources
    figure
    hold on
    daspect([1 1 1])

    axis([-10, source_locations.x(1)+10, -10, source_locations.y(1)+10])
    rectangle('Position',[0 0 source_locations.x(1) source_locations.y(1)])

    for i = 2:size(source_locations,1)
        scatter(source_locations.x(i), source_locations.y(i), 'bo')
    end
    



end

