function RSSI = SignalStrength(r,c, field)
    [rows,cols] = find(field)
    for i = 1:length(rows)
        r = [r ,rows(i)];
        c = [c,cols(i)];
        D = norm(r-c)
    end
    RSSI = 1;
end




