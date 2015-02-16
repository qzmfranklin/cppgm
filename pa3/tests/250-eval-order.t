true?5:5/0 // only evaluate one conditional branch
true?5/0:5

5||(5%0)   // only evaluate second operand of || if first false
0||(5%0)

0&&(5/0)   // only evaluate second operand of && if first true
5&&(5/0)

true?5:5<<64
true?5<<64:5

5||(5<<64)
0||(5<<64)

0&&(5<<64) 
5&&(5<<64)

true?5:5<<-1
true?5<<-1:5

5||(5<<-1)
0||(5<<-1)

0&&(5<<-1) 
5&&(5<<-1)

