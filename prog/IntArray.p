program IntArray();
type
  IntArray = array of integer;
var
  a: IntArray;

procedure PrintA(a: IntArray);
var
  i, n: integer;
begin
  n := a.Length;
  for i := 0 to n - 1 do Writeln(a[i]);
end;

begin
  a := new integer[3];
  a[0] := 1;
  a[1] := 2;
  a[2] := 3;
  PrintA(a);
end.
