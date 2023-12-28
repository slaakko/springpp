program Add;
var 
  x: integer;

function add(x, y: integer): integer;
begin
  add := x + y;
end;

begin
  x := add(1, 2);
end.
