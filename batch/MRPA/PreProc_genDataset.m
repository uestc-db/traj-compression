% Create two dataset, Geolife and MOPSI

workfolder = cd;
cd ..
rootfolder = cd;

tmp = find(currfolder=='\');
if isempty(tmp)
    % unix
    oritxtfolder = [rootfolder '/data/geolife/oritxt/'];
else
    oritxtfolder = [rootfolder '\data\geolife\oritxt\'];
end

flist = dir(oritxtfolder);
xall = cell(length(flist)-2,1);
yall = cell(length(flist)-2,1);
tall = cell(length(flist)-2,1);
cd(workfolder);
for  i = 3:length(flist)
    i
    fnamein = flist(i).name;
    fin = [oritxtfolder fnamein];
    [lat, lon, tin] = readdata(fin);
   
    %% mercator projection
    [xin,yin, scalefactor] = mercator_proj(lat,lon);
    xall{i-2} = xin;
    yall{i-2} = yin;
    tall{i-2} = tin;
end
save geolife_PA xall yall tall

%for mopsi
cd ..
rootfolder = cd;

tmp = find(currfolder=='\');
if isempty(tmp)
    % unix
    oritxtfolder = [rootfolder '/data/mopsi/oritxt/'];
else
    oritxtfolder = [rootfolder '\data\mopsi\oritxt\'];
end

flist = dir(oritxtfolder);
xall = cell(length(flist)-2,1);
yall = cell(length(flist)-2,1);
tall = cell(length(flist)-2,1);
cd(workfolder);
for  i = 3:length(flist)
    i
    fnamein = flist(i).name;
    fin = [oritxtfolder fnamein];
    [lat, lon, tin] = readdata(fin);
   
    %% mercator projection
    [xin,yin, scalefactor] = mercator_proj(lat,lon);
    xall{i-2} = xin;
    yall{i-2} = yin;
    tall{i-2} = tin;
end
save mopsi_PA xall yall tall