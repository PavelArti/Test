{Test Hash program}
program Hash;
const
    b = 42;
    ten = 10;
var
    a, res : integer;
begin
    writeln('Enter value: ');
    readln(a);
    res := b * ((a * 2) - b * ((3 + 4 * ten) div 5) mod 45);
    writeln('Hash: ',res);
end.

