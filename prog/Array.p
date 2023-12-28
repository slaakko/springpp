program Array;
type
  Shape = object
    constructor();
  end;
var
  a: array of Shape;
  s: Shape;

constructor Shape();
begin
end;

begin
  a := new Shape[4];
  a[0] := new Shape();
  s := a[0];
end.
