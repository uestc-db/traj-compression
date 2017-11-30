function [Xsum,Ysum,Tsum, X2sum,Y2sum,T2sum,XTsum, YTsum]=get_cumsumval_SED(x0,y0,t0)
%cumsum  for LISE_SED caculation
x2 = x0.*x0;
y2 = y0.*y0;
t2 = t0.*t0;
xt = t0.*x0;
yt = t0.*y0;

Xsum = cumsum(x0);
Ysum = cumsum(y0);
Tsum = cumsum(t0);
X2sum = cumsum(x2);
Y2sum = cumsum(y2);
T2sum = cumsum(t2);
XTsum = cumsum(xt);
YTsum = cumsum(yt);



% ifint64 = 0;
% if ifint64
%     ptx = int64(ptx);
%     pty = int64(pty);
%     X2 = ptx.*ptx;
%     XY = ptx.*pty;
%     Y2 = pty.*pty;
%     l = length(ptx);
%     Xsum = zeros(1,l,'int64');
%     Ysum = zeros(1,l,'int64');
%     X2sum = zeros(1,l,'int64');
%     Y2sum = zeros(1,l,'int64');
%     XYsum = zeros(1,l,'int64');
% 
%     Xsum(1) = ptx(1);
%     Ysum(1) = pty(1);
%     XYsum(1) = XY(1);
%     X2sum(1) = X2(1);
%     Y2sum(1) = Y2(1);
%     for  i = 2:l
%         i0 = i-1;
%         Xsum(i) = Xsum(i0) + ptx(i);
%         Ysum(i) = Ysum(i0) + pty(i);
%         XYsum(i) = XYsum(i0) + XY(i);
%         X2sum(i) = X2sum(i0)+X2(i);
%         Y2sum(i) = Y2sum(i0)+Y2(i);
%     end
% else
%     X2 = ptx.*ptx;
%     XY = ptx.*pty;
%     Y2 = pty.*pty;
%     l = length(ptx);
%     Xsum = cumsum(ptx);
%     Ysum = cumsum(pty);
%     XYsum = cumsum(XY);
%     X2sum = cumsum(X2);
%     Y2sum = cumsum(Y2);


% end
