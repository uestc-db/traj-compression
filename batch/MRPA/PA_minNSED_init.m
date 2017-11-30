function [linkcell,parind] = PA_minNSED_init(x0,y0,t0,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,thre,dThre,ptind)

%return a tree structure for min-# initialization
%treeinfo record sonnode for every vertex
%This is constructed using priority queue structure

%for testing
ptnum = length(x0);
if nargin<14
    ptind = 1:ptnum;
end

linkcell = cell(1,ptnum);
linkcell{1} = 1;
segind = inf * ones(1,ptnum);
segind(1) = 0;
parind = segind;
Vpr = true(1,ptnum);
Vpr(1) = 0;
Qlist1 = ones(ptnum,1);
Qlist2 = -ones(ptnum,1);
Qlist2t= -ones(ptnum,1);
Q1size =1;
Q2pos =1;%position to write

currlen = 0;
spandist = ptnum-1;

while (segind(end)>= inf)
    for p1 = 1:Q1size
        j = Qlist1(p1);
        xi = x0(j);
        yi = y0(j);
        ti = t0(j);
        Q2tpos = 1;
        for i = (j+1):ptnum
            if (Vpr(i))
                lst = ptind(i);
                fst = ptind(j);
                if ( (fst +1) == lst)
                    dists = 0;
                else
                    
                    xj = x0(i);
                    yj = y0(i);
                    tj = t0(i);
                    
                    
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
% 
%                 dists = (dists)/(lst-fst);
                if (dists > dThre)
                    break;
                end
                if (dists <= thre)
                    Qlist2t(Q2tpos) = i;
                    Q2tpos = Q2tpos+1;
                    Vpr(i) = 0;
                    parind(i) = j;			% parents
                end
            end
        end
        tmp = Q2tpos-1;
%         if tmp>0      
%             treeinfo{j} = Qlist2t(1:tmp)';
% 
%         end
        ed = Q2pos+tmp-1;
        Qlist2(Q2pos:ed) = Qlist2t(1:tmp);
        Q2pos = ed+1;
    end
    tmp = Q2pos-1;
    Qlist2(1:tmp) = sort(Qlist2(1:tmp),'descend');%sort may not necessary?
    currlen = currlen + 1;
    segind(Qlist2(1:tmp)) = currlen;
    linkcell{currlen+1} = Qlist2(1:tmp);
    
    tmp = Q2pos-1;
    Qlist1(1:tmp) = Qlist2(1:tmp);
    Q1size = tmp;
    Q2pos = 1;
%     spandist = Qlist1(1)/currlen;
end
linkcell = linkcell(1:(currlen+1));
% % ptsq = ptnum;
% pathpt = zeros(1,currlen+1);
% pathpt(end) = ptnum;
% for i = currlen:-1:1
%     pathpt(i) = parind(pathpt(i+1));
% end
% pathpt =[];
% while (ptsq>0)
%     pathpt= [pathpt ptsq];
%     ptsq = parind(ptsq);
% end
% pathpt = pathpt(end:-1:1);

% ptsegsq{1} = 1;
% ptsegsq{end} = ptnum;
