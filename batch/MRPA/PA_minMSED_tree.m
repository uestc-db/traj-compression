function [linkcelln,parind,tnum] = PA_minMSED_tree(x0,y0,t0,linkcell,parind,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,W,thre,ptind)
%all segind
tnum = 0;%testing edges

linkcelln = linkcell;


ptnum = length(x0);
if nargin<16
    ptind = 1:ptnum;
end
pathnum = length(linkcell);
if pathnum<3
    return;
end


for i = pathnum:-1:3
    %
    tarpt = linkcelln{i};
    n_tarpt = length(tarpt);
    b_test = true(1,n_tarpt);
    
    for wth = W:-1:1
        tmp = i-1-wth;
        if tmp>0
            movpt = [];
            curpt = linkcelln{tmp};
            n_curpt = length(curpt);
            for tarsq = 1:n_tarpt
                if b_test(tarsq)
                    ind2 = tarpt(tarsq);
                    for cursq = 1:n_curpt
                        ind1 = curpt(cursq);
                        xi = x0(ind1);
                        yi = y0(ind1);
                        ti = t0(ind1);

                        if (ind2>ind1)
                            fst = ptind(ind1);
                            lst = ptind(ind2);
                            if ( (fst +1) == lst)
                                dists = 0;
                            else
                                tnum =  tnum +1;
                                
                                xj = x0(ind2);
                                yj = y0(ind2);
                                tj = t0(ind2);
                                
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
                                b_test(tarsq) = 0;
                                
                                %UPDATE TREE
                                parind(ind2) = ind1;
                                movpt = [movpt;ind2];                            
                            end                           
                        end
                    end
                end
            end
            linkcelln{i} = tarpt(b_test);
            if ~isempty(movpt)
                linkcelln{tmp+1} = [linkcelln{tmp+1};movpt];
            end
        end
    end
end

%update segind and linkcell
segind = ones(1,ptnum);
for i = 2:ptnum
    partmp = parind(i);
    if partmp<ptnum
        segind(i) = segind(parind(i)) + 1;
    else
        segind(i) = inf;
    end
end

nlink = segind(ptnum);
if nlink< length(linkcell)
    linkcelln = cell(1,nlink);
    linkcelln{1} = 1;
    fq = histc(segind,1:nlink);
    pos = ones(1,nlink);
    for i = 2:(nlink)
         linkcelln{i} = zeros(fq(i),1);
    end
    for i = 2:ptnum
        linktmp = segind(i);
        if linktmp<=nlink
            linkcelln{linktmp}(pos(linktmp)) = i;
            pos(linktmp) = pos(linktmp)+1;
        end
    end
    
end



% ptsq = ptnum;
% while (ptsq>0)
%     pathpt= [pathpt ptsq];
%     ptsq = parind(ptsq);
% end
% pathpt = pathpt(end:-1:1);

