{Test Strings program}
program ArrMin;
var
    s1, s2, res : string;
    ch : char;
begin
    writeln('Enter first string: ');
    readln(s1);
    writeln('Enter second string: ');
    readln(s2);
    ch := 'X';
    res := 'a';
    res += s1;
    res += ' ';
    res += s2;
    res += s1;
    writeln(res);
    res := 'abcd';
    res[3] := ch;
    writeln(res);
end.

