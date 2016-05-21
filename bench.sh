#!/bin/bash
number_of_reputation=100

if [ ! -f 2048/2048.rb ]; then
    echo "aiプログラムやlogs等があるディレクトリで実行してください"
    exit 1
fi

if [ ! -e logs ]; then
    mkdir logs
fi

if [ -f logs/bench_turn.log ]; then
    rm logs/bench_turn.log
fi
if [ -f logs/bench_score.log ]; then
    rm logs/bench_score.log
fi

for i in `seq 1 $number_of_reputation`
do
    ruby 2048/2048.rb -exec "ruby ai.rb"
    tmpf=$(mktemp)
    cat "logs/2048.log" | grep "total turn" > $tmpf
    echo `awk 'BEGIN {  } END { printf("%d\n", $3); }' $tmpf` >> logs/bench_turn.log
    rm $tmpf
    tmpf=$(mktemp)
    cat "logs/2048.log" | grep "total score" > $tmpf
    echo `awk 'BEGIN {  } END { printf("%d\n", $3); }' $tmpf` >> logs/bench_score.log
    rm $tmpf
    tmpf=$(mktemp)
    cat "logs/2048.log" | grep "max num" > $tmpf
    echo `awk 'BEGIN {  } END { printf("%d\n", $3); }' $tmpf` >> logs/bench_max_num.log
    rm $tmpf
done

turn=`awk 'BEGIN { } { sum += $1; sum2 += $1 * $1; } END { printf("%lf #avarage, %lf #SDev\n", sum / NR, sqrt(sum2 / NR - (sum / NR ) ^ 2)); }' logs/bench_turn.log`
score=`awk 'BEGIN { } { sum += $1; sum2 += $1 * $1; } END { printf("%lf #avarage, %lf #SDev\n", sum / NR, sqrt(sum2 / NR - (sum / NR ) ^ 2)); }' logs/bench_score.log`
max_num=`awk 'BEGIN { } { sum += $1; sum2 += $1 * $1; } END { printf("%lf #avarage, %lf #SDev\n", sum / NR, sqrt(sum2 / NR - (sum / NR ) ^ 2)); }' logs/bench_max_num.log`
max_num_max=`awk 'BEGIN { } { if ($1 > sum) { sum = $1; } } END { printf("%ld #Max Result\n", sum); }' logs/bench_max_num.log`

echo -n "Turn : "
echo $turn
echo -n "Score : "
echo $score
echo -n "Max Num : "
echo $max_num
echo -n "Max Result : "
echo $max_num_max

if [[ $1 = "log" ]]; then
    date_for_file=`date '+%y_%m_%d_%H_%M_%s'`
    dir_name_for_log="past_logs/`echo $date_for_file`"
    mkdir -p $dir_name_for_log
    echo -n "Turn : " >> $dir_name_for_log/bench.log
    echo $turn >> $dir_name_for_log/bench.log
    echo -n "Score : " >> $dir_name_for_log/bench.log
    echo $score >> $dir_name_for_log/bench.log
    echo -n "Max Num : " >> $dir_name_for_log/bench.log
    echo $max_num >> $dir_name_for_log/bench.log
    echo -n "Max Result : " >> $dir_name_for_log/bench.log
    echo $max_num_max >> $dir_name_for_log/bench.log
fi
