function [Xout,Yout,pathpt]= GPStraj_TS(ptx,pty,M)
if M==2
    pathpt = [1 length(ptx)];
    Xout = ptx(pathpt);
    Yout = pty(pathpt);
    return;
end
N = length(ptx);

dx = diff(ptx);
dy = diff(pty);
dist0 = sqrt(dx.^2 +dy.^2);
dist0 = dist0(1:end-1) + dist0(2:end);

ang = atan2(dy,dx);
dang = diff(ang);
dang(dang>=pi) =dang(dang>=pi)-2*pi;

dang(dang<=-pi) =dang(dang<=-pi)+2*pi;

tu = 3;
dangf = imfilter(dang,ones(1, 2*tu+1),'replicate');
dangabs = abs(dang);
dangf = 180/pi*dangf;
dangabs = 180/pi*dangf;
wfinal = (abs(dangf)+dangabs).*dist0;
[tmp,sq] = sort(wfinal,'descend');
if size(sq,1)==1
    sqall = [sq(1:M-2)+1 1 N];
else
    sqall = [sq(1:M-2)+1; 1; N];
end

[sq,tmp]=sort(sqall);
pathpt = sq;
Xout = ptx(pathpt);
Yout = pty(pathpt);