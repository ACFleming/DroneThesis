function PlotField(drone_pos, field)
    figure
    spy(field)
    hold on
    d_rows = drone_pos(:,1);
    d_cols = drone_pos(:,2);
    scatter(d_rows,d_cols)
    hold off
end