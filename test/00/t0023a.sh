#!/bin/sh

prog="saSampleStdInt_test"
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
int8_t signed : 1
int8_t digits : 7
int8_t max : 01111111b
int8_t min : 10000000b
int8_t -1 cast to int32_t : 0xffffffff
uint8_t signed : 0
uint8_t digits : 8
uint8_t max : 11111111b
uint8_t min : 00000000b
int16_t signed : 1
int16_t digits : 15
int16_t max : 0x7fff
convert int16_t max to int32_t : 0x7fff
convert int16_t max to uint32_t : 0x7fff
int16_t min : 0x8000
convert int16_t min to int32_t : 0xffff8000
convert int16_t min to uint32_t (?) : 0xffff8000
uint16_t signed : 0
uint16_t digits : 16
uint16_t max : 0xffff
convert uint16_t max to int32_t : 0xffff
convert uint16_t max to uint32_t : 0xffff
uint16_t min : 0x0
convert uint16_t min to int32_t : 0x0
convert uint16_t min to uint32_t : 0x0
int32_t signed : 1
int32_t digits : 31
int32_t max : 0x7fffffff
int32_t min : 0x80000000
uint32_t signed : 0
uint32_t digits : 32
uint32_t max : 0xffffffff
uint32_t min : 0x0
int64_t digits : 63
int64_t signed : 1
int64_t max : 0x7fffffffffffffff
int64_t min : 0x8000000000000000
uint64_t signed : 0
uint64_t digits : 64
uint64_t max : 0xffffffffffffffff
uint64_t min : 0x0
ssize_t signed : 1
ssize_t digits : 63
ssize_t max : 0x7fffffffffffffff
ssize_t min : 0x8000000000000000
size_t signed : 0
size_t digits : 64
size_t max : 0xffffffffffffffff
size_t min : 0x0
float signed : 1
float digits : 24
Set precision to 12 digits
float max : 3.40282346639e+38
float min (?) : 1.17549435082e-38
float -max : -3.40282346639e+38
Set precision to 40 digits
float max : 340282346638528859811704183484516925440
float min (?) : 1.175494350822287507968736537222245677819e-38
float -max : -340282346638528859811704183484516925440
Set precision to 12 digits
double signed : 1
double digits : 53
double max : 1.79769313486e+308
double min (?) : 2.22507385851e-308
double -max : -1.79769313486e+308
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
