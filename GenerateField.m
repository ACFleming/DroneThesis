function field = GenerateField(x_width,y_height, n_sources)
        % x is cols
        % y is rows
        field = zeros(y_height, x_width);
        s_pos = randperm(numel(field),n_sources);
        field(s_pos) = 1;
end 
