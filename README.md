2048 AI project by future731

使い方
基本的にai.cppなどのmain関数を見ればなんとなくわかります

一応多言語対応していて、gitのbranchはmasterがc++, Cはc, rubyはrubyです。他の言語を使いたいときは2048/2048.rbの.popen("./AI")と、exec.shまたはbench.shの-exec "./AI"を書き換えてください

実行
g++、ruby環境を整えて、
 ./exec.sh
すれば一回分、
 ./bench.sh
 すればn回(初期値は100、bench.shのnumber_of_reputationを書き換えれば回数は調整できる)
のスコアが取れます
 ./bench.sh log
とするとログがpast_logs/時刻名/に作られます
