#!/bin/sh

prog="saSample_test"
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
Test constructors
saInputMax=1.03846e+34
saInputMin=-1.03846e+34
saInternalMax=3.40282e+38
saInternalMin=-3.40282e+38
saInternalOne=32768
Default: a=0
Explicitly initialise to 0 : b=0
Explicitly initialise to 0.0 : c=0
Explicitly initialise to 1 : d=1
Explicitly initialise to -1 : e=-1
Test addressof : f = &e : *f=-1
Explicitly initialise to 1.03846e+34 : g=1.03846e+34
Explicitly initialise to -1.03846e+34 : h=-1.03846e+34
Explicitly initialise to 1.03846e+34+1
i=1.03846e+34
Explicitly initialise to -1.03846e+34-1
j=-1.03846e+34
Explicitly initialise to 1.03846e+34 : k=1.03846e+34
Explicitly initialise to 1.0f/32768 : l=3.05176e-05
Explicitly initialise to -1.0f/32768 : m=-3.05176e-05
Explicitly initialise to 1.0f/32769
n = 3.05166e-05
Explicitly initialise to -1.0f/32769
Copy constructor : p = 1 : q = *p : q=1
Copy constructor : r(*p) : r=1
Test assignments
Assignment : s=1 : s=1
Assignment : s=0.5 : s=0.5
Assignment : s+=0 : s=0.5
Assignment : s+=1.0 : s=1.5
Assignment : s+=-2.0 : s=-0.5
Assignment : s+=1 : s=0.5
Assignment : s+=-2 : s=-1.5
Assignment : s=1.03846e+34 : s+=1
Assignment : s=1.03846e+34 : s-=-1
Assignment : s=-1.03846e+34 : s+=-1
Assignment : s=-1.03846e+34 : s-=1
Assignment : s=1 : s*=2 : s=2
Assignment : s=1.0 : s*=2.0 : s=2
Assignment : s=1.0 : s*=-2 : s=-2
Assignment : s=1 : s*=-2.0 : s=-2
Assignment : s=1 : s*=2 : s=-2
Assignment : s=-1 : s*=2.0 : s=-2
Assignment : s=-1.0 : s*=-2 : s=2
Assignment : s=-1.0 : s*=-2.0 : s=2
Assignment : s=1111; s*=3.33333 : s=3703.33
Assignment : s=1.03846e+34 : s*=2.0
Assignment : s=1.03846e+34 : s*=-2.0
Assignment : s=1.0f/32768 : s/=2
Assignment : s=1/0
saSample: divide by zero
Assignment : s=0; s/=2 : s=0
Assignment : s=1.0; s/=2 : s=0.5
Assignment : s=1.0; s/=2.0 : s=0.5
Assignment : s=-1.0; s/=2 : s=-0.5
Assignment : s=-1; s/=2.0 : s=-0.5
Assignment : s=1.0; s/=-2 : s=-0.5
Assignment : s=1.0; s/=-2.0 : s=-0.5
Assignment : s=1; s/=-2 : s=0.5
Assignment : s=-1.0; s/=-2.0 : s=0.5
Assignment : s=5555; s/=3333 : s=1.66667
Assignment : s=1 : s/=1.0 : s=1
Assignment : s/=1 : s=1
Assignment : s=1.03846e+34 ; s/=1 : s=1.03846e+34
Test arithmetic
Arithmetic t=-(t=1); t=-1
Arithmetic t=-(t=-1); t=1
Arithmetic t=-(t=2); t=-2
Arithmetic t=-(t=-2); t=2
Arithmetic t=-(t=0.5); t=-0.5
Arithmetic t=-(t=-0.5); t=0.5
Arithmetic t=-(t=saInputMax); t=-1.03846e+34
Arithmetic t=-(t=saInputMin);
t=1.03846e+34
Arithmetic 1+2 : v=3
Arithmetic 1+(-2) : v=-1
Arithmetic (-1)+2 : v=1
Arithmetic (-1)+(-2) : v=-3
Arithmetic -(1) : v=1 u=-1
Arithmetic -(-1) : v=-1 u=1
Arithmetic 1-2 : v=-1
Arithmetic 1-(-2) : v=3
Arithmetic (-1)-2 : v=-3
Arithmetic (-1)-(-2) : v=1
Arithmetic 2.0*3.0 : v=6
Arithmetic 2.0*(-3.0) : v=-6
Arithmetic (-2.0)*3.0 : v=-6
Arithmetic (-2.0)*(-3.0) : v=6
Arithmetic 2.0/4.0 : v=0.5
Arithmetic 2.0/(-4.0) : v=-0.5
Arithmetic (-2.0)/4.0 : v=-0.5
Arithmetic (-2.0)/(-4.0) : v=0.5
Arithmetic 2+(u=-4) : v=-2
Arithmetic (u=-4)+2 : v=-2
Arithmetic 0.5+(u=-4) : v=-3.5
Arithmetic (u=-4)+0.5 : v=-3.5
Arithmetic 2-(u=-4) : v=6
Arithmetic (u=-4)-2 : v=-6
Arithmetic 0.5-(u=4) : v=-3.5
Arithmetic (u=-4)-0.5 : v=-4.5
Arithmetic 2*(u=-4) : v=-8
Arithmetic (u=-4)*2 : v=-8
Arithmetic 0.5*(u=-4) : v=-2
Arithmetic (u=-4)*0.5 : v=-2
Arithmetic 2/(u=-4) : v=-0.5
Arithmetic (u=-4)/2 : v=-2
Arithmetic 0.5/(u=-4) : v=-0.125
Arithmetic (u=-4)/0.5 : v=-8
Test comparison
Comparison : w == 0
Comparison : w != 0
Comparison : w < x
Comparison : w > 0
Comparison : w >= 0
Comparison : w <= 0
Comparison : w < 0
Comparison : w == x
Comparison : w != x
Comparison : w > x
Comparison : w >= x
Comparison : w <= x
Comparison : w < x
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
