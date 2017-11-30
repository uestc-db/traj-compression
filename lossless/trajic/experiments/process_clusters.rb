#!/usr/bin/env ruby

require './common'

# In KMs
max_errors = %w[0.001 0.005 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.1]

base_path = File.join(DATA_BASE_PATH, "IllinoisClustered/Data")
processed_base_path = File.join(DATA_BASE_PATH, "IllinoisClustered/Processed")

if Dir.exists? processed_base_path
  rm_r processed_base_path
end

max_errors.each do |max_error|
  puts "Processing max_error=#{max_error}"
  member_list_path = File.join(base_path, "memberList-maxError-#{max_error}.txt")
  center_list_path = File.join(base_path, "centerList-maxError-#{max_error}.txt")

  centers_dir = File.join(processed_base_path, max_error, "centers")
  members_dir = File.join(processed_base_path, max_error, "members")

  mkdir_p centers_dir
  mkdir_p members_dir

  File.readlines(center_list_path).each do |line|
    next if line.empty?
    grid_id, traj_id, lat, lon, timestamp = *line.strip.split(",")
    File.open(File.join(centers_dir, [grid_id, traj_id].join("_") + ".csv"), "a") do |f|
      f.puts [timestamp, "%0.6f" % lat.to_f, "%0.6f" % -lon.to_f].join(", ")
    end
  end

  File.readlines(member_list_path).each do |line|
    next if line.empty?
    grid_id, center_traj_id, traj_id, timestamp = *line.strip.split(",")
    File.open(File.join(members_dir, [grid_id, center_traj_id, traj_id].join("_") + ".csv"), "a") do |f|
      f.puts timestamp
    end
  end
end
