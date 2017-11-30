#!/usr/bin/env ruby

require './common'

check_for_executables!

DATA_ROOT = "/home/aiden/Data/Trajectories/Geolife/Data"

config = {
  :n          => 1000,
  :algorithm  => "trajic",
  :time       => "decompression"
}

files = []
Find.find(DATA_ROOT) do |path|
  if FileTest.file? path and path.end_with? ".plt"
    files << path
  end
end

unless config[:n].nil? or config[:n] == :all
  files = files[0...config[:n]]
end

fout = open("results.dat", "w")

checkpoint = 0.1 * files.length
files.each_with_index do |path, i|
  if i >= checkpoint
    checkpoint += 0.1 * files.length
    print "."
    $stdout.flush
  end

  # Error approx 1 s and 1 m
  io = IO.popen("./stats #{config[:algorithm]} \"#{path}\" 1 0.00001")
  lines = io.readlines
  io.close

  unless lines.empty?
    results = {}
    lines.each do |line|
      key, val = *line.split("=")
      results[key] = val.strip
    end

    points = results["raw_size"].to_i / 24
    time = 0
    if config[:time] == "compression"
      time = results["compr_time"].to_f / 1000
    elsif config[:time] == "decompression"
      time = results["decompr_time"].to_f / 1000
    end
    fout.puts "#{points} #{time}"
  end
end
puts "."

fout.close

gnuplot_instr = <<-END
set autoscale
set xtic auto
set ytic auto
set title "Trajectory size vs execution time (#{config[:n]} trajectories)"
set xlabel "Number of points"
set ylabel "#{config[:time].capitalize} time (ms)"
set size 1.0, 0.6
set terminal postscript portrait enhanced mono dashed lw 1 "Helvetica" 14
set output "size_vs_#{config[:time][0..0]}time-#{config[:algorithm]}-#{config[:n]}.ps"
plot "results.dat" using 1:2 title '#{config[:algorithm]}' with points
END

open("results.p", "w") do |f|
  f.puts gnuplot_instr
end

`gnuplot "results.p"`

rm "results.dat"
rm "results.p"
