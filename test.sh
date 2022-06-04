#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./my_c "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5; "
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'
assert 3 "a = 3;"
assert 6 "a = 1; b = 2 + 3; a + b;" 
assert 3 "return 3;"
assert 13 "a = 3; b = 5 * 6 - 7; return (a + b) / 2;"

assert 3 "a = 3; if(a == 3) return a; return 5;"
assert 5 "a = 5; if(a != 3) return a; return 3;"
assert 4 "a = 3; b = 1; if(a+b == 4) return a+b; return 0;"
assert 2 "a = 3; b = 1; if(a == b) return a + b; else return a - b;"
assert 4 "a = 3; b = 1; if(a != b) return a + b; else return a - b;"

assert 5 "a = 0; while(a < 5) a = a + 1; return a;"
assert 0 "a = 0; while(a) a = a + 1; return a;"

assert 5 "b = 0; for(a = 0; a < 5; a = a + 1) b = b + 1; return b;"
assert 5 "b = 5; for(; ;) return b;"

assert 5 "b = 0; for(; b < 3;) b = b + 1; while(b < 5) b = b + 1; return b;"

assert 5 "a = 0; while(a < 5) { a = a + 1; } return 5;"
assert 5 "b = 0; for(; b < 5;) { b = b + 1; } return b;"
assert 9 "foo = 0; bar = 0; while(foo < 3) { foo = foo + 1; bar = bar + 2; } return foo+bar;"

echo OK
