#!/usr/bin/env python 
#coding:utf-8 
import sys
import os
def scan_files(directory,prefix=None,postfix=None):
  files_list=[]
    
  for root, sub_dirs, files in os.walk(directory):
    for special_file in files:
      if postfix:
        if special_file.endswith(postfix):
          files_list.append(os.path.join(root,special_file))
      elif prefix:
        if special_file.startswith(prefix):
          files_list.append(os.path.join(root,special_file))
      else:
        files_list.append(os.path.join(root,special_file))
               
  return files_list

if __name__ == '__main__':
      paras = sys.argv
      dirname = paras[1]  
      spatial_erro = float(paras[2])
      dataset = dirname.split('/')[-1]
      #files_list = scan_files(paras[1])
      filenames = "/home/dingmengting/work/part/"+dataset+"_filelist.txt"
      f_filenames = open(filenames,'r')
      files_list = f_filenames.readlines()
      #files_list = scan_files(dir_names)
      for filename in files_list:
            filename = filename[:-1]
	    print filename
            save_path = '/data4/dingmengting/time/DPhull'
	    f = open(filename,"r")
	    n = len(f.readlines())
            if not os.path.exists(save_path):
                os.mkdir(save_path)
            if "Illinois" in filename:
                   save_path = save_path +'/Illinois/'
            if "geolife" in filename:
                   save_path = save_path + '/geolife/'
            if "indoor" in filename:
                   save_path = save_path + '/indoor/'
            if "SingaporeTaxi" in filename:
                   save_path = save_path + '/SingaporeTaxi/'
	    if "truck" in filename:
		   save_path = save_path +"/truck/"
	    if "Phone" in filename:
		   save_path = save_path +"/Phone/"
            if not os.path.exists(save_path):
                   os.mkdir(save_path)
            save_path = save_path + str(spatial_erro)+'/'
            if not os.path.exists(save_path):
                   os.mkdir(save_path)
            save_path = save_path + filename.split('/')[-3]+'/'
            if not os.path.exists(save_path):
                   os.mkdir(save_path)
            save_path = save_path + filename.split('/')[-2]+'/'
            if not os.path.exists(save_path):
                   os.mkdir(save_path)
            save_path = save_path + filename.split('/')[-1]
            commond = "/home/dingmengting/work/DPhull/hull" +" " + filename + " " + str(n) + " " + str(spatial_erro) + " " + save_path
	    try:
	    		os.system(commond)
	    except:
			continue
