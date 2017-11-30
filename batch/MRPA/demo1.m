clear all
close all
clc
%Note that if only sptial info are considered, LISE and ISE can be used instead.

%% load data
%fin is the filename for simplification
fin = 'r6.txt';
tic
[lat, lon, tin] = readdata(fin);
t= toc;
disp(['time cost of read data and convert to unix time is ' num2str(t) 's'])

%% mercator projection
[xin,yin, scalefactor] = mercator_proj(lat,lon);


%% simplification by given thre
LSSDth = 10000;
tic
pathpt = TDMRPA_SED(xin,yin,tin, LSSDth,2,2);
toc

%% plot
df = ['LSSD= ' num2str(LSSDth) ', N = ' num2str(length(lat)) ', M = ' num2str(length(pathpt))];
figure,plot(lon, lat,'b-',lon(pathpt),lat(pathpt), 'rx-'),title(df)

