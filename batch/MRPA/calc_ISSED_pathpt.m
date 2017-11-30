function distsISSED = calc_ISSED_pathpt(x0,y0,t0,pathpt,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum, YTsum)
% eml.inline('always')

l = length(pathpt);
distsISSED = 0;
for i = 1:(l-1)
    fst = pathpt(i);
    lst = pathpt(i+1);
    if ( (fst +1) == lst)
        dists = 0;
    else
        
        xi = x0(fst);
        yi = y0(fst);
        ti = t0(fst);     
        xj = x0(lst);
        yj = y0(lst);
        tj = t0(lst);
        

        c1x = xi*tj- xj*ti;
        c2x = c1x^2;
        c3x = (tj-ti);
        c4x = c3x^2;
        c5x = xj-xi;
        c6x = c5x^2;
        
        dists = (lst-fst-1)* c2x/ c4x + c6x/c4x * (T2sum(lst-1) - T2sum(fst)) ...
            + (X2sum(lst-1) - X2sum(fst))...
            +  2* c1x * c5x/c4x * (Tsum(lst-1) - Tsum(fst))...
            -  2* c1x/c3x *(Xsum(lst-1) - Xsum(fst))...
            -  2* c5x/c3x *(XTsum(lst-1) - XTsum(fst));
        
        
        c1y = yi*tj- yj*ti;
        c2y = c1y^2;
        c3y = c3x;
        c4y = c3y^2;
        c5y = yj-yi;
        c6y = c5y^2;
        
        dists = dists +  (lst-fst-1)* c2y/ c4y...
             +  c6y/c4y * (T2sum(lst-1) - T2sum(fst))...
             +  (Y2sum(lst-1) - Y2sum(fst))...
             +   2* c1y * c5y/c4y * (Tsum(lst-1) - Tsum(fst))...
             -   2* c1y/c3y *(Ysum(lst-1) - Ysum(fst))...
             -   2* c5y/c3y *(YTsum(lst-1) - YTsum(fst));
    end

    distsISSED = distsISSED + dists;

end