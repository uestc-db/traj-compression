function [xin,yin, scalefactor] = mercator_proj(lat,lon,param)
%% mercator projection
if ~exist('param')
    acc_scalefactor = 1e-4;
    acc_x = 0.1;
    acc_y = 0.1;
else
    acc_scalefactor = param.acc_scalefactor;
    acc_x = param.acc_x;
    acc_y = param.acc_y;
end

xy_dist = 6378100;


x = deg2rad(lon);
y = deg2rad(lat);
% Projection:
y2 = log(abs(tan(y)+sec(y)));
%   y2 = log(tan(pi/4+y/2));
sf = sec(y);
scalefactor = round(mean(sf)/acc_scalefactor)*acc_scalefactor;

xin = x* xy_dist /scalefactor;
yin = y2* xy_dist /scalefactor;

xin = round(xin/acc_x)*acc_x;
yin = round(yin/acc_y)*acc_y;






function rad = deg2rad(deg)
rad = deg*pi/180;





