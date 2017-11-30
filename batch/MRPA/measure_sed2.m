function sedist2 = measure_sed2(x0,y0,t0, i, j)
%measure sum of sed2 dist of compressed curve
%seddist = [maxsed, avesed, medsed. mssed]

% t0 = t0-t0(1);
% tc = tc-tc(1);

sedist2 = 0;
for  k = i:j

    tp = (t0(k) - t0(i))/(t0(j) - t0(i));
    
    
    xest = x0(i) + (x0(j)-x0(i))*tp;
    yest = y0(i) + (y0(j)-y0(i))*tp;

    sedist2 = sedist2+ (xest-x0(k))^2 + (yest-y0(k))^2;
end


