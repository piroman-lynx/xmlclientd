Event(-) 0 EVENT_ANY DO B
Event(-) 1 EVENT_ANY DO B
Event(-) 2 EVENT_ANY DO B
Event(-) 3 EVENT_ANY LOG "unable to conenct"
Event(-) 4 EVENT_ANY LOG "unable to get /"
Event(-) 5 EVENT_ANY LOG "unable to recaive"
Do(A)
Connect(0) tcp://127.0.0.1:81
Writeln(1) GET /
Read(2) ALL
Close(9) ALL
Die(A)
Do(B)
Close(9) ALL
Connect(3) tcp://127.0.0.1:80
Writeln(4) GET /
Read(5) ALL
Close(9) ALL
Die(B)
