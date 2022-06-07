function PlotField()
    close all

    source_locations = readmatrix("../logs/field_log_file.csv", "NumHeaderLines", 1);

    % Draw signal sources
    figure
    hold on
    daspect([1 1 1])

    axis([-10, source_locations(1,1)+10, -10, source_locations(1,2)+10])
    rectangle('Position',[0 0 source_locations(1,1) source_locations(1,2)])

    for i = 2:length(source_locations)
        scatter(source_locations(i,1), source_locations(i,2), 'bo')
    end
    



end

