%out_mopsi, LSSD diff for walking and no-walking
clear all
close all
clc
% average N/M
load mopsi_PA
load perf_MRPASED_mopsi_PA
Nall = zeros(length(xall),1);
for i = 1:length(xall)
    Nall(i) = length(xall{i});
end
NdM1 = mean(Nall'./m_MRPASED(1,:));
NdM2 = mean(Nall'./m_MRPASED(2,:));
NdM3 = mean(Nall'./m_MRPASED(3,:));

load geolife_PA
load perf_MRPASED_geolife_PA
Nall = zeros(length(xall),1);
for i = 1:length(xall)
    Nall(i) = length(xall{i});
end
NdM1a = mean(Nall'./m_MRPASED(1,:));
NdM2a = mean(Nall'./m_MRPASED(2,:));
NdM3a = mean(Nall'./m_MRPASED(3,:));


%% measure no-walking and walking reduction rate LSSD,
%out_mopsi, LSSD diff for walking and no-walking
clear all
close all
clc
% average N/M
load mopsi_PA
load perf_MRPASED_mopsi_PA

val_walk = zeros(16,1);
val_nowalk = zeros(16,1);
curr_w = 1;
curr_nw = 1;

for i = 1:length(xall)
    xtmp = xall{i};
    ytmp = yall{i};
    ttmp = tall{i};
    spdtmp = sum(sqrt(diff(xtmp).^2 + diff(ytmp).^2))/(ttmp(end)- ttmp(1));
    if spdtmp<1.8%walk
        val_walk(curr_w,1:4) = [length(xtmp) m_MRPASED(:,i)'];
        val_walk(curr_w,5:8) = [errall_MRPASED{1}(5:8,i)'];
        val_walk(curr_w,9:12) = [errall_MRPASED{2}(5:8,i)'];
        val_walk(curr_w,13:16) = [errall_MRPASED{3}(5:8,i)'];
        curr_w = curr_w +1;
    elseif spdtmp>15%car
        val_nowalk(curr_nw,1:4) = [length(xtmp) m_MRPASED(:,i)'];
        val_nowalk(curr_nw,5:8) = [errall_MRPASED{1}(5:8,i)'];
        val_nowalk(curr_nw,9:12) = [errall_MRPASED{2}(5:8,i)'];
        val_nowalk(curr_nw,13:16) = [errall_MRPASED{3}(5:8,i)'];
        curr_nw = curr_nw +1;
    end
end

m_walk = [mean(val_walk(:,1)./val_walk(:,2)) mean(val_walk(:,1)./val_walk(:,3)) mean(val_walk(:,1)./val_walk(:,4 )), ...
    mean(val_walk(:,5:end))];

m_nwalk = [mean(val_nowalk(:,1)./val_nowalk(:,2)) mean(val_nowalk(:,1)./val_nowalk(:,3)) mean(val_nowalk(:,1)./val_nowalk(:,4 )), ...
    mean(val_nowalk(:,5:end))];    


