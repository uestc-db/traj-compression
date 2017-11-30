function [Xout,Yout, pathpt] = PA_mineSED_CRSDP(x0,y0,t0,linkcell,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,thre,ptind)

%testing for C-RSDP reduce e, using proirity queue structure

ptnum = length(x0);
if nargin<14
    ptind = 1:ptnum;
end
pathnum = length(linkcell)-1;
E = inf*ones(1,ptnum);
A = inf*ones(1,ptnum);
E(1) = 0;
A(1) = 0;
linkcell{end} = ptnum;

% treeinfo = cell2struct(treeinfo,'ch');

for m = 1:pathnum
    tarind = linkcell{m+1};
    curind = linkcell{m};

    for  ind2 = 1:length(tarind)
        n = tarind(ind2);
        cmin = inf;

        xj = x0(n);
        yj = y0(n);
        tj = t0(n);
        
        for ind1 = 1:length(curind) 
            j = curind(ind1);
            if j<n
                fst = ptind(j);
                lst = ptind(n);
                if ( (fst +1) == lst)
                    dists = 0;
                else
                    xi = x0(j);
                    yi = y0(j);
                    ti = t0(j);
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

                if (dists <= thre)
                    c = E(j) + dists;
                    if (c<cmin)
                        cmin = c;
                        jmin = j;
                    end
                end
            end
        end
        E(n) = cmin;
        A(n) = jmin;
    end

end

% ptsq = ptnum;
% pathpt =[];
% for i = pathnum:-1:0
%     pathpt= [pathpt ptsq];
%     ptsq = A(ptsq);
% end
% pathpt = pathpt(end:-1:1);

pathpt = zeros(1,pathnum+1);
pathpt(end) = ptnum;
for i = pathnum:-1:1
    pathpt(i) = A(pathpt(i+1));
end
%copy reduced points' cordinate and index to cell
Xout = x0(pathpt);
Yout = y0(pathpt);


