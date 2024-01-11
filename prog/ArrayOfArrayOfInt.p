program ArrayOfArrayOfInt();
type
  IntArray = array of integer;
  ArrayOfIntArray = array of IntArray;
var
  i: IntArray;
  a: ArrayOfIntArray;

procedure PrintA(a: ArrayOfIntArray);
var
  n, m, index, k: integer;
  i: IntArray;
begin
  n := a.Length;
  for index := 0 to n - 1 do 
  begin
    i := a[index];
    m := i.Length;
    for k := 0 to m - 1 do Writeln(index, ', ', k, ': ', i[k]);
  end;
end;

begin
  i := new integer[3];
  i[0] := 0;
  i[1] := 1;
  i[2] := 2;
  a := new IntArray[1];
  a[0] := i;
  PrintA(a);
end.
