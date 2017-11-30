function pathpt = TDMRPA_SED(ptx,pty,tin, LISSEDth,c, shiftv)


%% thre
% LISSEDth = 1e-4;
if ~exist('c')
    c = 2;
end
if ~exist('shiftv')
    shiftv = 1;
end

%% init
meanptx = (mean(ptx));
meanpty = (mean(pty));
ptx = ptx - meanptx;
pty = pty - meanpty;
tin = tin - tin(1);
ptnum =  length(ptx);
ptind = 1:ptnum;

%% Caculate Cumulative value for later LISE caculation

% global Xcum Ycum XYcum X2cum Y2cum;
% tic
[Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum, YTsum]=get_cumsumval_SED(ptx,pty,tin);
% [Xcum,Ycum,XYcum,X2cum,Y2cum]=get_cumsumval(ptx,pty);
% t_getcum=toc;
ptx_ori = ptx;
pty_ori = pty;
tin_ori = tin;

%% Init logN Thre

th = [];
testM = fix(ptnum/c);
cont = 2;
while (testM>16)&&(cont>0)

    if (testM==2)
        path1 = [1 ptnum];
    else
        sptmp = (ptnum-1)/(testM-1);
        path1 = 1:sptmp:ptnum;
        path1 = round(path1);
    end
    distsISSED = calc_ISSED_pathpt(ptx,pty,tin,path1,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum, YTsum);
    estval = distsISSED/(testM-1);
    th = [th estval];
    testM = fix(testM/c);
    if estval>LISSEDth
        if (cont>1)
            cont = 1;
        elseif (cont==1)
            cont = 0;
        end
    end
end

th = sort(th);% prevent coarser scale has a larger tol, may happen very rarely.

%% TDMR for logN  initial curve
scalenum = length(th) - 1;
path_ref = cell(1,scalenum);
tcost_TDMR_init = zeros(1,scalenum);
for i = 1:scalenum
    thre0 = th(i);
    dThre = th(i+1);
%     tic

%      [linkcell,parind] = PA_minN_initv2(ptx,pty,Xcum,Ycum,X2cum,Y2cum,XYcum,thre0,dThre,ptind);
     [linkcell,parind] = PA_minNSED_init(ptx,pty,tin,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,thre0,dThre,ptind);

%%  This part can also skipped for fast processing speed.  
%     W=2;
%     tic
%     [treeinfo,pathpt,parind,tnum] = PA_minM_tree(ptx,pty,treeinfo,parind,pathpt,Xcum,Ycum,X2cum,Y2cum,XYcum,W,thre0,ptind);
%     toc
%%
    [Xout,Yout, pathpt] = PA_mineSED_CRSDP(ptx,pty,tin,linkcell,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,thre0,ptind);
%     tcost_TDMR_init(i) = toc;
    path_ref{i} = ptind(pathpt);
    %     figure,plot(ptx(pathpt),pty(pathpt))
    % maxdist1 = test_ISEerror(ptx, pty, pathpt_1a);%(4), sumISE
    % maxdist2 = test_ISEerror(ptx, pty, pathpt_1b);
    ptind = ptind(pathpt);
    ptx = ptx(pathpt);
    pty = pty(pathpt);
    tin = tin(pathpt);
%     outM = [outM length(ptx)];
end
% tcost_TDMR_init= cumsum(tcost_TDMR_init);
% tcost_TDMR_init = tcost_TDMR_init + t_getcum

%% APPLY TDMR to LISSEDth
ptx = ptx_ori;
pty= pty_ori;
tin = tin_ori;
outM_PA_TDMR = [];

thre0 = LISSEDth;
sq0 =find(th>=thre0, 1,'first');
sq = sq0-shiftv;
if (sq<=0)
    ptxs = ptx;
    ptys = pty;
    tins = tin;
    ptind = 1:ptnum;
    sel_scale = 0;%input scale 0 original curve
elseif isempty(sq)
    ptxs = ptx(path_ref{end});
    ptys = pty(path_ref{end});
    tins = tin(path_ref{end});
    ptind = path_ref{end};
    sel_scale = length(path_ref);
else
    ptxs = ptx(path_ref{sq});
    ptys = pty(path_ref{sq});
    tins = tin(path_ref{sq});
    ptind = path_ref{sq};
    sel_scale = sq;
end
tar = sq0 + 1;
if tar>length(th)
    dThre = inf;
else
    dThre = th(tar);
end
% tic
  [linkcell,parind] = PA_minNSED_init(ptxs,ptys,tins,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,thre0,dThre,ptind);
  
  w=2;
  [linkcelln,parind,tnum] = PA_minMSED_tree(ptxs,ptys,tins,linkcell,parind,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,w,thre0,ptind);
  
  [Xout,Yout, pathpt] = PA_mineSED_CRSDP(ptxs,ptys,tins,linkcelln,Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum,YTsum,thre0,ptind);
%   tcost_MRPA = toc;
%   sq
% if sq>0
%     tcost_MRPA = tcost_MRPA+tcost_TDMR_init(sq);
% else
%     tcost_MRPA = tcost_MRPA+t_getcum;
% end
pathpt = ptind(pathpt);
% tcost_MRPA
% M_MRPA = length(pathpt)

% eff_split = M_opt/M_split;
% eff_merge = M_opt/M_merge;
% eff_MRPA = M_opt/M_MRPA;
% M_opt
% [eff_split eff_merge eff_MRPA]
% [tcost_opt tcost_split tcost_merge tcost_MRPA]/5




