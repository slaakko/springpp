program Nil;
type
  o = object
    a: integer;
    procedure foo(); 
  end;
var
  x: o;

procedure o.foo();
var
  b: integer;
begin
  b := a;
end;

begin
  x.foo();
end.
