function [lat, lon, tin] = readdata(fname)
% read data lat/lon/time
fid = fopen(fname);
outtmp = textscan(fid, '%f %f %s %s');
fclose(fid);
lat = outtmp{1,1};
lon = outtmp{1,2};
tin = zeros(size(lat));
for i = 1:length(tin)
%     tstr = [ outtmp{3}{i} ' '  outtmp{4}{i}];
%     tin(i) = datenum(tstr);
%     datatime = datevec(tstr);
   datetmp = outtmp{3}{i};
   timetmp = outtmp{4}{i};
   sq1 = find(datetmp=='-');
   sq2 = find(timetmp==':');
   
   yy = str2double(datetmp(1:(sq1(1)-1)));
   mm = str2double(datetmp((sq1(1)+1):(sq1(2)-1)));
   dd = str2double(datetmp((sq1(2)+1):end));
   
   hh = str2double(timetmp(1:(sq2(1)-1)));
   mi = str2double(timetmp((sq2(1)+1):(sq2(2)-1)));
   ss = str2double(timetmp((sq2(2)+1):end));
   
   
   
   datatime = [yy mm dd hh mi ss];
    

    tin(i) = unixtime(datatime);
end


%% MOPSI DATA has errors, remove points with same timestamp but different
%% position
difft = diff(tin);
sq = find(difft==0);
lat(sq) = [];
lon(sq) = [];
tin(sq) = [];







