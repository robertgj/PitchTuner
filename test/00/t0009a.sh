#!/bin/sh

prog="saCircBuff_test"
tmp=/tmp/$$
here=`pwd`
bin=$here/bin

# Sanity checks
if [ $? -ne 0 ]; then echo "Failed pwd"; exit 1; fi
if [ ! -f $bin/$prog ] ; then echo "Didnt find "$prog; exit 2; fi

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
Instantiating saCircBuff<char>(123)
Instantiating saCircBuff<int8_t>(123)
Success!
Testing nullptr argument
Caught exception in bufnull
Insert() from nullptr
Caught exception in bufnull
Extract() from nullptr
Success!
Testing  saCircBuff<int16_t>(100)
num_written: Caught exception in buf1
Insert() overrun
num_read: Caught exception in buf1
Extract() underrun
num_read: 1
Success! Read 10
num_written: 100
Caught exception in bufsz
Insert() overrun
num_read: Caught exception in bufsz after Clear()
Extract() underrun
num_read: 100
Compare 100 success!
total_num_written = 114
total_num_read = 114
Compare 33 success!
Testing threaded int16_t write and int16_t read 
Testing threaded int16_t write and int32_t read 
Testing threaded saSample write and int32_t read 
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
