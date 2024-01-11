program VarParam();
var
  y: integer;
  s: string;

procedure foo(var x: integer; var t: string);
begin
  x := 1;
  t := 'foo';
end;

begin
  y := 0;
  Writeln(y);
  s := 'prog';
  Writeln(s);
  foo(y, s);
  Writeln(y);
  Writeln(s);
end.
