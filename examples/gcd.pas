{Test GCD program}
program GCD;
var
    z, i, a, b, m: integer;
begin
    writeln('Enter a and b');
    readln(a,b);
    if a < b then 
        m := a
    else
        m := b;
    i := 1;
    while i <= m do
    begin
        if (a mod i = 0) then
            if (b mod i = 0) then 
                z := i;
        i += 1;
    end;
    writeln('GCD: ', z);
end.


