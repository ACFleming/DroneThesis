function RSSI = SignalStrength(x,y, field)
    
    [rows,cols] = find(field);
    RSSI = zeros(length(rows),1);
    for i = 1:length(rows)
        RSSI(i) = sqrt((x-cols(i))^2 + (y-rows(i))^2);
    end
end




