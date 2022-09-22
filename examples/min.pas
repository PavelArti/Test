{Test ArrMin program}
program ArrMin;
var 
    a: array[1..100] of integer;
    min, i, n: integer;
begin
    write('Enter array size: ');
    readln(n);
    i := 1;
    while i <= n do
    begin
       write('Enter ',i,' element: ');
       read(a[i]);
       i += 1;
    end;
    min := a[1];
    i := 2;
    while i <= n do
    begin
        if a[i]<min then
            min := a[i];
        i += 1;
    end;
    writeln('Min: ', min);
end.

