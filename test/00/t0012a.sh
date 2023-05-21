#!/bin/sh

prog="saSampleTo_test"
tmp=/tmp/$$
here=`pwd`
bin=$here/bin
if [ $? -ne 0 ]; then echo "Failed pwd"; exit 1; fi


fail()
{
        echo FAILED $0 $prog 1>&2
        cd $here
        rm -rf $tmp
        exit 1
}


pass()
{
        echo PASSED $0 $prog
        cd $here
        rm -rf $tmp
        exit 0
}

trap "fail" 1 2 3 15
mkdir $tmp
if [ $? -ne 0 ]; then echo "Failed mkdir"; exit 1; fi
cd $tmp
if [ $? -ne 0 ]; then echo "Failed cd"; fail; fi

#
# the output should look like this
#
cat > test.ok << 'EOF'
10000
20000
30000
40000
50000
60000
70000
-10000
-20000
-30000
-40000
-50000
-60000
-70000
50
25
12.5
6.25
3.125
1.5625
0.78125
0.390625
0.1953125
0.09765625
256
512
1024
2048
4096
8192
16384
32768
65536
131072
0.048828125
0.024414062
0.012207031
0.0061035156
0.0030517578
0.0015258789
0.00076293945
0.00038146973
0.00019073486
9.5367432e-05
4.7683716e-05
2.3841858e-05
EOF
if [ $? -ne 0 ]; then echo "Failed output cat"; fail; fi

#
# run and see if the results match
#
echo "Running $prog"

$VALGRIND_CMD $bin/$prog > test.out 2>&1
if [ $? -ne 0 ]; then echo "Failed running $prog"; fail; fi
diff test.ok test.out
if [ $? -ne 0 ]; then echo "Failed diff"; fail; fi


#
# this much worked
#
pass
