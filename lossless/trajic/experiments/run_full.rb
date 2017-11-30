#!/usr/bin/env ruby

require './common'

check_for_executables!

config = {
  algorithm: "trajic",
  #data_path: "Illinois/person1",
  #extension: "txt"
  data_path: "IllinoisClustered/Processed/0.03/centers",
  extension: "csv"
}

files = data_files(config[:data_path], config[:extension], :all)

checkpoint = 0.1 * files.length
usize = 0.0
csize = 0.0
n_trajs = 0
compr_time = 0.0
decompr_time = 0.0
max_error_kms = 0.0

files.each_with_index do |path, i|
  if i >= checkpoint
    checkpoint += 0.1 * files.length
    print "."
    $stdout.flush
  end

  # # Lossless
  command = "./stats #{config[:algorithm]} '#{path}'"

  # # ~1m error
  # command= "./stats #{config[:algorithm]} '#{path}' 1 0.00001"

  # # ~30m error
  # if config[:algorithm] == "squish"
  #   command = "./stats #{config[:algorithm]} '#{path}' 0 0.08"
  # else
  #   command = "./stats #{config[:algorithm]} '#{path}' 0 0.00028"
  # end

  lines = IO.popen(command) do |io|
    io.readlines
  end

  unless $?.success?
    $stderr.puts "[FAIL] `#{command}`"
    next
  end

  unless lines.empty?
    results = {}
    lines.each do |line|
      key, val = *line.split("=")
      results[key] = val.strip
    end

    usize += results["raw_size"].to_i
    csize += results["compr_size"].to_i
    compr_time += results["compr_time"].to_i
    decompr_time += results["decompr_time"].to_i

    error_kms = results["max_error_kms"].to_f
    max_error_kms = error_kms if error_kms > max_error_kms

    n_trajs += 1
  end
end

puts "."
puts "Algorithm: #{config[:algorithm]}"
puts "Data path: #{config[:data_path]}"
puts "Num trajectories: #{n_trajs}"
puts "Max error: #{max_error_kms * 1000} m"
puts "Compression ratio: #{csize / usize}"
puts "Average compression time: #{compr_time / n_trajs / 1000} ms"
puts "Average decompression time: #{decompr_time / n_trajs / 1000} ms"

