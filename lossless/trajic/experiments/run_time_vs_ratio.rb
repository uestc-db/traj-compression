#!/usr/bin/env ruby

require './common'

check_for_executables!

DATA_ROOT = "/home/aiden/Data/Trajectories/Geolife/Data"

config = {
  :n => 100
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

error = 0.1

while error > 0.00000001
  puts error
  ["trajic", "dpd", "dp"].each do |alg|
    checkpoint = 0.1 * files.length
    max_error_kms = -1
    usize = 0.0
    csize = 0.0
    n_trajs = 0
    compr_time = 0.0
    decompr_time = 0.0
    files.each_with_index do |path, i|
      if i >= checkpoint
        checkpoint += 0.1 * files.length
        print "."
        $stdout.flush
      end

      io = IO.popen("./stats #{alg} \"#{path}\" 0 #{error}")
      lines = io.readlines
      io.close

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
    puts "Max error: #{max_error_kms * 1000} m"
    puts "Compression ratio: #{csize / usize}"
    puts "Average compression time: #{compr_time / n_trajs / 1000} ms"
    puts "Average decompression time: #{decompr_time / n_trajs / 1000} ms"
    if max_error_kms > 0.1
      fout.print "? ? "
    else
      fout.print "#{csize / usize} #{compr_time / n_trajs / 1000} "
    end
  end

  puts
  fout.puts
  error /= 2
end

fout.close

gnuplot_instr = <<-END
set autoscale
set xrange[0:1]
set yrange[0:6]
set xtic auto
set ytic auto
set xlabel "Compression ratio"
set ylabel "Compression time (ms)"
set size 1.0, 0.6
set terminal postscript portrait enhanced mono dashed dl 5 lw 1 "Helvetica" 14
set output "time_vs_ratio-#{config[:n]}.ps"
plot "results.dat" using 1:2 lt 1 title 'Trajic' with lines , \
     "results.dat" using 3:4 lt 2 title 'TD-SED + Delta' with lines , \
     "results.dat" using 5:6 lt 4 title 'TD-SED' with lines
END

open("results.p", "w") do |f|
  f.puts gnuplot_instr
end

`gnuplot "results.p"`

rm "results.dat"
rm "results.p"

