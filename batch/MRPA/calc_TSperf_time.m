function errall = calc_TSperf_time(ptx,pty,tpos,pathpt)
%consider time
% err = [rmse,mae,maxerr,rmse_SEC,mae_SEC, maxerr_SEC]
dist_all = zeros(1,length(ptx));
for  i = 1:(length(pathpt)-1)
    st = pathpt(i);
    ed = pathpt(i+1);
    dists = p2l_all(ptx,pty,st,ed);
    dist_all((st+1):(ed-1)) = dists;
end
rmse = sqrt(mean(dist_all.^2));
mae = mean(dist_all);
merr = max(dist_all);

mae_med = median(dist_all);

appx_sec = ptx;
appy_sec = pty;
for  i = 1:(length(pathpt)-1)
    st = pathpt(i);
    ed = pathpt(i+1);
    tst = tpos(st);
    ted = tpos(ed);
    dx = ptx(ed)-ptx(st);
    dy = pty(ed)-pty(st);
    xst = ptx(st);
    yst = pty(st);
    for pos = (st+1):(ed-1)
        rt = (tpos(pos)-tst)/(ted-tst);
        appx_sec(pos) = xst +dx*rt;
        appy_sec(pos) = yst +dy*rt;    
    end
end
dist_sec = sqrt((ptx-appx_sec).^2 +(pty-appy_sec).^2);
rmse_sec = sqrt(mean(dist_sec.^2));
mae_sec = mean(dist_sec);
merr_sec = max(dist_sec);

mae_sec_med =  median(dist_sec);
errall = [rmse mae mae_med  merr rmse_sec mae_sec mae_sec_med merr_sec];

function distall = p2l_all(ptx,pty,st,ed)
% sx,  sy,  ex,  ey,  px,  py
if st +1 ==ed
    distall = [];
    return;
end
distall = [];
sx = ptx(st);
sy = pty(st);
ex = ptx(ed);
ey = pty(ed);
for pos = (st+1):(ed-1)
    px = ptx(pos);
    py = pty(pos);
    dA = px - sx;
    dB = py - sy;
    dC = ex - sx;
    dD = ey - sy;
    dot = dA * dC + dB * dD;
    len_sq = dC * dC + dD * dD;
    if len_sq==0
        dists = sqrt(dA*dA +dB*dB);
    else
        param = dot / len_sq;
        closest_X = sx + param * dC;
        closest_Y = sy + param * dD;
        dists = sqrt((px-closest_X)^2 + (py-closest_Y)^2);
    end
    distall = [distall dists];

end