program Level;

var
  level0Var: integer;

procedure Level1();
var
  localVar: integer;
begin
  localVar := level0Var;
end;

begin
  level0Var := 12345;
  Level1();
end.
