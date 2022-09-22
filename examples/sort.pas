{Test BubbleSort program}
program BubbleSort;
var
   arr : array[1..100] of integer;
   i, j, buf, N : integer;
begin
   write('Enter array size: ');
   readln(N);
   i := 1;
   while (i <= N) do
       begin
       write('Enter ',i,' element: ');
       readln(arr[i]);
       i += 1;
       end;
 
   i := 1;
   while (i < N) do
       begin
       j := 1;
       while (j <= N - i) do
            begin
            if (arr[j] > arr[j + 1]) then
                begin
                buf := arr[j];
                arr[j] := arr[j + 1];
                arr[j + 1] := buf;
                end;
            j += 1;
            end;
       i += 1;
       end;
 
   writeln('Sorted array:');
   i := 1;
   while (i < N) do
       begin
       write(arr[i],' ');
       i += 1;
       end;
    writeln(arr[i]);
end.


