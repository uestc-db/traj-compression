#!/usr/bin/env ruby

require './common'

require 'json'

check_for_executables!

config = {
  algorithm: "trajic",
  data_path: "Illinois/person1",
  extension: "txt"
  #data_path: "IllinoisClustered/Processed/0.03/centers",
  #extension: "csv"
}

files = data_files(config[:data_path], config[:extension], :all)

checkpoint = 0.01 * files.length

all_results = {}

files.each_with_index do |path, i|
  if i >= checkpoint
    checkpoint += 0.01 * files.length
    print "."
    $stdout.flush
  end

  command = "./run_predictors '#{path}'"

  results = IO.popen(command) do |io|
    io.read
  end

  unless $?.success?
    $stderr.puts "[FAIL] `#{command}`"
    next
  end

  json = JSON.parse(results)
  json.fetch("predictors").each do |predictor|
    predictor_results = (all_results[predictor.fetch("name")] ||= {})

    %w[time_us mean_residual_length mean_spatial_residual_length mean_temporal_residual_length].each do |metric_name|
      (predictor_results[metric_name] ||= []) << predictor.fetch(metric_name)
    end
  end
end

puts "."

puts "Medians:"
all_results.each do |predictor_name, metrics|
  puts "##  #{predictor_name}"
  metrics.each do |metric_name, values|
    puts "#{metric_name}: #{median(values)}"
  end
end
